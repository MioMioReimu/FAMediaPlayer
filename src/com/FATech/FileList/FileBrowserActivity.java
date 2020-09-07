package com.FATech.FileList;

import com.FATech.famediaplayer.*;

import android.app.Activity;
import android.content.Intent;
import android.net.Uri;
import android.os.Bundle;
import android.os.Environment;
import android.view.KeyEvent;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ListView;
import android.widget.TextView;

import com.FATech.FileList.FileListAdapter.FileInfo;
import com.FATech.FileList.FileListView.OnChangePathListener;
import com.FATech.FileList.FileListView.OnClickFileItemListener;

public class FileBrowserActivity extends Activity {
	FileListView fileListView;
	Button openParentButton;
	TextView filePathTextView;
	static {
		System.loadLibrary("ffmpeg");
		System.loadLibrary("famedia");
	}
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_file_browser);
		fileListView = (FileListView) this.findViewById(R.id.fileListView);

		openParentButton = (Button) this.findViewById(R.id.openParentButton);
		filePathTextView = (TextView) this.findViewById(R.id.filePathTextView);
		fileListView.setOnClickFileItemListener(new OnClickFileItemListener() {

			@Override
			public void onClickFileItem(FileInfo f) {
				Intent intent;
				if (f.extensioninfo.mimeType!=null&&f.extensioninfo.mimeType.startsWith("video/")) {
					intent = new Intent(com.FATech.Action.View);
				} else {
					intent = new Intent(Intent.ACTION_VIEW);
				}
				Uri uri = Uri.fromFile(f.file);
				intent.setDataAndType(uri, f.extensioninfo.mimeType);
				startActivity(intent);
			}

		});
		openParentButton.setOnClickListener(new OnClickListener() {

			@Override
			public void onClick(View v) {
				fileListView.OpenParent();
			}

		});
		fileListView.setOnChangePathListener(new OnChangePathListener() {

			@Override
			public void OnChangePath(String path) {
				filePathTextView.setText(path);
			}

		});
	}

	@Override
	public boolean onKeyDown(int keyCode, KeyEvent event) {
		if (keyCode == KeyEvent.KEYCODE_BACK && event.getRepeatCount() == 0) {
			if (!fileListView.OpenParent()) {
				this.finish();
				return false;
			}
			return true;
		}
		return false;
	}
}
