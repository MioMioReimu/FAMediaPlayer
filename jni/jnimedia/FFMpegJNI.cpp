#include <assert.h>
#include "jniUtils.h"
#include "FFMpegPlayer.h"
#include "FAVideoDecoder.h"
extern FFMpegPlayer ffmpegPlayer;
extern "C" {
	void start(JNIEnv *env,jclass clazz);
	void pause(JNIEnv *env,jclass clazz);
	void prepare(JNIEnv *env,jclass clazz);
	void setDataSource(JNIEnv* env,jclass clazz,jstring filepath);
	void setSurface(JNIEnv *env,jclass,jobject surface);
	jint getVideoHeight(JNIEnv *env,jclass clazz);
	jint getVideoWidth(JNIEnv *env,jclass clazz);
	jint getDuration(JNIEnv *env,jclass clazz);
	jint getCurrentPosition(JNIEnv *env,jclass clazz);
	void seekTo(JNIEnv *env,jclass clazz,jint msec);
	jint getState(JNIEnv *env,jclass clazz);
	void stop(JNIEnv* env,jclass clazz);
	void release(JNIEnv* env,jclass clazz);
}
static JNINativeMethod methods[]={
		{"start","()V",(void*)start},
		{"pause","()V",(void*)pause},
		{"prepare","()V",(void*)prepare},
		{"setDataSource","(Ljava/lang/String;)V",(void*)setDataSource},
		{"setSurface","(Landroid/view/Surface;)V",(void*)setSurface},
		{"getVideoHeight","()I",(void*)getVideoHeight},
		{"getVideoWidth","()I",(void*)getVideoWidth},
		{"getCurrentPosition","()I",(void*)getCurrentPosition},
		{"getDuration","()I",(void*)getDuration},
		{"seekTo","(I)V",(void*)seekTo},
		{"getState","()I",(void*)getState},
		{"stop","()V",(void*)stop},
		{"release","()V",(void*)release}
};
int registerAllMethodsCallback(JNIEnv *env){
	int result;
	result=jniRegisterNativeMethods(env,"com/FATech/media/FAMediaPlayer",
			methods,sizeof(methods)/sizeof(methods[0]));
	return result;
}
void start(JNIEnv *env,jclass clazz) {
	ffmpegPlayer.play();
}

void stop(JNIEnv *env,jclass clazz){
	ffmpegPlayer.stop();
}

void release(JNIEnv* env,jclass clazz){
	ffmpegPlayer.release();
}

void pause(JNIEnv *env,jclass clazz) {
	ffmpegPlayer.pause();
}
void setSurface(JNIEnv *env,jclass,jobject surface) {
	ANativeWindow *p=ANativeWindow_fromSurface(env,surface);
	ffmpegPlayer.videoPlayer->setWindow(p);
}
void prepare(JNIEnv *env,jclass clazz) {
	ffmpegPlayer.prepare();
}

void setDataSource(JNIEnv* env,jclass clazz,jstring filepath){
	const char *utf8Path=env->GetStringUTFChars(filepath,NULL);
	assert(NULL!=utf8Path);
	ffmpegPlayer.setDataSource(utf8Path);
	env->ReleaseStringUTFChars(filepath,utf8Path);
}

jint getVideoHeight(JNIEnv *env,jclass clazz) {
	return ffmpegPlayer.getVideoHeight();
}
jint getVideoWidth(JNIEnv *env,jclass clazz) {
	return ffmpegPlayer.getVideoWidth();

}
jint getDuration(JNIEnv *env,jclass clazz) {
	return ffmpegPlayer.getDuration();
}
jint getCurrentPosition(JNIEnv *env,jclass clazz) {
	return ffmpegPlayer.getCurrentPosition();
}
void seekTo(JNIEnv *env,jclass clazz,jint msec){
	ffmpegPlayer.seekTo(msec);
}
jint getState(JNIEnv *env,jclass clazz){
	return ffmpegPlayer.getState();
}
