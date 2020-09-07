LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_CFLAGS := -D__STDC_CONSTANT_MACROS -D__STDC_FORMAT_MACROS -g

LOCAL_C_INCLUDES += \
	$(LOCAL_PATH)/

LOCAL_ARM_MODE := arm

LOCAL_LDLIBS += -llog
LOCAL_LDLIBS += -lOpenSLES
LOCAL_LDLIBS += -landroid
LOCAL_SHARED_LIBRARIES +=ffmpeg
LOCAL_SRC_FILES := jniUtils.cpp\
                   FFMpegPlayer.cpp\
                   FFMpegJNI.cpp\
                   thread.cpp \
                   FAPktList.cpp\
                   FAAudioDecoder.cpp\
                   FAVideoDecoder.cpp

LOCAL_MODULE := libfamedia

include $(BUILD_SHARED_LIBRARY)
