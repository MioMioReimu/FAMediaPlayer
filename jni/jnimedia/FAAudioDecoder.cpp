/*
 * FAAudioDecoder.cpp
 *
 *  Created on: 2013年9月15日
 *      Author: tlm
 */

#include <android/log.h>
#include "FAAudioDecoder.h"
extern "C" void bufferQueueCallback(SLAndroidSimpleBufferQueueItf bufferQueue,
		void *pContext);
FAAudioDecoder::FAAudioDecoder() {
	// TODO Auto-generated constructor stub
	this->audioBuffer = NULL;
	this->audioBufferLinesize = 0;
	this->audioBufferQueue = NULL;
	this->audioBufferQueuePlayer = NULL;
	this->audioBufferQueuePlayerEffectSend = NULL;
	this->audioBufferQueuePlayerObject = NULL;
	this->audioBufferQueuePlayerVolume = NULL;
	this->audioEngineObject = NULL;
	this->audioEngine = NULL;
	currentPkt = NULL;
	outputMixObject = NULL;
	outputMixEnvironmentalReverb = NULL;
	currentPosition = 0;
	this->audioBufferQueueCallbackPointer = bufferQueueCallback;
}

FAAudioDecoder::~FAAudioDecoder() {
	// TODO Auto-generated destructor stub

}
void FAAudioDecoder::run() {
	if (*state == PLAYING) {
		for (int i = 0; i < 4; i++) {
			FFMPEG_PLAYER_DEBUG_PRINT("Init AudioBuffer\n");
			bufferQueuePlayerCallback(audioBufferQueue, NULL);

		}

		(*audioBufferQueuePlayer)->SetPlayState(audioBufferQueuePlayer,
		SL_PLAYSTATE_PLAYING);
		FFMPEG_PLAYER_DEBUG_PRINT("Success to start audio player!\n");
	} else if ((*state==SEEKING)||(*state==STOPPED)) {
		this->audioDecoderSigmore->wakeAll();
		(*audioBufferQueuePlayer)->SetPlayState(audioBufferQueuePlayer,
		SL_PLAYSTATE_STOPPED);
	}else if((*state == PAUSED)){
		this->audioDecoderSigmore->wakeAll();
		(*audioBufferQueuePlayer)->SetPlayState(audioBufferQueuePlayer,
				SL_PLAYSTATE_PAUSED);
	}
}
int FAAudioDecoder::play() {
	this->start();
	return FFMpegPlayer::SUCCESS;
}
int FAAudioDecoder::pause() {
	this->start();
	return FFMpegPlayer::SUCCESS;
}
int FAAudioDecoder::stop() {
	this->start();
	return FFMpegPlayer::SUCCESS;
}
void FAAudioDecoder::bufferQueuePlayerCallback(SLAndroidSimpleBufferQueueItf bq,
		void * context) {
	SLresult result;
	int frametype;
	int got_frame = 0;
	int ret;
	int decoded;
	unsigned int playerstate;
	mLock.lock();
	if (*state == PLAYING) {

		while ((got_frame == 0) && (*state == PLAYING)) {

			(*this->audioBufferQueuePlayer)->GetPlayState(
					audioBufferQueuePlayer, &playerstate);
			if (playerstate != SL_PLAYSTATE_PLAYING) {
				(*this->audioBufferQueuePlayer)->SetPlayState(
						audioBufferQueuePlayer, SL_PLAYSTATE_PLAYING);
			}
			if (currentPkt == NULL) {

				currentPkt = this->audioPktList->pop_front();
				//FFMPEG_PLAYER_DEBUG_PRINT("audio Decode :audio Pkt num:%d\n",
				//audioPktList->size());
				if (currentPkt == NULL) {
					//FFMPEG_PLAYER_DEBUG_PRINT("audio pkt list in null!\n");
					this->audioDecoderSigmore->wait();
					//FFMPEG_PLAYER_DEBUG_PRINT(
					//		"audio pkt list in not null now!\n");
				}
			}
			if (currentPkt != NULL) {
				//FFMPEG_PLAYER_DEBUG_PRINT("stream:%d\n",currentPkt->stream_index);

				ret = avcodec_decode_audio4(audioDecodeCtx, frame, &got_frame,
						currentPkt);
				if (ret < 0) {
					break;
					//FFMPEG_PLAYER_DEBUG_PRINT("audio decode error!\n");
				}
				//FFMPEG_PLAYER_DEBUG_PRINT("decode audio success:%d!\n",got_frame);
				decoded = FFMIN(ret, currentPkt->size);

				currentPkt->data += decoded;
				currentPkt->size -= decoded;
				if (currentPkt->size <= 0) {
					av_free_packet(currentPkt);
					//FFMPEG_PLAYER_DEBUG_PRINT("free audio pkt success!\n");
					pktList->push_back(currentPkt);
					//FFMPEG_PLAYER_DEBUG_PRINT("push free pkt success!\n");
					deMuxerAndDecoderSigmore->wake();
					//FFMPEG_PLAYER_DEBUG_PRINT("wake demuxer thread success!\n");
					currentPkt = NULL;
				}
			} else {
				//FFMPEG_PLAYER_DEBUG_PRINT("audio current Pkt is null");
			}

		}
		if (got_frame) {
			//FFMPEG_PLAYER_DEBUG_PRINT("Frame Channels:%d", frame->channels);
			int nbSamples = av_rescale_rnd(
					swr_get_delay(audioResampleCtx, frame->sample_rate)
							+ frame->nb_samples, this->sampleRate,
					audioDecodeCtx->sample_rate, AV_ROUND_UP);
			if (nbSamples > this->nbSamplesOfBuffer) {
				av_free(this->audioBuffer[0]);
				nbSamplesOfBuffer = nbSamples;
				av_samples_alloc(audioBuffer, &audioBufferLinesize,
						nbOfChannels, nbSamples, DEFAULT_SAMPLE_FORMAT, 0);
			}
			ret = swr_convert(audioResampleCtx, audioBuffer, nbSamples,
					(const uint8_t**) frame->extended_data, frame->nb_samples);
			result = (*audioBufferQueue)->Enqueue(audioBufferQueue,
					audioBuffer[0],
					av_get_bytes_per_sample(DEFAULT_SAMPLE_FORMAT)
							* nbOfChannels * ret);
			//FFMPEG_PLAYER_DEBUG_PRINT("audio Decode :free Pkt num:%d\n",
			//pktList->size());
			this->currentPosition += frame->pkt_duration;
			//av_samples_get_buffer_size(&audioBufferLinesize,
			//nbOfChannels, ret, DEFAULT_SAMPLE_FORMAT, 0));
			//FFMPEG_PLAYER_DEBUG_PRINT("Enqueue Buffer Success\n");
		}

	}
	mLock.unlock();
}

int FAAudioDecoder::releaseAllPkts() {
	unsigned int playerstate;
	while (audioPktList->size() > 0) {
		AVPacket *p = audioPktList->pop_front();
		p->data += p->size;
		p->size -= p->size;
		av_free_packet(p);
		this->pktList->push_back(p);

	}
	this->deMuxerAndDecoderSigmore->wakeAll();
}

int FAAudioDecoder::setDecoder(AVFormatContext *fmtCtx, int streamIndex,
		FAPktList* audioPktList, FAPktList *pktList, volatile FAplayer_State *state,
		Sigmore *deMuxerAndDecoderSigmore, Sigmore *audioDecoderSigmore) {
	this->audioStreamIndex = streamIndex;
	this->audioStream = fmtCtx->streams[streamIndex];
	this->audioPktList = audioPktList;
	this->pktList = pktList;
	this->state = state;
	this->deMuxerAndDecoderSigmore = deMuxerAndDecoderSigmore;
	this->audioDecoderSigmore = audioDecoderSigmore;
	this->audioDecodeCtx = audioStream->codec;
	currentPosition = 0;
	FFMPEG_PLAYER_DEBUG_PRINT("audio decoder name:%s",
			audioDecodeCtx->codec->name);
	this->frame = avcodec_alloc_frame();
	if (!frame) {
		FFMPEG_PLAYER_DEBUG_PRINT("Could not allocate frame\n");
		return FFMpegPlayer::MEMORY_ERROR_CANNOT_ALLOCATE_FRAME;
	}
	FFMPEG_PLAYER_DEBUG_PRINT("allocate audio frame success!\n");

	audioResampleCtx = swr_alloc();
	if (!audioResampleCtx) {
		FFMPEG_PLAYER_DEBUG_PRINT("Could not allocate resample context\n");
		return FFMpegPlayer::RESAMPLE_ERROR_CANNOT_ALLOCATE_RESAMPLER;
	}

	av_opt_set_int(audioResampleCtx, "in_channel_layout",
			audioDecodeCtx->channel_layout, 0);
	av_opt_set_int(audioResampleCtx, "in_sample_rate",
			audioDecodeCtx->sample_rate, 0);
	av_opt_set_sample_fmt(audioResampleCtx, "in_sample_fmt",
			audioDecodeCtx->sample_fmt, 0);

	this->nbOfChannels = av_get_channel_layout_nb_channels(
			DEFAULT_CHANNEL_LAYOUT);
	this->bitsPerSample = av_get_bytes_per_sample(DEFAULT_SAMPLE_FORMAT) * 8;
	this->sampleRate =
			audioDecodeCtx->sample_rate > DEFAULT_SAMPLE_RATE ?
					DEFAULT_SAMPLE_RATE : audioDecodeCtx->sample_rate;
	this->channelLayout = DEFAULT_CHANNEL_LAYOUT;
	av_opt_set_int(audioResampleCtx, "out_channel_layout",
			DEFAULT_CHANNEL_LAYOUT, 0);
	av_opt_set_int(audioResampleCtx, "out_sample_rate", sampleRate, 0);
	av_opt_set_sample_fmt(audioResampleCtx, "out_sample_fmt",
			DEFAULT_SAMPLE_FORMAT, 0);
	FFMPEG_PLAYER_DEBUG_PRINT("in_channel_layout:%d",
			(int ) (audioDecodeCtx->channel_layout));
	FFMPEG_PLAYER_DEBUG_PRINT("in_sample_rate:%d",
			(int ) (audioDecodeCtx->sample_rate));
	FFMPEG_PLAYER_DEBUG_PRINT("in_sample_fmt:%s",
			av_get_sample_fmt_name(audioDecodeCtx->sample_fmt));
	FFMPEG_PLAYER_DEBUG_PRINT("out_channel_layout:%d",
			(int ) (DEFAULT_CHANNEL_LAYOUT));
	FFMPEG_PLAYER_DEBUG_PRINT("out_sample_rate:%d", (int ) sampleRate);
	FFMPEG_PLAYER_DEBUG_PRINT("out_sample_fmt:%s",
			av_get_sample_fmt_name(DEFAULT_SAMPLE_FORMAT));
	if (swr_init(audioResampleCtx) < 0) {
		FFMPEG_PLAYER_DEBUG_PRINT(
				"Failed to initialize the resampling context\n");
		return FFMpegPlayer::RESAMPLE_ERROR_CANNOT_INIT_RESAMPLER;
	}
	this->nbSamplesOfBuffer = DEFAULT_NB_SAMPLES;
	av_samples_alloc_array_and_samples(&audioBuffer, &audioBufferLinesize,
			nbOfChannels, nbSamplesOfBuffer, DEFAULT_SAMPLE_FORMAT, 0);
	return FFMpegPlayer::SUCCESS;
}
int FAAudioDecoder::prepare() {
	SLresult result;
//创建引擎结构体
	result = slCreateEngine(&audioEngineObject, 0, NULL, 0, NULL, NULL);
	if (SL_RESULT_SUCCESS != result) {
		this->releaseAudioPlayer();
		return FFMpegPlayer::SLES_ERROR_ENGINE;
	}
	FFMPEG_PLAYER_DEBUG_PRINT("Create Audio Engine Object Success\n");

	result = (*audioEngineObject)->Realize(audioEngineObject, SL_BOOLEAN_FALSE);
	if (SL_RESULT_SUCCESS != result) {
		this->releaseAudioPlayer();
		return FFMpegPlayer::SLES_ERROR_ENGINE;
	}
	FFMPEG_PLAYER_DEBUG_PRINT("Realize Audio Engine Success\n");

//获得引擎的可调用函数结构体 这个结构体就是一个函数指针的结构体，后续很多工作都是采用这个结构体里的函数进行的
	result = (*audioEngineObject)->GetInterface(audioEngineObject,
			SL_IID_ENGINE, &audioEngine);
	if (SL_RESULT_SUCCESS != result) {
		this->releaseAudioPlayer();
		return FFMpegPlayer::SLES_ERROR_ENGINE;
	};
	FFMPEG_PLAYER_DEBUG_PRINT("Create Audio Engine Success\n");

//创建输出设备
//混响是属于输出设备的，可要可不要
	const SLInterfaceID mixId[1] = { SL_IID_ENVIRONMENTALREVERB };
	const SLboolean mixRequest[1] = { SL_BOOLEAN_FALSE };
	result = (*audioEngine)->CreateOutputMix(audioEngine, &outputMixObject, 1,
			mixId, mixRequest);
	if (SL_RESULT_SUCCESS != result) {
		this->releaseAudioPlayer();
		return FFMpegPlayer::SLES_ERROR_ENGINE;
	};
	FFMPEG_PLAYER_DEBUG_PRINT("Create Audio Output Mix Object Success\n");

//实例化输出设备
	result = (*outputMixObject)->Realize(outputMixObject, SL_BOOLEAN_FALSE);
	if (SL_RESULT_SUCCESS != result) {
		this->releaseAudioPlayer();
		return FFMpegPlayer::SLES_ERROR_ENGINE;
	};
	FFMPEG_PLAYER_DEBUG_PRINT("Realize Audio Output Mix Obejct Success\n");

//获得混响的结构体
	result = (*outputMixObject)->GetInterface(outputMixObject,
			SL_IID_ENVIRONMENTALREVERB, &outputMixEnvironmentalReverb);
	FFMPEG_PLAYER_DEBUG_PRINT(
			"get Audio Output Mix Environment Reverb Success\n");
	SLDataLocator_AndroidSimpleBufferQueue locator_bufqueue;
	locator_bufqueue.locatorType = SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE;
	locator_bufqueue.numBuffers = NB_AUDIO_BUFFER_QUEUE;
	SLDataFormat_PCM format_pcm;
	format_pcm.formatType = SL_DATAFORMAT_PCM;
	format_pcm.numChannels = nbOfChannels;	//this->audioDecodeCtx->channels;
	format_pcm.samplesPerSec = sampleRate * 1000;
	format_pcm.bitsPerSample = bitsPerSample;// SL_PCMSAMPLEFORMAT_FIXED_16;	// av_get_bytes_per_sample(
//audioDecodeCtx->sample_fmt) * 8;
	format_pcm.containerSize = bitsPerSample;//SL_PCMSAMPLEFORMAT_FIXED_16;	// av_get_bytes_per_sample(
//audioDecodeCtx->sample_fmt) * 8;
	format_pcm.channelMask = channelLayout;
	format_pcm.endianness = SL_BYTEORDER_LITTLEENDIAN;

	SLDataSource audioSrc = { &locator_bufqueue, &format_pcm };

	SLDataLocator_OutputMix loc_outmix = { SL_DATALOCATOR_OUTPUTMIX,
			outputMixObject };
	SLDataSink audioSnk = { &loc_outmix, NULL };

	const SLInterfaceID playerIds[3] = { SL_IID_BUFFERQUEUE, SL_IID_EFFECTSEND,
			SL_IID_VOLUME };
	const SLboolean playerReqs[3] = { SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE,
	SL_BOOLEAN_TRUE };
	result = (*audioEngine)->CreateAudioPlayer(audioEngine,
			&audioBufferQueuePlayerObject, &audioSrc, &audioSnk, 3, playerIds,
			playerReqs);
	if (SL_RESULT_SUCCESS != result) {
		this->releaseAudioPlayer();
		return FFMpegPlayer::SLES_ERROR_ENGINE;
	};
	FFMPEG_PLAYER_DEBUG_PRINT("Create Audio Player Success\n");

	result = (*audioBufferQueuePlayerObject)->Realize(
			audioBufferQueuePlayerObject,
			SL_BOOLEAN_FALSE);
	if (SL_RESULT_SUCCESS != result) {
		this->releaseAudioPlayer();
		return FFMpegPlayer::SLES_ERROR_ENGINE;
	};

	result = (*audioBufferQueuePlayerObject)->GetInterface(
			audioBufferQueuePlayerObject, SL_IID_PLAY, &audioBufferQueuePlayer);
	if (SL_RESULT_SUCCESS != result) {
		this->releaseAudioPlayer();
		return FFMpegPlayer::SLES_ERROR_ENGINE;
	};

	result = (*audioBufferQueuePlayerObject)->GetInterface(
			audioBufferQueuePlayerObject, SL_IID_BUFFERQUEUE,
			&audioBufferQueue);
	if (SL_RESULT_SUCCESS != result) {
		this->releaseAudioPlayer();
		return FFMpegPlayer::SLES_ERROR_ENGINE;
	};

	result = (*audioBufferQueue)->RegisterCallback(audioBufferQueue,
			audioBufferQueueCallbackPointer, NULL);
	if (SL_RESULT_SUCCESS != result) {
		this->releaseAudioPlayer();
		return FFMpegPlayer::SLES_ERROR_ENGINE;
	};

	result = (*audioBufferQueuePlayerObject)->GetInterface(
			audioBufferQueuePlayerObject, SL_IID_EFFECTSEND,
			&audioBufferQueuePlayerEffectSend);
	if (SL_RESULT_SUCCESS != result) {
		this->releaseAudioPlayer();
		return FFMpegPlayer::SLES_ERROR_ENGINE;
	};
	result = (*audioBufferQueuePlayerObject)->GetInterface(
			audioBufferQueuePlayerObject, SL_IID_VOLUME,
			&audioBufferQueuePlayerVolume);
	if (SL_RESULT_SUCCESS != result) {
		this->releaseAudioPlayer();
		return FFMpegPlayer::SLES_ERROR_ENGINE;
	};

	(*audioBufferQueuePlayer)->SetPlayState(audioBufferQueuePlayer,
	SL_PLAYSTATE_PLAYING);
	short int maxVolume;
	(*audioBufferQueuePlayerVolume)->GetMaxVolumeLevel(
			audioBufferQueuePlayerVolume, &maxVolume);
	(*audioBufferQueuePlayerVolume)->SetVolumeLevel(
			audioBufferQueuePlayerVolume, maxVolume / 2);
	FFMPEG_PLAYER_DEBUG_PRINT("Prepare Audio Player Success\n");
	return FFMpegPlayer::SUCCESS;
}

int FAAudioDecoder::getCurrentPosition() {
	if (audioStream) {
		//FFMPEG_PLAYER_DEBUG_PRINT("pos:%d", (int )currentPosition);
		int pos = this->currentPosition * 1000
				* this->audioStream->time_base.num
				/ this->audioStream->time_base.den;
		//FFMPEG_PLAYER_DEBUG_PRINT("pos: %d ms", pos);
		return pos;
	} else {
		return 0;
	}
}
int FAAudioDecoder::setCurrentPosition(int msec){
	if (audioStream) {
			//FFMPEG_PLAYER_DEBUG_PRINT("pos:%d", (int )currentPosition);
		currentPosition = (double)msec *this->audioStream->time_base.den/( 1000.0
					* this->audioStream->time_base.num)
					;
			//FFMPEG_PLAYER_DEBUG_PRINT("pos: %d ms", pos);
	}
}
void FAAudioDecoder::releaseAudioPlayer() {
	if (audioBufferQueuePlayerObject != NULL) {
		unsigned int state;
		(*audioBufferQueuePlayer)->GetPlayState(audioBufferQueuePlayer, &state);
		if (state == SL_PLAYSTATE_PLAYING || state == SL_PLAYSTATE_PAUSED) {
			(*audioBufferQueuePlayer)->SetPlayState(audioBufferQueuePlayer,
			SL_PLAYSTATE_STOPPED);
		}
		(*audioBufferQueuePlayerObject)->Destroy(audioBufferQueuePlayerObject);
		audioBufferQueuePlayerObject = NULL;
		audioBufferQueuePlayer = NULL;
		audioBufferQueuePlayerEffectSend = NULL;
		audioBufferQueuePlayerVolume = NULL;
		audioBufferQueue = NULL;
	}
	if (outputMixObject != NULL) {
		(*outputMixObject)->Destroy(outputMixObject);
		outputMixObject = NULL;
		outputMixEnvironmentalReverb = NULL;
	}
	if (audioEngineObject != NULL) {
		(*audioEngineObject)->Destroy(audioEngineObject);
		audioEngineObject = NULL;
		audioEngine = NULL;
	}
}

void FAAudioDecoder::releaseDecoder() {
	avcodec_close(audioDecodeCtx);
	if (audioBuffer)
		av_freep(&audioBuffer[0]);
	av_freep(&audioBuffer);
	avcodec_free_frame(&frame);
	swr_free(&audioResampleCtx);
	this->audioDecodeCtx=NULL;
	this->audioDecoderSigmore=NULL;
	this->deMuxerAndDecoderSigmore=NULL;
	this->pktList=NULL;
	this->currentPkt=NULL;
	this->audioPktList=NULL;
	this->audioStream=NULL;
	this->audioStreamIndex=-1;
}
