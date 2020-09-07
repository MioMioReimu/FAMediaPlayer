package com.FATech.media;

import java.io.IOException;

import android.media.MediaPlayer;
import android.media.MediaPlayer.TrackInfo;
import android.media.TimedText;
import android.view.Surface;
import android.view.SurfaceHolder;

public class AndroidMediaPlayer extends AbstractMediaPlayer
	implements MediaPlayer.OnBufferingUpdateListener,MediaPlayer.OnSeekCompleteListener,MediaPlayer.OnPreparedListener,
			MediaPlayer.OnTimedTextListener,MediaPlayer.OnVideoSizeChangedListener,MediaPlayer.OnCompletionListener,
			MediaPlayer.OnErrorListener,MediaPlayer.OnInfoListener
{

	AndroidMediaPlayer() {
		m_sysMediaPlayer=new MediaPlayer();
		m_sysMediaPlayer.setOnBufferingUpdateListener(this);
		m_sysMediaPlayer.setOnCompletionListener(this);
		m_sysMediaPlayer.setOnErrorListener(this);
		m_sysMediaPlayer.setOnInfoListener(this);
		m_sysMediaPlayer.setOnPreparedListener(this);
		m_sysMediaPlayer.setOnSeekCompleteListener(this);
		m_sysMediaPlayer.setOnTimedTextListener(this);
		m_sysMediaPlayer.setOnVideoSizeChangedListener(this);
	}
	
	MediaPlayer m_sysMediaPlayer=null;
	@Override
	public int getCurrentPosition() {
		return m_sysMediaPlayer.getCurrentPosition();
	}
	@Override
	public int getDuration() {
		return m_sysMediaPlayer.getDuration();
	}

	@Override
	public int getVideoHeight() {
		return m_sysMediaPlayer.getVideoHeight();
	}

	@Override
	public int getVideoWidth() {
		return m_sysMediaPlayer.getVideoWidth();
	}

	@Override
	public boolean isLooping() {
		return m_sysMediaPlayer.isLooping();
	}

	@Override
	public boolean isPlaying() throws IllegalStateException{
		return m_sysMediaPlayer.isPlaying();
	}

	@Override
	public void pause() throws IllegalStateException {
		m_sysMediaPlayer.pause();
	}

	@Override
	public void prepare() throws IllegalStateException, IOException {
		m_sysMediaPlayer.prepare();
	}

	@Override
	public void prepareAsync() throws IllegalStateException{
		m_sysMediaPlayer.prepareAsync();
	}

	@Override
	public void release() {
		m_sysMediaPlayer.release();
	}

	@Override
	public void reset() {
		m_sysMediaPlayer.reset();
	}

	@Override
	public void seekTo(int msec) throws IllegalStateException{
			m_sysMediaPlayer.seekTo(msec);
	}

	@Override
	public void setDataSource(String path) 
			throws IllegalArgumentException, SecurityException, 
			IllegalStateException, IOException {
		m_sysMediaPlayer.setDataSource(path);
	}

	@Override
	public void setDisplay(SurfaceHolder sh) {
		m_sysMediaPlayer.setDisplay(sh);
	}

	@Override
	public void setSurface(Surface surface) {
		m_sysMediaPlayer.setSurface(surface);
	}

	@Override
	public void setLooping(boolean looping) {
		m_sysMediaPlayer.setLooping(looping);
	}

	
	@Override
	public void setOnBufferingUpdateListener(OnBufferingUpdateListener listener) {
		onBufferingUpdateListener=listener;
	}

	
	@Override
	public void setOnCompletionListener(OnCompletionListener listener) {
		onCompletionListener=listener;
	}

	@Override
	public void setOnErrorListener(OnErrorListener listener) {
		this.onErrorListener=listener;
	}

	@Override
	public void setOnInfoListener(OnInfoListener listener) {
		this.onInfoListener=listener;
	}

	@Override
	public void setOnPreparedListener(OnPreparedListener listener) {
		this.onPreparedListener=listener;
	}

	@Override
	public void setOnSeekCompleteListener(OnSeekCompleteListener listener) {
		this.onSeekCompleteListener=listener;
	}

	@Override
	public void setOnTimedTextListener(OnTimedTextListener listener) {
		this.onTimedTextListener=listener;
	}

	@Override
	public void setOnVideoSizeChangedListener(
			OnVideoSizeChangedListener listener) {
		this.onVideoSizeChangedListener=listener;
	}

	@Override
	public void setScreenOnWhilePlaying(boolean screenOn) {
		this.m_sysMediaPlayer.setScreenOnWhilePlaying(screenOn);
	}

	@Override
	public void setVideoScalingMode(int mode) {
		m_sysMediaPlayer.setVideoScalingMode(mode);
	}

	@Override
	public void setVolume(float leftVolume, float rightVolume) {
		m_sysMediaPlayer.setVolume(leftVolume, rightVolume);
	}

	@Override
	public void start() throws IllegalStateException{
		m_sysMediaPlayer.start();
	}

	@Override
	public void stop() throws IllegalStateException{
		m_sysMediaPlayer.stop();
		
	}



	@Override
	public boolean onInfo(MediaPlayer mp, int what, int extra) {
		if(this.onInfoListener!=null){
			onInfoListener.onInfo(this, what, extra);
		}
		return true;
	}

	@Override
	public boolean onError(MediaPlayer mp, int what, int extra) {
		//this.currentState=AbstractMediaPlayer.MEDIA_STATE_ERROR;
		if(onErrorListener!=null) {
			onErrorListener.onError(this, what, extra);
		}
		return true;
	}

	@Override
	public void onCompletion(MediaPlayer mp) {
		//this.currentState=AbstractMediaPlayer.MEDIA_STATE_COMPLETED;
		if(onCompletionListener!=null) {
			onCompletionListener.onCompletion(this);
		}
	}

	@Override
	public void onVideoSizeChanged(MediaPlayer mp, int width, int height) {
		if(onVideoSizeChangedListener!=null) {
			onVideoSizeChangedListener.onVideoSizeChanged(this, width, height);
		}
	}

	@Override
	public void onTimedText(MediaPlayer mp, TimedText text) {
		if(onTimedTextListener!=null) {
			onTimedTextListener.onTimedText(this, text);
		}
	}

	@Override
	public void onPrepared(MediaPlayer mp) {
		//this.currentState=AbstractMediaPlayer.MEDIA_STATE_PREPARED;
		if(onPreparedListener!=null){
			onPreparedListener.onPrepared(this);
		}
	}

	@Override
	public void onSeekComplete(MediaPlayer mp) {
		if(onSeekCompleteListener!=null) {
			onSeekCompleteListener.onSeekComplete(this);
		}
	}

	@Override
	public void onBufferingUpdate(MediaPlayer mp, int percent) {
		if(onBufferingUpdateListener!=null){
			onBufferingUpdateListener.onBufferingUpdate(this, percent);
		}
	}
	
	protected OnBufferingUpdateListener onBufferingUpdateListener;
	protected OnCompletionListener onCompletionListener;
	protected OnErrorListener onErrorListener;
	protected OnInfoListener onInfoListener;
	protected OnPreparedListener onPreparedListener;
	protected OnSeekCompleteListener onSeekCompleteListener;
	protected OnTimedTextListener onTimedTextListener;
	protected OnVideoSizeChangedListener onVideoSizeChangedListener;
	
}
