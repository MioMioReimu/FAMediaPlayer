package com.FATech.media;

import java.io.File;
import java.io.IOException;
import java.util.Timer;
import java.util.TimerTask;

import com.FATech.famediaplayer.*;

import android.app.Activity;
import android.content.Intent;
import android.graphics.drawable.Drawable;
import android.os.Bundle;
import android.util.Log;
import android.view.MotionEvent;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.View.OnTouchListener;
import android.widget.ImageButton;
import android.widget.RelativeLayout;
import android.widget.SeekBar;
import android.widget.SeekBar.OnSeekBarChangeListener;
import android.widget.TextView;
import android.widget.Toast;

public class MediaPlayerActivity extends Activity implements
		android.view.SurfaceHolder.Callback2 {
	String mMediaPath;
	AbstractMediaPlayer sysPlayer;
	ImageButton playButton;
	ImageButton prevButton;
	ImageButton nextButton;
	ImageButton stopButton;
	TextView progressView;
	TextView durationView;
	TextView title;
	SeekBar playProgressBar;
	SurfaceView videoSurface;
	RelativeLayout videoCtrlView;
	Timer progressNotifyTimer;

	static int HideDelay = 3000;
	Timer videoCtrlHideTimer;
	boolean bVideoCtrlViewHided = false;

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		Intent intent = getIntent();
		String action = intent.getAction();

		if (action.equals(Intent.ACTION_VIEW)
				|| action.equals(com.FATech.Action.View)) {
			String path = intent.getData().getPath();
			if (path != null) {
				mMediaPath = path;

			} else {
				this.finish();
			}
		} else {
			this.finish();
		}

		setContentView(R.layout.activity_video_player);

		playButton = (ImageButton) findViewById(R.id.video_controller_play);
		prevButton = (ImageButton) findViewById(R.id.video_controller_prev);
		nextButton = (ImageButton) findViewById(R.id.video_controller_next);
		stopButton = (ImageButton) findViewById(R.id.video_controller_stop);
		playProgressBar = (SeekBar) findViewById(R.id.playProgressBar);
		progressView = (TextView) findViewById(R.id.progress);
		durationView = (TextView) findViewById(R.id.duration);
		title = (TextView) findViewById(R.id.video_controller_title);
		videoCtrlView = (RelativeLayout) findViewById(R.id.video_controller_view);
		videoSurface = (SurfaceView) findViewById(R.id.video_player_surface);
		videoSurface.getHolder().addCallback(this);
		playButton.setOnClickListener(new OnClickListener() {
			@Override
			public void onClick(View v) {
				if (!bVideoCtrlViewHided) {
					setHideTimer();
					if (sysPlayer != null) {
						if (sysPlayer.isPlaying()) {
							sysPlayer.pause();
							playButton
									.setImageDrawable(getResources()
											.getDrawable(
													R.drawable.app_new_audio_ctrl_play));
						} else {
							sysPlayer.start();
							playButton
									.setImageDrawable(getResources()
											.getDrawable(
													R.drawable.app_new_audio_ctrl_pause));
						}
					} else {
						if (mMediaPath != null) {
							initPlayer();
						}
					}
				}
			}
		});
		prevButton.setOnClickListener(new OnClickListener() {
			@Override
			public void onClick(View v) {
				if (!bVideoCtrlViewHided) {
					setHideTimer();
				}
				Toast.makeText(MediaPlayerActivity.this, "prev!!",
						Toast.LENGTH_SHORT).show();
			}
		});
		nextButton.setOnClickListener(new OnClickListener() {
			@Override
			public void onClick(View v) {
				if (!bVideoCtrlViewHided) {
					setHideTimer();
				}
				Toast.makeText(MediaPlayerActivity.this, "next!!",
						Toast.LENGTH_SHORT).show();
			}
		});
		stopButton.setOnClickListener(new OnClickListener() {
			@Override
			public void onClick(View v) {
				if (!bVideoCtrlViewHided) {
					setHideTimer();
				}
				stopPlayer();
			}
		});

		videoSurface.setOnClickListener(new OnClickListener() {

			@Override
			public void onClick(View v) {
				if (bVideoCtrlViewHided) {
					bVideoCtrlViewHided = false;
					videoCtrlView.setVisibility(View.VISIBLE);
					setHideTimer();
				} else {
					bVideoCtrlViewHided = true;
					videoCtrlView.setVisibility(View.GONE);
					if (videoCtrlHideTimer != null) {
						videoCtrlHideTimer.cancel();
						videoCtrlHideTimer.purge();
						videoCtrlHideTimer = null;
					}
				}
			}

		});
		videoSurface.setOnTouchListener(new OnTouchListener() {

			@Override
			public boolean onTouch(View v, MotionEvent event) {
				// TODO Auto-generated method stub
				return false;
			}

		});
	}

	protected void onPause() {
		super.onPause();
	}

	protected void onResume() {
		super.onResume();
	}

	@Override
	public void surfaceCreated(SurfaceHolder holder) {
		initPlayer();

	}

	@Override
	public void surfaceChanged(SurfaceHolder holder, int format, int width,
			int height) {
	}

	@Override
	public void surfaceDestroyed(SurfaceHolder holder) {
		stopPlayer();
	}

	@Override
	public void surfaceRedrawNeeded(SurfaceHolder holder) {
	}

	private String MillisecondToTimeString(int ms) {
		int s = ms / 1000;
		int min = s / 60;
		int rs = s - min * 60;
		if (min > 60) {
			int h = min / 60;
			min = min - h * 60;
			return String.format("%02d:%02d:%02d", h, min, rs);
		} else {
			return String.format("%02d:%02d", min, rs);
		}
	}

	public void stopPlayer() {
		if (sysPlayer != null) {
			playProgressBar.setProgress(0);
			if (progressNotifyTimer != null) {
				progressNotifyTimer.purge();
				progressNotifyTimer.cancel();
				progressNotifyTimer = null;
			}
			videoSurface.setBackgroundColor(getResources().getColor(
					R.color.bkgd_total_black));
			playButton.setImageDrawable(getResources().getDrawable(
					R.drawable.app_new_audio_ctrl_play));
			this.progressView.setText("00:00");
			if (sysPlayer != null && sysPlayer.isPlaying()) {
				sysPlayer.stop();
			}

			sysPlayer.release();
			sysPlayer = null;
		}
	}

	public void initPlayer() {
		try {
			if (mMediaPath != null) {
				sysPlayer = new FAMediaPlayer();
				sysPlayer.setDataSource(mMediaPath);
				sysPlayer.setDisplay(this.videoSurface.getHolder());
				sysPlayer.prepare();
				sysPlayer.start();
				videoSurface
						.setBackgroundResource(R.color.bkgd_transtotal_black);
				title.setText((new File(mMediaPath)).getName());
				//Log.e("FAMediaPlayer","duration"+sysPlayer.getDuration());
				playProgressBar.setMax(sysPlayer.getDuration());
				durationView.setText(MillisecondToTimeString(sysPlayer
						.getDuration()));
				playButton.setImageDrawable(this.getResources().getDrawable(
						R.drawable.app_new_audio_ctrl_pause));
				playProgressBar
						.setOnSeekBarChangeListener(new OnSeekBarChangeListener() {

							@Override
							public void onProgressChanged(SeekBar seekBar,
									int progress, boolean fromUser) {
								if (sysPlayer != null && fromUser) {
									Log.e("FAMediaPlayer","seek");
									sysPlayer.seekTo(progress);
								}
							}

							@Override
							public void onStartTrackingTouch(SeekBar seekBar) {
								// TODO Auto-generated method stub

							}

							@Override
							public void onStopTrackingTouch(SeekBar seekBar) {
								// TODO Auto-generated method stub

							}

						});
				if (progressNotifyTimer != null) {
					progressNotifyTimer.cancel();
					progressNotifyTimer.purge();
					progressNotifyTimer = null;
				}
				progressNotifyTimer = new Timer();
				progressNotifyTimer.schedule(new TimerTask() {

					@Override
					public void run() {

						playProgressBar.post(new Runnable() {

							@Override
							public void run() {
								if (sysPlayer != null && sysPlayer.isPlaying()) {
									int position = sysPlayer
											.getCurrentPosition();
									//Log.e("position:", String.valueOf(position));
									playProgressBar.setProgress(position);
								}
							}

						});
						progressView.post(new Runnable() {

							@Override
							public void run() {
								if (sysPlayer != null && sysPlayer.isPlaying()) {
									int position = sysPlayer
											.getCurrentPosition();
									String currentPosition = MillisecondToTimeString(position);
									progressView.setText(currentPosition);
								}
							}
						});

					}

				}, 0, 100);
				setHideTimer();
			}
		} catch (IllegalArgumentException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (SecurityException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (IllegalStateException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}

	public void setHideTimer() {
		if (videoCtrlHideTimer != null) {
			videoCtrlHideTimer.cancel();
			videoCtrlHideTimer.purge();

		}
		videoCtrlHideTimer = new Timer();
		videoCtrlHideTimer.schedule(new TimerTask() {
			@Override
			public void run() {
				videoCtrlView.post(new Runnable() {
					@Override
					public void run() {
						bVideoCtrlViewHided = true;
						videoCtrlView.setVisibility(View.GONE);
					}
				});
			}
		}, HideDelay);
	}
}