LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE    := libcurl
LOCAL_SRC_FILES := libcurl.a   
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := libjsoncpp
LOCAL_SRC_FILES := libjsoncpp.a   
include $(PREBUILT_STATIC_LIBRARY)



