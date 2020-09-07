package com.FATech.FileList;

public class ExtensionInfo {
	public String name;
	public int IconResourceId;
	public String mimeType;
	public ExtensionInfo(String name,int iconid,String MIMEType){
		this.name=name;
		this.IconResourceId=iconid;
		this.mimeType=MIMEType;
	}
}
