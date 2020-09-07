package com.FATech.media;

import java.io.IOException;

import android.media.MediaPlayer.TrackInfo;
import android.view.Surface;
import android.view.SurfaceHolder;

public class FAMediaPlayer extends AbstractMediaPlayer {

	static {
		System.loadLibrary("ffmpeg");
		System.loadLibrary("famedia");
	}

	public static int UN_INITED = 0;
	public static int INITED = 1;
	public static int PREPARED = 2;
	public static int PLAYING = 3;
	public static int PAUSED = 4;
	public static int SEEKING = 5;
	public static int STOPPED = -1;
	public static int RELEASED = -2;

	@Override
	public native int getCurrentPosition();

	@Override
	public native int getDuration();

	@Override
	public native int getVideoHeight();

	@Override
	public native int getVideoWidth();

	@Override
	public boolean isLooping() {
		// TODO Auto-generated method stub
		return false;
	}

	@Override
	public boolean isPlaying() {
		return getState()==PLAYING;
	}

	public native int getState();

	// 暂停播放
	@Override
	public native void pause() throws IllegalStateException;

	@Override
	public native void prepare() throws IllegalStateException, IOException;

	@Override
	public void prepareAsync() throws IllegalStateException {
		// TODO Auto-generated method stub

	}

	@Override
	public native void release();
	@Override
	public void reset() {
		// TODO Auto-generated method stub

	}

	@Override
	public native void seekTo(int msec) throws IllegalStateException;

	@Override
	public native void setDataSource(String path)
			throws IllegalArgumentException, SecurityException,
			IllegalStateException, IOException;

	@Override
	public void setDisplay(SurfaceHolder sh) {
		setSurface(sh.getSurface());
	}

	@Override
	public native void setSurface(Surface surface);

	@Override
	public void setLooping(boolean looping) {
		// TODO Auto-generated method stub

	}

	@Override
	public void setOnBufferingUpdateListener(OnBufferingUpdateListener listener) {
		// TODO Auto-generated method stub

	}

	@Override
	public void setOnCompletionListener(OnCompletionListener listener) {
		// TODO Auto-generated method stub

	}

	@Override
	public void setOnErrorListener(OnErrorListener listener) {
		// TODO Auto-generated method stub

	}

	@Override
	public void setOnInfoListener(OnInfoListener listener) {
		// TODO Auto-generated method stub

	}

	@Override
	public void setOnPreparedListener(OnPreparedListener listener) {
		// TODO Auto-generated method stub

	}

	@Override
	public void setOnSeekCompleteListener(OnSeekCompleteListener listener) {
		// TODO Auto-generated method stub

	}

	@Override
	public void setOnTimedTextListener(OnTimedTextListener listener) {
		// TODO Auto-generated method stub

	}

	@Override
	public void setOnVideoSizeChangedListener(
			OnVideoSizeChangedListener listener) {
		// TODO Auto-generated method stub

	}

	@Override
	public void setScreenOnWhilePlaying(boolean screenOn) {
		// TODO Auto-generated method stub

	}

	@Override
	public void setVideoScalingMode(int mode) {
		// TODO Auto-generated method stub

	}

	@Override
	public void setVolume(float leftVolume, float rightVolume) {
		// TODO Auto-generated method stub

	}

	@Override
	public native void start() throws IllegalStateException;

	@Override
	public native void stop() throws IllegalStateException;

}
