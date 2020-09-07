package com.FATech.FileList;

import com.FATech.famediaplayer.*;

import android.graphics.drawable.Drawable;
import android.os.Environment;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.TextView;
import android.widget.BaseAdapter;

import java.io.FilenameFilter;
import java.io.File;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Iterator;

public class FileListAdapter extends BaseAdapter {
	ArrayList<ExtensionInfo> extensionList = new ArrayList<ExtensionInfo>();
	ExtensionInfo dirInfo = new ExtensionInfo(".", R.drawable.format_folder,
			null);
	ExtensionInfo unknownFileInfo = new ExtensionInfo("*",
			R.drawable.format_unkown, null);
	boolean bAcceptUnkownExtension;
	String currentPath;
	ArrayList<FileInfo> contents = new ArrayList<FileInfo>();

	FilenameFilter filenameFilter = new FilenameFilter() {
		@Override
		public boolean accept(File dir, String filename) {
			Log.d("FileNameFilter ", "Path of dir:" + dir.getPath());
			File file = new File(dir.getPath() + "/" + filename);
			if (file.isDirectory()) {
				return true;
			}
			String extension = getExtensionOfFilename(filename);
			Iterator<ExtensionInfo> extensionIterator = extensionList
					.iterator();
			while (extensionIterator.hasNext()) {
				ExtensionInfo nextExtensionInfo = extensionIterator.next();
				if (bAcceptUnkownExtension
						|| nextExtensionInfo.name.equals(extension)) {
					return true;
				}
			}
			return false;
		}

		private String getExtensionOfFilename(String name) {

			if (name.length() < 1) {
				return "";
			}
			int i = name.length() - 1;
			for (; i >= 0; i--) {
				if (name.charAt(i) == '.') {
					break;
				}
			}
			if (i > 0) {
				return name.substring(i + 1);
			} else {
				return "";
			}
		}
	};

	public FileListAdapter(ExtensionInfo extensionName[],
			boolean acceptUnkownExtension) {
		this.bAcceptUnkownExtension = acceptUnkownExtension;
		setExtensionFilters(extensionName);
		//setPath(path);
	}
	
	public FileListAdapter(ArrayList<ExtensionInfo> extensionName,boolean acceptUnkownExtension) {
		this.bAcceptUnkownExtension = acceptUnkownExtension;
		this.extensionList=extensionName;
		//setPath(path);
	}

	public boolean setPath(String path) {
		File file;
		if (path == null || path.length() < 1)
			file = Environment.getExternalStorageDirectory();
		else
			file = new File(path);
		if (!file.isDirectory()) {
			Log.d("FileListAdapter.setPath: ", "path is not a directory!");
			return false;
		}

		File[] newContents = file.listFiles(filenameFilter);

		if (newContents == null)
			return false;
		Arrays.sort(newContents);

		currentPath = file.getPath();
		contents.clear();

		for (int i = 0; i < newContents.length; i++) {
			FileInfo p = new FileInfo();
			p.file = newContents[i];
			p.fileExtension = p.getExtensionOfFilename();
			if (p.file.isDirectory()) {
				p.IconResourceId = dirInfo.IconResourceId;
				p.extensioninfo = dirInfo;
			} else {
				p.IconResourceId = findIconId(p.fileExtension);
			}
			if (p.extensioninfo == null) {
				for (int j = 0; j < extensionList.size(); j++) {
					ExtensionInfo e = extensionList.get(j);
					if (p.fileExtension.equals(e.name)) {
						p.extensioninfo = e;
						break;
					}
				}
			}
			if (p.extensioninfo == null) {
				p.extensioninfo = unknownFileInfo;
			}
			contents.add(p);
		}
		notifyDataSetChanged();
		return true;
	}

	private int findIconId(String fileExtension) {
		for (int i = 0; i < extensionList.size(); i++) {
			ExtensionInfo ei = extensionList.get(i);
			if (ei.name.equals(fileExtension)) {
				return ei.IconResourceId;
			}
		}
		return unknownFileInfo.IconResourceId;
	}

	public void setExtensionFilters(ExtensionInfo extensionName[]) {
		if (extensionName != null) {
			extensionList.clear();
			for (int i = 0; i < extensionName.length; i++) {
				extensionList.add(extensionName[i]);
			}
		}
	}

	public void addExtensionFilter(ExtensionInfo extensionName) {
		extensionList.add(extensionName);
	}

	@Override
	public int getCount() {
		// TODO Auto-generated method stub
		return contents.size();
	}

	@Override
	public Object getItem(int position) {
		// TODO Auto-generated method stub
		return contents.get(position);
	}

	@Override
	public long getItemId(int position) {
		// TODO Auto-generated method stub
		return position;
	}

	@Override
	public View getView(int position, View convertView, ViewGroup parent) {
		if (convertView == null) {
			LayoutInflater inflater = LayoutInflater.from(parent.getContext());
			convertView = inflater.inflate(R.layout.item_file, null);
		}
		TextView textView1 = (TextView) convertView
				.findViewById(R.id.textView1);
		FileInfo fi = (FileInfo) this.getItem(position);
		textView1.setText(fi.file.getName());
		Drawable d = parent.getContext().getResources()
				.getDrawable(fi.IconResourceId);
		d.setBounds(0, 0, 96, 96);
		textView1.setCompoundDrawables(d, null, null, null);
		textView1.invalidate();
		return convertView;
	}

	public boolean OpenChild(int position) {
		if (position < 0 || position >= contents.size())
			return false;
		FileInfo fi = (FileInfo) contents.get(position);
		if (fi.file.isDirectory()) {
			if (currentPath.equals("/"))
				currentPath = "";
			setPath(currentPath + "/" + fi.file.getName());
			return true;
		}
		return false;
	}

	public boolean OpenParent() {
		String parentPath = new File(currentPath).getParent();
		if (parentPath.equals("/")) {
			return false;
		} else {
			setPath(parentPath);
			return true;
		}
	}

	public class FileInfo {
		public File file;
		public int IconResourceId;
		public String fileExtension = null;
		public ExtensionInfo extensioninfo;

		public String getExtensionOfFilename() {
			if (fileExtension == null) {
				String name = file.getName();
				if (name.length() < 1) {
					fileExtension = "";
					return fileExtension;
				}
				int i = name.length() - 1;
				for (; i >= 0; i--) {
					if (name.charAt(i) == '.') {
						break;
					}
				}
				if (i > 0) {
					fileExtension = name.substring(i + 1);
				} else {
					fileExtension = "";
				}
			}
			return fileExtension;

		}
	}

}
