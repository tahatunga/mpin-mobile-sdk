LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    :=  AndroidMpinSDK
LOCAL_SRC_FILES :=  AndroidMpinSDK.cpp \
					HTTPConnector.cpp \
					Storage.cpp 
LOCAL_CFLAGS := -fexceptions
LOCAL_LDLIBS := -llog

include $(BUILD_SHARED_LIBRARY)
