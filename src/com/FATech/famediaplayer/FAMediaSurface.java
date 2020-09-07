package com.FATech.famediaplayer;

import android.content.Context;
import android.util.AttributeSet;
import android.view.Surface;
import android.view.SurfaceView;

public class FAMediaSurface extends SurfaceView {

	// 该部分代码会在新建Class时候运行 可载入多个库
	//static {
	//	System.loadLibrary("famedia");
	//}
	
	//public native void sendSurfaceToNative(Surface surface);
	
	public FAMediaSurface(Context context) {
		super(context);
		// TODO Auto-generated constructor stub
		
	}
	public FAMediaSurface(Context context, AttributeSet attrs) {
        this(context, attrs , 0);
    }

    public FAMediaSurface(Context context, AttributeSet attrs, int defStyle) {
        super(context, attrs, defStyle);
    }
    public void start()
    {
    //	sendSurfaceToNative(this.getHolder().getSurface());
    }
    
}
