#ifndef _JNI_UTILS_H_
#define _JNI_UTILS_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdlib.h>
#include <jni.h>

int registerAllMethodsCallback(JNIEnv *env);
int jniThrowException(JNIEnv* env, const char* className, const char* msg);

JNIEnv* getJNIEnv();

int jniRegisterNativeMethods(JNIEnv* env, const char* className, const JNINativeMethod* gMethods, int numMethods);

#ifdef __cplusplus
}
#endif

#endif
