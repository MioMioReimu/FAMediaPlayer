/*
 * FAAudioDecoder.h
 *
 *  Created on: 2013年9月15日
 *      Author: tlm
 */

#ifndef FAAUDIODECODER_H_
#define FAAUDIODECODER_H_

#include "thread.h"
#include "FFMpegPlayer.h"
class FAAudioDecoder : public Thread {
public:
	FAAudioDecoder();
	virtual ~FAAudioDecoder();
	void run();
	int play();
	int pause();
	int stop();

	int releaseAllPkts();
	int setCurrentPosition(int msec);
	int getCurrentPosition();
	int setDecoder(AVFormatContext *fmtCtx,int streamIndex,
			FAPktList* audioPktList,FAPktList *pktList,
			volatile FAplayer_State *state,Sigmore *deMuxerAndDecoderSigmore
			,Sigmore *audioDecoderSigmore);
	void releaseAudioPlayer();
	void releaseDecoder();
	void release(){
		this->releaseAudioPlayer();
		this->releaseDecoder();
	}
	int prepare();
public:
	const static int DEFAULT_SAMPLE_RATE=192000;
	const static AVSampleFormat DEFAULT_SAMPLE_FORMAT=AV_SAMPLE_FMT_S16;
	const static int DEFAULT_CHANNEL_LAYOUT=AV_CH_LAYOUT_STEREO;
	const static int DEFAULT_NB_SAMPLES=1000;
	const static int NB_AUDIO_BUFFER_QUEUE=16;
	//this callback is called every time a buffer finishes playing
	void bufferQueuePlayerCallback(SLAndroidSimpleBufferQueueItf bq, void* context);

private:
	int64_t currentPosition;

	volatile FAplayer_State *state;

	//SLES Objects
	SLObjectItf audioEngineObject;
	SLEngineItf audioEngine;

	//音频流结构体
	AVStream *audioStream;
	int audioStreamIndex;
	//音频解码器环境结构
	AVCodecContext *audioDecodeCtx;

	AVFrame * frame;

	SwrContext *audioResampleCtx;
	//当前播放的音频流的声道数
	unsigned int nbOfChannels;
	//当前播放的音频流的声道模式
	unsigned int channelLayout;
	//当前音频流的采样数据位数
	unsigned int bitsPerSample;
	//当前音频流的采样率
	unsigned int sampleRate;

	//进行重采样时需要的缓冲区
	uint8_t **audioBuffer;
	int audioBufferLinesize;
	//缓冲区的大小
	int nbSamplesOfBuffer;

	//声音输出的结构体 必不可少的一个结构体
	SLObjectItf outputMixObject;
	//环境混响控制器
	SLEnvironmentalReverbItf outputMixEnvironmentalReverb;

	//SLES 的带有缓冲队列的播放器
	SLObjectItf audioBufferQueuePlayerObject;
	SLPlayItf audioBufferQueuePlayer;
	SLAndroidSimpleBufferQueueItf audioBufferQueue;
	SLEffectSendItf audioBufferQueuePlayerEffectSend;
	SLVolumeItf audioBufferQueuePlayerVolume;

	Sigmore * deMuxerAndDecoderSigmore;
	Sigmore *audioDecoderSigmore;

	FAPktList* audioPktList;
	FAPktList * pktList;
	AVPacket * currentPkt;
	Mutex mLock;
	//BufferQueue callback function pointer
	void (* audioBufferQueueCallbackPointer)(SLAndroidSimpleBufferQueueItf bufferQueue,void *pContext);
};

#endif /* FAAUDIODECODER_H_ */
