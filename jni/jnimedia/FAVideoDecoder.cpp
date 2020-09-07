/*
 * FAVideoDecoder.cpp
 *
 *  Created on: 2013年9月15日
 *      Author: tlm
 */
#include <android/log.h>
#include "FAVideoDecoder.h"

FAVideoDecoder::FAVideoDecoder() {
	// TODO Auto-generated constructor stub
	this->currentPkt = NULL;
	this->frame = NULL;
	this->pWindow = NULL;
	for (int i = 0; i < 4; i++) {
		this->videoDecodeDataLinesize[i] = 0;
	}
	findKeyFrame = 1;
}

FAVideoDecoder::~FAVideoDecoder() {
	// TODO Auto-generated destructor stub
}

int FAVideoDecoder::windowResize(int width, int height) {
	this->windowWidth = width;
	this->windowHeight = height;
	float src_aspect = (float) videoDecodeCtx->width
			/ (float) videoDecodeCtx->height;
	float dst_aspect = (float) width / (float) height;
	if ((src_aspect - dst_aspect) <- 0.001f) {
		dstFrameHeight = height;
		dstFrameWidth = (int) (dstFrameHeight
				* ((float) videoDecodeCtx->width
						/ (float) videoDecodeCtx->height));
		widthOffset = (width - dstFrameWidth) / 2;
		heightOffset = 0;
	} else if ((src_aspect - dst_aspect) > 0.001f) {
		dstFrameWidth = width;
		dstFrameHeight = (int) (dstFrameWidth
				* ((float) videoDecodeCtx->height
						/ (float) videoDecodeCtx->width));
		widthOffset = 0;
		heightOffset = (height - dstFrameHeight) / 2;
	} else {
		dstFrameHeight = height;
		dstFrameWidth = width;
		widthOffset = heightOffset = 0;
	}
	FFMPEG_PLAYER_DEBUG_PRINT("frameSize :%dx%d\toffsets :%dx%d\n",
			dstFrameWidth, dstFrameHeight, widthOffset, heightOffset);
	FFMPEG_PLAYER_DEBUG_PRINT("videoSize :%dx%d\t",
			videoDecodeCtx->width, videoDecodeCtx->height);
	if (!swsCtx) {
		sws_freeContext(swsCtx);
		swsCtx = NULL;
	}
	swsCtx = sws_getContext(videoDecodeCtx->width, videoDecodeCtx->height,
			videoDecodeCtx->pix_fmt, dstFrameWidth, dstFrameHeight,
			AV_PIX_FMT_RGB565LE,
			SWS_BILINEAR, NULL, NULL, NULL);
	if (!swsCtx) {
		FFMPEG_PLAYER_DEBUG_PRINT("Impossible to create scale "
				"context for the converson fmt:%s s:%dx%d ->fmt:%s s:%dx%d\n",
				av_get_pix_fmt_name(videoDecodeCtx->pix_fmt),
				videoDecodeCtx->width, videoDecodeCtx->height,
				av_get_pix_fmt_name(AV_PIX_FMT_RGB565LE), dstFrameWidth,
				dstFrameHeight);
		return FFMpegPlayer::SWSCALE_CREATE_CONTEXT_ERROR;
	}
	FFMPEG_PLAYER_DEBUG_PRINT("Success to create scale "
			"context for the converson fmt:%s s:%dx%d ->fmt:%s s:%dx%d\n",
			av_get_pix_fmt_name(videoDecodeCtx->pix_fmt), videoDecodeCtx->width,
			videoDecodeCtx->height, av_get_pix_fmt_name(AV_PIX_FMT_RGB565LE),
			dstFrameWidth, dstFrameHeight);
	dirtyRect.left = dirtyRect.top = 0;
	dirtyRect.right = width;
	dirtyRect.bottom = height;
	return FFMpegPlayer::SUCCESS;
}

int FAVideoDecoder::prepare() {
	int ret;
	int width = ANativeWindow_getWidth(pWindow);
	int height = ANativeWindow_getHeight(pWindow);
	ret = windowResize(width, height);
	FFMPEG_PLAYER_DEBUG_PRINT("Success prepare Video decoder!\n");
	return ret;
}
int FAVideoDecoder::setDecoder(AVFormatContext *fmtCtx, int streamIndex,
		FAPktList* videoPktList, FAPktList *pktList, volatile FAplayer_State *state,
		Sigmore *deMuxerAndDecoderSigmore, Sigmore *videoDecoderSigmore,
		Sigmore *mainThreadSigmore) {
	this->videoStreamIndex = streamIndex;
	this->videoStream = fmtCtx->streams[streamIndex];
	this->videoPktList = videoPktList;
	this->pktList = pktList;
	this->state = state;
	this->deMuxerAndDecoderSigmore = deMuxerAndDecoderSigmore;
	this->videoDecoderSigmore = videoDecoderSigmore;
	this->mainThreadSigmore = mainThreadSigmore;
	this->videoDecodeCtx = videoStream->codec;

	this->frame = avcodec_alloc_frame();
	if (!frame) {
		FFMPEG_PLAYER_DEBUG_PRINT("Could not allocate frame\n");
		return FFMpegPlayer::MEMORY_ERROR_CANNOT_ALLOCATE_FRAME;
	}
	return FFMpegPlayer::SUCCESS;
}

void FAVideoDecoder::run() {
	while (1) {
		int got_frame = 0;
		int ret = 0;
		//FFMPEG_PLAYER_DEBUG_PRINT("Begin thread!\n");
		if (*state == PLAYING) {
			while ((got_frame == 0)&&(*state==PLAYING)) {
				if (currentPkt == NULL) {
					currentPkt = this->videoPktList->pop_front();
					if (currentPkt == NULL) {
						this->videoDecoderSigmore->wait();
					}
					if (currentPkt != NULL) {
						findKeyFrame = findKeyFrame
								| (currentPkt->flags & AV_PKT_FLAG_KEY);
						if (this->videoPktList->size() > 48) {
							findKeyFrame = 0;
							currentPkt->data += currentPkt->size;
							currentPkt->size = 0;
							av_free_packet(currentPkt);
							pktList->push_back(currentPkt);
							currentPkt = NULL;
							for (int i = 0; i < 32; i++) {
								AVPacket *p = videoPktList->pop_front();
								p->data += p->size;
								p->size -= p->size;
								av_free_packet(p);
								this->pktList->push_back(p);
								deMuxerAndDecoderSigmore->wake();
							}
							for (int i = 0; i < 15; i++) {
								currentPkt = videoPktList->pop_front();
								if ((currentPkt->flags & AV_PKT_FLAG_KEY)
										!= AV_PKT_FLAG_KEY ) {
									//FFMPEG_PLAYER_DEBUG_PRINT("drop not key pkt%d",i);
									currentPkt->data += currentPkt->size;
									currentPkt->size -= currentPkt->size;
									av_free_packet(currentPkt);
									this->pktList->push_back(currentPkt);
									deMuxerAndDecoderSigmore->wake();
									currentPkt = NULL;
								} else {
									findKeyFrame = 1;
									//FFMPEG_PLAYER_DEBUG_PRINT(
									//		"Find Key frame %d!\n", i);
									break;
								}
							}
						}
						//FFMPEG_PLAYER_DEBUG_PRINT("video Decode :video Pkt num:%d\n",videoPktList->size());
					}
				}
				if (currentPkt != NULL) {
					if (findKeyFrame == 1) {
						ret = avcodec_decode_video2(videoDecodeCtx, frame,
								&got_frame, currentPkt);
						if (ret < 0) {
							break;
						}
					} else {
						got_frame=0;
						ret = currentPkt->size;
						//FFMPEG_PLAYER_DEBUG_PRINT("drop not key pkt");
					}
					currentPkt->data += ret;
					currentPkt->size -= ret;
					if (currentPkt->size <= 0) {
						av_free_packet(currentPkt);
						pktList->push_back(currentPkt);
						deMuxerAndDecoderSigmore->wake();
						//FFMPEG_PLAYER_DEBUG_PRINT("video Decode :free Pkt num:%d\n",pktList->size());
						currentPkt = NULL;
					}
				}
			}
			//FFMPEG_PLAYER_DEBUG_PRINT("get One Video frame!\n");
			if (got_frame) {
				ANativeWindow_lock(this->pWindow, &this->windowBuffer,
						&dirtyRect);
				if (windowBuffer.height != this->windowHeight
						|| windowBuffer.width != windowWidth) {
					this->windowResize(windowBuffer.width, windowBuffer.height);

				}
				this->videoDecodeDataLinesize[0] = windowBuffer.stride * 2;
				void *p = windowBuffer.bits + widthOffset * 2
						+ heightOffset * windowBuffer.stride * 2;
				//FFMPEG_PLAYER_DEBUG_PRINT("window buffer start:%x",p);
				sws_scale(swsCtx, frame->data, frame->linesize, 0,
						frame->height, (uint8_t **) &(p),
						(const int*) videoDecodeDataLinesize);
				ANativeWindow_unlockAndPost(pWindow);
				//FFMPEG_PLAYER_DEBUG_PRINT("swscale height:%d\t srceen:%d\n",frame->height,videoDecodeDataLinesize[0]);
			}
		} else if(*state==PAUSED){
			this->mainThreadSigmore->wait();
		}
		else{
			break;
		}
	}
}

int FAVideoDecoder::setWindow(ANativeWindow *surfaceWindow) {
	this->pWindow = surfaceWindow;
	ANativeWindow_setBuffersGeometry(pWindow, 0, 0, WINDOW_FORMAT_RGB_565);
	this->windowHeight = ANativeWindow_getHeight(pWindow);
	this->windowWidth = ANativeWindow_getWidth(pWindow);
}

int FAVideoDecoder::releaseAllPkts() {

	while (videoPktList->size() > 0) {
		AVPacket *p = videoPktList->pop_front();
		p->data += p->size;
		p->size -= p->size;
		av_free_packet(p);
		this->pktList->push_back(p);
	}
	this->deMuxerAndDecoderSigmore->wakeAll();
}

int FAVideoDecoder::play() {
	if (!this->isRunning()) {
		this->start();
	}
	FFMPEG_PLAYER_DEBUG_PRINT("First beigin video thread!\n");
	return FFMpegPlayer::SUCCESS;
}
int FAVideoDecoder::pause() {
	return FFMpegPlayer::SUCCESS;
}
int FAVideoDecoder::stop() {
	return FFMpegPlayer::SUCCESS;
}

void FAVideoDecoder::release(){
	avcodec_close(videoDecodeCtx);
	avcodec_free_frame(&frame);
	sws_freeContext(this->swsCtx);
	swsCtx=NULL;
	this->videoStream=NULL;
	this->videoPktList=NULL;
	this->videoDecoderSigmore=NULL;
	this->mainThreadSigmore=NULL;
	this->deMuxerAndDecoderSigmore=NULL;
	this->currentPkt=NULL;
	this->pWindow=NULL;
	this->pktList=NULL;
	this->state=NULL;
	this->videoDecodeCtx=NULL;
	this->videoStreamIndex=-1;
}
