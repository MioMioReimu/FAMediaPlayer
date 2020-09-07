/*
 * FFMpegPlayer.cpp
 *
 *  Created on: 2013年9月4日
 *      Author: tlm
 */

#include "FFMpegPlayer.h"
#include "FAAudioDecoder.h"
#include "FAVideoDecoder.h"
#include <android/log.h>
#include <unistd.h>
FFMpegPlayer ffmpegPlayer;
void bufferQueueCallback(SLAndroidSimpleBufferQueueItf bufferQueue,
		void *pContext) {
	//FFMPEG_PLAYER_DEBUG_PRINT("Begin callback");
	ffmpegPlayer.audioPlayer->bufferQueuePlayerCallback(bufferQueue, pContext);
}
FFMpegPlayer::FFMpegPlayer() {
	this->fmtCtx = NULL;
	this->audioStream = NULL;
	this->audioDecodeCtx = NULL;
	this->audioStreamIndex = -1;
	this->videoStream = NULL;
	this->videoDecodeCtx = NULL;
	this->videoStreamIndex = -1;

	this->pktList = new FAPktList(NB_PKT_QUEUE);
	this->audioPktList = new FAPktList();
	this->videoPktList = new FAPktList();
	this->state = UN_INITED;
	this->mainThreadSigmore = new Sigmore();
	this->deMuxerAndDecoderSigmore = new Sigmore();
	this->videoDecoderSigmore = new Sigmore();
	this->audioDecoderSigmore = new Sigmore();
	this->audioPlayer = new FAAudioDecoder();
	this->videoPlayer = new FAVideoDecoder();
	av_register_all();
}

FFMpegPlayer::~FFMpegPlayer() {
	delete pktList;
	delete audioPktList;
	delete videoPktList;
	delete mainThreadSigmore;
	delete videoDecoderSigmore;
	delete audioDecoderSigmore;
	delete audioPlayer;
	delete videoPlayer;
}

int FFMpegPlayer::getDuration() {
	if (this->state >= INITED) {
		return fmtCtx->duration / (AV_TIME_BASE / 1000);
	} else {
		return 0;
	}
}
int FFMpegPlayer::getVideoHeight() {
	if (this->state >= INITED) {
		return videoDecodeCtx->height;
	} else {
		return 0;
	}
}
int FFMpegPlayer::getVideoWidth() {
	if (this->state >= INITED) {
		return videoDecodeCtx->width;
	} else {
		return 0;
	}
}

int FFMpegPlayer::getCurrentPosition() {
	if (this->state >= PREPARED) {
		return this->audioPlayer->getCurrentPosition();
	} else {
		return 0;
	}
}

int openCodecContext(AVFormatContext *fmtCtx, enum AVMediaType type) {
	int ret;
	ret = av_find_best_stream(fmtCtx, type, -1, -1, NULL, 0);
	if (ret < 0) {
		FFMPEG_PLAYER_DEBUG_PRINT(
				"Could not find %s stream in input file %s \n",
				av_get_media_type_string(type), fmtCtx->filename);
		return -1;
	} else {
		AVCodecContext *dec_ctx = fmtCtx->streams[ret]->codec;
		AVCodec *dec = avcodec_find_decoder(dec_ctx->codec_id);
		if (!dec) {
			FFMPEG_PLAYER_DEBUG_PRINT("Failed to find %s codec\n",
					av_get_media_type_string(type));
			return -1;
		}
		if (avcodec_open2(dec_ctx, dec, NULL) < 0) {
			FFMPEG_PLAYER_DEBUG_PRINT("Failed to open %s codec \n",
					av_get_media_type_string(type));
			return -1;
		}
		FFMPEG_PLAYER_DEBUG_PRINT("Stream Type:%s\t codec:%s\n",
				av_get_media_type_string(type), dec->name);
		return ret;
	}
}

int FFMpegPlayer::setDataSource(const char* filepath) {
	int ret;
	if (avformat_open_input(&this->fmtCtx, filepath, NULL, NULL) < 0) {
		FFMPEG_PLAYER_DEBUG_PRINT("Could not open source file %s\n", filepath);
		return IO_ERROR_CANNOT_OPEN_FILE;
	}
	FFMPEG_PLAYER_DEBUG_PRINT("Open source file success\n");

	if (avformat_find_stream_info(fmtCtx, NULL) < 0) {
		FFMPEG_PLAYER_DEBUG_PRINT("Could not find stream information\n");
		return IO_ERROR_CANNOT_FIND_STREAM;
	}
	FFMPEG_PLAYER_DEBUG_PRINT("Find Stream infor success\n");

	//根据获取的容器格式信息进行视频解码器的初始化
	videoStreamIndex = openCodecContext(fmtCtx, AVMEDIA_TYPE_VIDEO);
	if (videoStreamIndex < 0) {
		FFMPEG_PLAYER_DEBUG_PRINT("Could not find decode!\n");
		return IO_ERROR_CANNOT_FIND_DECODER;
	}
	this->videoStream = fmtCtx->streams[videoStreamIndex];
	this->videoDecodeCtx = videoStream->codec;

	audioStreamIndex = openCodecContext(fmtCtx, AVMEDIA_TYPE_AUDIO);
	//根据获取的容器格式信息进行音频解码器的初始化
	if (audioStreamIndex < 0) {
		FFMPEG_PLAYER_DEBUG_PRINT("Could not find decode!\n");
		return IO_ERROR_CANNOT_FIND_DECODER;
	}
	audioStream = fmtCtx->streams[audioStreamIndex];
	audioDecodeCtx = audioStream->codec;
	FFMPEG_PLAYER_DEBUG_PRINT("Init Streams Decode Success\n");

	FFMPEG_PLAYER_DEBUG_PRINT("init pkt Success\n");

	ret = this->audioPlayer->setDecoder(fmtCtx, audioStreamIndex, audioPktList,
			pktList, &state, deMuxerAndDecoderSigmore, audioDecoderSigmore);
	if (ret != SUCCESS) {
		return ret;
	}
	ret = this->videoPlayer->setDecoder(fmtCtx, videoStreamIndex, videoPktList,
			pktList, &state, deMuxerAndDecoderSigmore, videoDecoderSigmore,
			mainThreadSigmore);
	if (ret != SUCCESS) {
		return ret;
	}
	this->state = INITED;
	return SUCCESS;
}

int FFMpegPlayer::prepare() {
	int result;
	if ((result = this->audioPlayer->prepare()) != SUCCESS) {
		return result;
	}
	if ((result = this->videoPlayer->prepare()) != SUCCESS) {
		return result;
	}
	FFMPEG_PLAYER_DEBUG_PRINT("Success to prepare player!\n");
	this->state = PREPARED;
	return result;
}

int FFMpegPlayer::play() {
	if (state == PAUSED) {
		this->state = PLAYING;
		this->mainThreadSigmore->wakeAll();
		this->audioPlayer->play();
		this->videoPlayer->play();
	} else if ((state == SEEKING) || (state == PREPARED)) {
		this->state = PLAYING;
		if (!this->isRunning()) {
			this->start();
			FFMPEG_PLAYER_DEBUG_PRINT("First Start Demuxer thread!\n");
		};
		this->audioPlayer->play();
		this->videoPlayer->play();
	}
}

int FFMpegPlayer::pause() {
	this->state = PAUSED;
	this->audioPlayer->pause();
	this->videoPlayer->pause();
}

void FFMpegPlayer::run() {
	while (1) {
		if (state == PLAYING) {
			AVPacket *pkt = this->pktList->pop_front();
			//FFMPEG_PLAYER_DEBUG_PRINT("demuxer :free Pkt num:%d\n",
			//		pktList->size());
			if (pkt == NULL) {
				//FFMPEG_PLAYER_DEBUG_PRINT("free pkt list is null!\n");
				deMuxerAndDecoderSigmore->wait();
			} else {
				av_read_frame(fmtCtx, pkt);
				//FFMPEG_PLAYER_DEBUG_PRINT("read One frame!\n");
				if (pkt->stream_index == this->audioStreamIndex) {
					this->audioPktList->push_back(pkt);
					//FFMPEG_PLAYER_DEBUG_PRINT("Success to push audio pkt:%d\n",
					//		audioPktList->size());
					audioDecoderSigmore->wake();

				} else if (pkt->stream_index == this->videoStreamIndex) {
					//FFMPEG_PLAYER_DEBUG_PRINT("Begin to Push video Pkt!\n");
					this->videoPktList->push_back(pkt);
					pkt = NULL;
					//FFMPEG_PLAYER_DEBUG_PRINT("Success to push video pkt:%d!\n",
					//		videoPktList->size());
					videoDecoderSigmore->wake();
				} else {
					pkt->data += pkt->size;
					pkt->size = 0;
					av_free_packet(pkt);
					pktList->push_back(pkt);
					pkt = NULL;
				}
			}
		} else if (state == PAUSED) {
			this->mainThreadSigmore->wait();
		} else {
			break;
		}
	}
}

int FFMpegPlayer::stop(){
	int ret;
	stateLock.lock();
	if(state == PLAYING ||PAUSED) {
		this->state=STOPPED;
		deMuxerAndDecoderSigmore->wakeAll();
		this->join();
		audioDecoderSigmore->wakeAll();
		this->audioPlayer->stop();
		this->audioPlayer->join();

		videoDecoderSigmore->wakeAll();
		videoPlayer->stop();
		videoPlayer->join();
		this->audioPlayer->releaseAllPkts();
		this->videoPlayer->releaseAllPkts();
		this->audioPlayer->setCurrentPosition(0);
		ret=SUCCESS;
	}else{
		ret=ILLEGAL_STATE;
	}
	stateLock.unlock();
}

void FFMpegPlayer::release() {
	stateLock.lock();
	if(state>=PLAYING){
		stateLock.unlock();
		this->stop();
		stateLock.lock();
	}
	this->audioPlayer->release();
	this->videoPlayer->release();
	avformat_close_input(&fmtCtx);
	this->videoStream=NULL;
	this->audioStream=NULL;
	this->videoStreamIndex=-1;
	this->audioStreamIndex=-1;
	stateLock.unlock();
}

int FFMpegPlayer::seekTo(int msec) {
	if (!stateLock.tryLock()) {
		if (state == PLAYING || PAUSED) {
			this->state = SEEKING;
			deMuxerAndDecoderSigmore->wakeAll();
			this->join();
			FFMPEG_PLAYER_DEBUG_PRINT("demuxer thread stop");
			this->audioDecoderSigmore->wakeAll();
			this->audioPlayer->join();
			this->audioPlayer->pause();
			this->audioPlayer->join();
			FFMPEG_PLAYER_DEBUG_PRINT("audio thread stop");
			this->videoDecoderSigmore->wakeAll();
			this->videoPlayer->pause();
			this->videoPlayer->join();
			FFMPEG_PLAYER_DEBUG_PRINT("video thread stop");
			this->audioPlayer->releaseAllPkts();
			this->videoPlayer->releaseAllPkts();
			//AV_TIME_BASE=1000 000
			if (av_seek_frame(fmtCtx, -1,
					msec * 1000
							, AVSEEK_FLAG_BACKWARD)
					>=0) {
				FFMPEG_PLAYER_DEBUG_PRINT("seek Success%.2f!\n",msec/1000.0f);
			}
			this->audioPlayer->setCurrentPosition(msec);
			play();
			usleep(100);
		}
		stateLock.unlock();
	}
}
