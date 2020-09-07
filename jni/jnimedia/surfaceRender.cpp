#include <assert.h>
#include <jni.h>
#include <unistd.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <android/log.h>
#include <android/native_window_jni.h>
#include <android/native_window.h>
#include <android/rect.h>
#include "jniUtils.h"
ANativeWindow* p_mediaWindow;
int mediaWindowWidth;
int mediaWindowHeight;
int mediaWindowFormat;
ANativeWindow_Buffer mediaWindowBufferObject;
ARect dirtyRect;
extern int ANativeWindow_setBuffersGeometry(ANativeWindow *window,int width,int height,int format);
typedef  int int32_t;
int ifstarted=0;
void sendSurfaceToNative(JNIEnv *env,jclass classz, jobject surface)
{
	p_mediaWindow=ANativeWindow_fromSurface(env,surface);
	//ANativeWindow_acquire(p_mediaWindow);
	//ANativeWindow_setBuffersGeometry(p_mediaWindow,0,0,0);
	mediaWindowWidth=ANativeWindow_getWidth(p_mediaWindow);
	mediaWindowHeight=ANativeWindow_getHeight(p_mediaWindow);
	mediaWindowFormat=ANativeWindow_getFormat(p_mediaWindow);
	//mediaWindowBufferObject.format=mediaWindowFormat;
	//mediaWindowBufferObject.width=mediaWindowWidth;
	//mediaWindowBufferObject.height=mediaWindowHeight;
	ANativeWindow_setBuffersGeometry(p_mediaWindow,mediaWindowWidth,mediaWindowHeight,WINDOW_FORMAT_RGB_565);
	mediaWindowFormat=ANativeWindow_getFormat(p_mediaWindow);
	dirtyRect.left=0;
	dirtyRect.top=0;
	dirtyRect.right=mediaWindowWidth;
	dirtyRect.bottom=mediaWindowHeight;
	ifstarted=100;
	while(ifstarted>0){
	ANativeWindow_lock(p_mediaWindow,&mediaWindowBufferObject,&dirtyRect);
	__android_log_print(ANDROID_LOG_ERROR,"NativeSurface","windowHeight:%d\nwindowWidth:%d",
	mediaWindowHeight,mediaWindowWidth);
	__android_log_print(ANDROID_LOG_ERROR,"NativeSurface","windowHeight:%d\nwindowWidth:%d",
			mediaWindowBufferObject.height,mediaWindowBufferObject.width);
	unsigned short *p=(unsigned short *)	mediaWindowBufferObject.bits;//new unsigned short[mediaWindowBufferObject.height*mediaWindowBufferObject.stride];
	for(int i=0;i<mediaWindowBufferObject.height;i++)
	{
		for(int j=0;j<mediaWindowBufferObject.width;j++){
			p[j+i*mediaWindowBufferObject.stride]=0xffff;
		}
	}
	__android_log_print(ANDROID_LOG_ERROR,"NativeSurface","WindowBufferLineSize:%d",mediaWindowBufferObject.stride);
	__android_log_print(ANDROID_LOG_ERROR,"NativeSurface","windowFormat:%d",
		mediaWindowFormat);
	ANativeWindow_unlockAndPost(p_mediaWindow);
	//delete []p;
	ifstarted--;
	usleep(50);
	}
	ANativeWindow_release(p_mediaWindow);
	//;

}
static JNINativeMethod methods[]= {
		{"sendSurfaceToNative","(Landroid/view/Surface;)V",(void*)sendSurfaceToNative },
};
int registerAllMethodsCallback(JNIEnv *env)
{
	return jniRegisterNativeMethods(env,"com/FATech/testndkmedia/FAMediaSurface",methods,sizeof(methods)/sizeof(methods[0]));
}

