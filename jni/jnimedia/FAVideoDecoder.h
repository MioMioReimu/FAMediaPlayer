/*
 * FAVideoDecoder.h
 *
 *  Created on: 2013年9月15日
 *      Author: tlm
 */

#ifndef FAVIDEODECODER_H_
#define FAVIDEODECODER_H_

#ifdef __cplusplus
extern "C"
{
#endif
#include <android/native_window.h>
#include <android/rect.h>
#include <android/native_window_jni.h>
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
#include <libavutil/parseutils.h>
#ifdef __cplusplus
}
#endif
#include "thread.h"
#include "FFMpegPlayer.h"
class FAVideoDecoder : public Thread {
public:
	FAVideoDecoder();
	virtual ~FAVideoDecoder();
	int setDecoder(AVFormatContext *fmtCtx,int streamIndex,
			FAPktList* videoPktList,FAPktList *pktList,
			volatile FAplayer_State *state,Sigmore *deMuxerAndDecoderSigmore
			,Sigmore *videoDecoderSigmore,Sigmore *mainThreadSigmore);
	int setWindow(ANativeWindow *surfaceWindow);
	void run();
	int play();
	int pause();
	int stop();
	int prepare();

	void release();

	int releaseAllPkts();
protected:
	int windowResize(int width,int height);
private:
	AVStream *videoStream;
	int videoStreamIndex;
	//视频解码器环境结构
	AVCodecContext *videoDecodeCtx;

	volatile FAplayer_State *state;
	AVFrame * frame;
	//uint8_t *videoDecodeData[4];
	int videoBufferSize;
	int videoDecodeDataLinesize[4];
	int findKeyFrame;

	SwsContext* swsCtx;

	int windowHeight;
	int windowWidth;
	int dstFrameWidth;
	int dstFrameHeight;
	int widthOffset;
	int heightOffset;
	ARect dirtyRect;
	ANativeWindow *pWindow;
	ANativeWindow_Buffer windowBuffer;

	FAPktList* videoPktList;
	FAPktList* pktList;
	AVPacket * currentPkt;

	Sigmore * deMuxerAndDecoderSigmore;
	Sigmore * videoDecoderSigmore;
	Sigmore * mainThreadSigmore;


};

#endif /* FAVIDEODECODER_H_ */
