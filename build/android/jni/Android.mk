
LOCAL_PATH:= $(call my-dir)/../../../
#include $(CLEAR_VARS)
#include $(libjingle-root-dir)/../../../thirdparty/lib/android/Android.mk



include $(CLEAR_VARS)


LOCAL_ARM_MODE := arm 
LOCAL_PRELINK_MODULE := false
LOCAL_MODULE := app

LOCAL_CPPFLAGS := -fexceptions

LOCAL_MODULE_TAGS := optional

#-DJSON_IS_AMALGAMATION 
	
	
LOCAL_SRC_FILES := \
				src/httpClient.cpp \
				src/httpjson.cpp \
				src/JsonBase.cpp \
				src/main.cpp \


#jsoncpp.cpp

LOCAL_C_INCLUDES :=  \
					$(LOCAL_PATH)/interface  \
					$(LOCAL_PATH)/src/  \
					$(LOCAL_PATH)/thirdparty/include 


#LOCAL_STATIC_LIBRARIES := jsoncpp curl 

LOCAL_LDLIBS += $(LOCAL_PATH)/thirdparty/lib/android/libjsoncpp.a
LOCAL_LDLIBS += $(LOCAL_PATH)/thirdparty/lib/android/libcurl.a



include $(BUILD_EXECUTABLE)
#include $(BUILD_STATIC_LIBRARY)
