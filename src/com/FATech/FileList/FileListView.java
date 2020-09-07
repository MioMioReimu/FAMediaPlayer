package com.FATech.FileList;

import java.io.File;
import java.util.ArrayList;

import android.content.Context;
import android.content.Intent;
import android.os.Environment;
import android.util.AttributeSet;
import android.util.Log;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ListView;

import com.FATech.FileList.FileListAdapter.FileInfo;
import com.FATech.famediaplayer.R;

public class FileListView extends ListView {
	OnClickFileItemListener onClickFileItemListener = null;
	ArrayList<Integer> parentsPositionStack = new ArrayList<Integer>();
	OnItemClickListener onItemClickListener = new OnItemClickListener() {

		@Override
		public void onItemClick(AdapterView<?> parent, View view, int position,
				long id) {
			if (((FileListAdapter.FileInfo) flAdapter.getItem(position)).file
					.isDirectory()) {
				OpenChild(position);
			} else {
				if (onClickFileItemListener != null) {
					onClickFileItemListener
							.onClickFileItem((FileListAdapter.FileInfo) flAdapter
									.getItem(position));

				}
			}
		}

	};

	FileListAdapter flAdapter;
	public ExtensionInfo extensionList[] = new ExtensionInfo[10];

	public FileListView(Context context) {
		super(context);
		init();
	}

	public FileListView(Context context, AttributeSet attrs) {
		super(context, attrs);
		init();
	}

	public FileListView(Context context, AttributeSet attrs, int defStyle) {
		super(context, attrs, defStyle);
		init();
	}

	public interface OnClickFileItemListener {
		public void onClickFileItem(FileListAdapter.FileInfo f);
	}

	private void init() {
		extensionList[0] = new ExtensionInfo("mkv", R.drawable.format_media,
				"video/*");
		extensionList[1] = new ExtensionInfo("mp4", R.drawable.format_media,
				"video/*");
		extensionList[2] = new ExtensionInfo("wmv", R.drawable.format_media,
				"video/*");
		extensionList[3] = new ExtensionInfo("avi", R.drawable.format_media,
				"video/*");
		extensionList[4] = new ExtensionInfo("flv", R.drawable.format_media,
				"video/*");
		extensionList[5] = new ExtensionInfo("mpeg", R.drawable.format_media,
				"video/*");
		extensionList[6] = new ExtensionInfo("mov", R.drawable.format_media,
				"video/*");
		extensionList[7] = new ExtensionInfo("mpeg4", R.drawable.format_media,
				"video/*");
		extensionList[8] = new ExtensionInfo("3gp", R.drawable.format_media,
				"video/*");
		extensionList[9] = new ExtensionInfo("txt", R.drawable.format_text,
				"text/plain");

		flAdapter = new FileListAdapter(extensionList, true);
		flAdapter.setPath(Environment.getExternalStorageDirectory().getPath());
		this.setAdapter(flAdapter);
		this.setOnItemClickListener(onItemClickListener);
	}

	public void setOnClickFileItemListener(OnClickFileItemListener listener) {
		onClickFileItemListener = listener;
	}

	public boolean OpenChild(int position) {
		Integer parentPosition = Integer.valueOf(getFirstVisiblePosition());
		FileInfo fi = (FileInfo) flAdapter.getItem(position);
		if (fi.file.isDirectory()) {
			if (flAdapter.currentPath.equals("/"))
				flAdapter.currentPath = "";
			FileListAdapter p = new FileListAdapter(flAdapter.extensionList,
					flAdapter.bAcceptUnkownExtension);

			if (p.setPath(flAdapter.currentPath + "/" + fi.file.getName())) {
				if (onChangePathListener != null) {
					this.onChangePathListener.OnChangePath(p.currentPath);
				}
				flAdapter = p;
				this.setAdapter(p);
				parentsPositionStack.add(parentPosition);
				return true;
			} else {
				return false;
			}
		}
		return false;

	}

	public boolean OpenParent() {
		/*
		 * if(flAdapter.OpenParent()) { if(parentsPositionStack.size()>0) {
		 * this.post(new Runnable() {
		 * 
		 * @Override public void run() { Integer
		 * parentPos=parentsPositionStack.get(parentsPositionStack.size()-1);
		 * parentsPositionStack.remove(parentPos);
		 * setSelection(parentPos.intValue()); } }); } return true; } return
		 * false;
		 */
		String Path = flAdapter.currentPath;
		ArrayList<ExtensionInfo> e = flAdapter.extensionList;
		boolean bAccept = flAdapter.bAcceptUnkownExtension;
		String parentPath = new File(Path).getParent();
		if (parentPath == null) {
			Log.e("RootFloder", "sdfsdfagasd");
			return false;
		} else {
			FileListAdapter p = new FileListAdapter(e, bAccept);
			if (p.setPath(parentPath)) {

				this.setAdapter(p);
				if (onChangePathListener != null) {
					this.onChangePathListener.OnChangePath(parentPath);
				}
				flAdapter = p;
				if (parentsPositionStack.size() > 0) {
					Integer parentPos = parentsPositionStack
							.get(parentsPositionStack.size() - 1);
					parentsPositionStack.remove(parentPos);
					setSelection(parentPos.intValue());
				}
				return true;
			} else {
				return false;
			}
		}
	}

	public interface OnChangePathListener {
		public void OnChangePath(String path);
	}

	public void setOnChangePathListener(OnChangePathListener listener) {
		onChangePathListener = listener;
		onChangePathListener.OnChangePath(flAdapter.currentPath);
	}

	OnChangePathListener onChangePathListener;
}
