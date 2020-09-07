/*
 * FFMpegPlayer.h
 *
 *  Created on: 2013年9月4日
 *      Author: tlm
 */

#ifndef FFMPEGPLAYER_H_
#define FFMPEGPLAYER_H_
#define __DEBUG__
#ifdef __DEBUG__
	#define FFMPEG_PLAYER_DEBUG_PRINT(format,...) \
	__android_log_print(ANDROID_LOG_ERROR,"FFMpegPlayer",format,##__VA_ARGS__)
#else
	#define FFMPEG_PLAYER_DEBUG_PRINT(format,...)
#endif

#ifdef __cplusplus
extern "C"
{
#endif
	#include <stdint.h>
	#include <libavutil/imgutils.h>
	#include <libavutil/samplefmt.h>
/*
#if !defined PRId64 || PRI_MACROS_BROKEN
# undef PRId64
# define PRId64 "lld"
#endif*/
	#include <libavutil/timestamp.h>
	#include <libavformat/avformat.h>
	#include <libavutil/opt.h>
	#include <libavutil/channel_layout.h>
	#include <libavutil/samplefmt.h>
	#include <libswresample/swresample.h>
	#include <SLES/OpenSLES.h>
	#include <SLES/OpenSLES_Android.h>
	void bufferQueueCallback(SLAndroidSimpleBufferQueueItf bufferQueue,void *pContext);
#ifdef __cplusplus
}
#endif
#include "FAPktList.h"
class FAAudioDecoder;
class FAVideoDecoder;
enum FAplayer_State {
	UN_INITED=0,
	INITED=1,
	PREPARED=2,
	PLAYING=3,
	PAUSED=4,
	SEEKING=5,
	STOPPED=-1,
	RELEASED=-2
};
class FFMpegPlayer: public Thread{
public:
	FFMpegPlayer();
	virtual ~FFMpegPlayer();
	//设置数据源
	int setDataSource(const char* filepath);
	//准备播放器
	int prepare();
	//开始播放
	int play();
	//停止或者继续播放
	int pause();
	//停止播放器
	int stop();

	void run();
public:
	int getDuration();
	int getCurrentPosition();
	int getVideoHeight();
	int getVideoWidth();
	inline int getState(){
		return (int)state;
	}
	int seekTo(int msec);
	void reset();
	void release();
protected:


public:
	const static int SUCCESS=1;
	const static int ILLEGAL_STATE=-14;

	const static int FRAME_TYPE_VIDEO=2;
	const static int FRAME_TYPE_AUDIO=3;

	const static int IO_ERROR_CANNOT_OPEN_FILE=-1;
	const static int IO_ERROR_CANNOT_FIND_STREAM=-2;
	const static int IO_ERROR_CANNOT_FIND_DECODER=-3;
	const static int MEMORY_ERROR_CANNOT_ALLOCATE_BUFFER=-4;
	const static int MEMORY_ERROR_CANNOT_ALLOCATE_FRAME=-5;
	const static int RESAMPLE_ERROR_CANNOT_ALLOCATE_RESAMPLER=-6;
	const static int RESAMPLE_ERROR_CANNOT_INIT_RESAMPLER=-7;

	const static int FILE_DECODE_COMPLETE=-8;
	const static int DECODE_ERROR_VIDEO_DECODE_ERROR=-9;
	const static int DECODE_ERROR_AUDIO_DECODE_ERROR=-10;
	const static int SLES_ERROR_ENGINE=-11;

	const static int SWSCALE_CREATE_CONTEXT_ERROR=-13;

	const static int FRAME_TYPE_UNKNOWN=-12;
public:

	const static int NB_PKT_QUEUE=64;
	FAAudioDecoder *audioPlayer;
	FAVideoDecoder *videoPlayer;
protected:
	//容器格式的环境结构
	AVFormatContext *fmtCtx;
	//视频解码器环境结构
	AVCodecContext *videoDecodeCtx;
	//音频解码器环境结构
	AVCodecContext *audioDecodeCtx;
	//视频流结构体
	AVStream *videoStream;
	int videoStreamIndex;
	//音频流结构体
	AVStream *audioStream;
	int audioStreamIndex;

private:
	volatile enum FAplayer_State state;

	FAPktList* pktList;
	//
	Sigmore * deMuxerAndDecoderSigmore;
	Sigmore *audioDecoderSigmore;
	Sigmore *videoDecoderSigmore;

	Sigmore *mainThreadSigmore;

	FAPktList* audioPktList;
	FAPktList* videoPktList;

	Mutex stateLock;

};

#endif /* FFMPEGPLAYER_H_ */
