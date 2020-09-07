package com.FATech.media;

import java.io.IOException;

import android.media.MediaPlayer;
import android.media.TimedText;
import android.media.MediaPlayer.TrackInfo;
import android.view.Surface;
import android.view.SurfaceHolder;

public abstract class AbstractMediaPlayer {
	/**
	 * @return the current playback position 当前播放的视频的播放位置
	 */
	public abstract int getCurrentPosition();
	
	/**
	 * @return the duration of the file 得到文件的持续时间 单位毫秒
	 */
	public abstract int getDuration();
	
	/**
	 * 
	 * @return 视频帧高度
	 */
	public abstract int getVideoHeight();
	
	/**
	 * 
	 * @return 视频帧宽度
	 */
	public abstract int getVideoWidth();
	
	/**
	 * 
	 * @return 是否循环播放该视频
	 */
	public abstract boolean isLooping();
	
	/**
	 * 
	 * @return 是否正在播放
	 */
	public abstract boolean isPlaying();
	
	/**
	 * 暂停
	 */
	public abstract void pause() throws IllegalStateException;
	
	/**
	 * 准备
	 * @throws IOException 
	 * @throws IllegalStateException 
	 */
	public abstract void prepare() throws IllegalStateException, IOException;
	
	/**
	 * 异步准备
	 */
	public abstract void prepareAsync()throws IllegalStateException;
	
	/**
     * Releases resources associated with this MediaPlayer object.
     * It is considered good practice to call this method when you're
     * done using the MediaPlayer. In particular, whenever an Activity
     * of an application is paused (its onPause() method is called),
     * or stopped (its onStop() method is called), this method should be
     * invoked to release the MediaPlayer object, unless the application
     * has a special need to keep the object around. In addition to
     * unnecessary resources (such as memory and instances of codecs)
     * being held, failure to call this method immediately if a
     * MediaPlayer object is no longer needed may also lead to
     * continuous battery consumption for mobile devices, and playback
     * failure for other applications if no multiple instances of the
     * same codec are supported on a device. Even if multiple instances
     * of the same codec are supported, some performance degradation
     * may be expected when unnecessary multiple instances are used
     * at the same time.
     */
	public abstract void release();
	
	/**
	 * 重置
	 * Resets the MediaPlayer to its uninitialized state. After calling this method,
	 * you will have to initialized it again by setting the data source and calling prepare();
	 */
	public abstract void reset();
	
	/**
	 * Seek to specified time position
	 * @param msec the offset in milliseconds from the start to seek to
	 */
	public abstract void seekTo (int msec) throws IllegalStateException;
	
	//public abstract void setAudioStreamType(int streamtype);
	
	/**
	 * Sets the data source(file-path or http/rtsp URL) to use.
	 * @param path the path of file, or the http/rtsp URL of the stream you want to play.
	 */
	public abstract void setDataSource(String path)
			throws IllegalArgumentException, SecurityException, 
			IllegalStateException, IOException;
	
	/**
     * Sets the {@link SurfaceHolder} to use for displaying the video
     * portion of the media.
     *
     * Either a surface holder or surface must be set if a display or video sink
     * is needed.  Not calling this method or {@link #setSurface(Surface)}
     * when playing back a video will result in only the audio track being played.
     * A null surface holder or surface will result in only the audio track being
     * played.
     *
     * @param sh the SurfaceHolder to use for video display
     */
	public abstract void setDisplay(SurfaceHolder sh);
	
	/**
     * Sets the {@link Surface} to be used as the sink for the video portion of
     * the media. This is similar to {@link #setDisplay(SurfaceHolder)}, but
     * does not support {@link #setScreenOnWhilePlaying(boolean)}.  Setting a
     * Surface will un-set any Surface or SurfaceHolder that was previously set.
     * A null surface will result in only the audio track being played.
     *
     * If the Surface sends frames to a {@link SurfaceTexture}, the timestamps
     * returned from {@link SurfaceTexture#getTimestamp()} will have an
     * unspecified zero point.  These timestamps cannot be directly compared
     * between different media sources, different instances of the same media
     * source, or multiple runs of the same program.  The timestamp is normally
     * monotonically increasing and is unaffected by time-of-day adjustments,
     * but it is reset when the position is set.
     *
     * @param surface The {@link Surface} to be used for the video portion of
     * the media.
     */
	public abstract void setSurface(Surface surface);
	
	/**
	 * Sets the player to be looping or non-lopping.
	 * @param looping whether to loop or not.
	 */
	public abstract void setLooping(boolean looping);
	
	public abstract void setOnBufferingUpdateListener(AbstractMediaPlayer.OnBufferingUpdateListener listener);
	
	public abstract void setOnCompletionListener(AbstractMediaPlayer.OnCompletionListener listener);
	
	public abstract void setOnErrorListener(AbstractMediaPlayer.OnErrorListener listener);
	
	public abstract void setOnInfoListener(AbstractMediaPlayer.OnInfoListener listener);
	
	public abstract void setOnPreparedListener(AbstractMediaPlayer.OnPreparedListener listener);
	
	public abstract void setOnSeekCompleteListener(AbstractMediaPlayer.OnSeekCompleteListener listener);
	
	public abstract void setOnTimedTextListener(AbstractMediaPlayer.OnTimedTextListener listener);
	
	public abstract void setOnVideoSizeChangedListener(AbstractMediaPlayer.OnVideoSizeChangedListener listener);
	
	public abstract void setScreenOnWhilePlaying(boolean screenOn);
	
	public abstract void setVideoScalingMode(int mode);
	
	public abstract void setVolume(float leftVolume,float rightVolume);
	
	public abstract void start() throws IllegalStateException;
	
	public abstract void stop() throws IllegalStateException;
	
	public interface OnBufferingUpdateListener {
		/**
         * Called to update status in buffering a media stream received through
         * progressive HTTP download. The received buffering percentage
         * indicates how much of the content has been buffered or played.
         * For example a buffering update of 80 percent when half the content
         * has already been played indicates that the next 30 percent of the
         * content to play has been buffered.
         *
         * @param mp      the MediaPlayer the update pertains to
         * @param percent the percentage (0-100) of the content
         *                that has been buffered or played thus far
         */
        void onBufferingUpdate(AbstractMediaPlayer mp, int percent);
	};
	
	public interface OnCompletionListener {
		/**
		 * Called when the end of a media source is reached during playback.
		 * 
		 * @param mp
		 *            the MediaPlayer that reached the end of the file
		 */
		void onCompletion(AbstractMediaPlayer mp);
	}
	
	public interface OnErrorListener
    {
        /**
         * Called to indicate an error.
         *
         * @param mp      the MediaPlayer the error pertains to
         * @param what    the type of error that has occurred:
         * <ul>
         * <li>{@link #MEDIA_ERROR_UNKNOWN}
         * <li>{@link #MEDIA_ERROR_SERVER_DIED}
         * </ul>
         * @param extra an extra code, specific to the error. Typically
         * implementation dependent.
         * <ul>
         * <li>{@link #MEDIA_ERROR_IO}
         * <li>{@link #MEDIA_ERROR_MALFORMED}
         * <li>{@link #MEDIA_ERROR_UNSUPPORTED}
         * <li>{@link #MEDIA_ERROR_TIMED_OUT}
         * </ul>
         * @return True if the method handled the error, false if it didn't.
         * Returning false, or not having an OnErrorListener at all, will
         * cause the OnCompletionListener to be called.
         */
        boolean onError(AbstractMediaPlayer mp, int what, int extra);
    }
	
	public interface OnInfoListener
    {
        /**
         * Called to indicate an info or a warning.
         *
         * @param mp      the MediaPlayer the info pertains to.
         * @param what    the type of info or warning.
         * <ul>
         * <li>{@link #MEDIA_INFO_UNKNOWN}
         * <li>{@link #MEDIA_INFO_VIDEO_TRACK_LAGGING}
         * <li>{@link #MEDIA_INFO_VIDEO_RENDERING_START}
         * <li>{@link #MEDIA_INFO_BUFFERING_START}
         * <li>{@link #MEDIA_INFO_BUFFERING_END}
         * <li>{@link #MEDIA_INFO_BAD_INTERLEAVING}
         * <li>{@link #MEDIA_INFO_NOT_SEEKABLE}
         * <li>{@link #MEDIA_INFO_METADATA_UPDATE}
         * </ul>
         * @param extra an extra code, specific to the info. Typically
         * implementation dependent.
         * @return True if the method handled the info, false if it didn't.
         * Returning false, or not having an OnErrorListener at all, will
         * cause the info to be discarded.
         */
        boolean onInfo(AbstractMediaPlayer mp, int what, int extra);
    }
	
	public interface OnPreparedListener
    {
        /**
         * Called when the media file is ready for playback.
         *
         * @param mp the MediaPlayer that is ready for playback
         */
        void onPrepared(AbstractMediaPlayer mp);
    }
	
	public interface OnSeekCompleteListener
    {
        /**
         * Called to indicate the completion of a seek operation.
         *
         * @param mp the MediaPlayer that issued the seek operation
         */
        public void onSeekComplete(AbstractMediaPlayer mp);
    }

	public interface OnTimedTextListener
    {
        /**
         * Called to indicate an avaliable timed text
         *
         * @param mp             the MediaPlayer associated with this callback
         * @param text           the timed text sample which contains the text
         *                       needed to be displayed and the display format.
         */
        public void onTimedText(AbstractMediaPlayer mp, TimedText text);
    }
	
	public interface OnVideoSizeChangedListener
    {
        /**
         * Called to indicate the video size
         *
         * The video size (width and height) could be 0 if there was no video,
         * no display surface was set, or the value was not determined yet.
         *
         * @param mp        the MediaPlayer associated with this callback
         * @param width     the width of the video
         * @param height    the height of the video
         */
        public void onVideoSizeChanged(AbstractMediaPlayer mp, int width, int height);
    }
	
	public static int MEDIA_STATE_IDLE=1;
	//public static int MEDIA_STATE_END=2;
	public static int MEDIA_STATE_ERROR=3;
	//public static int MEDIA_STATE_INITIALIZED=4;
	//public static int MEDIA_STATE_PREPARING=5;
	public static int MEDIA_STATE_PREPARED=6;
	public static int MEDIA_STATE_STARTED=7;
	public static int MEDIA_STATE_PAUSED=8;
	public static int MEDIA_STATE_STOPPED=9;
	public static int MEDIA_STATE_COMPLETED=10;
}
