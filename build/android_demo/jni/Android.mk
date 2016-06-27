
LOCAL_PATH:= $(call my-dir)/../



include $(CLEAR_VARS)

LOCAL_MODULE          := libhttpjson
LOCAL_SRC_FILES :=  \
			src/libhttp-json.so

include $(PREBUILT_SHARED_LIBRARY)



include $(CLEAR_VARS)

LOCAL_ARM_MODE := arm 
LOCAL_PRELINK_MODULE := false
LOCAL_MODULE := app

LOCAL_CPPFLAGS := -fexceptions

LOCAL_MODULE_TAGS := optional

#-DJSON_IS_AMALGAMATION 



#LOCAL_STATIC_LIBRARIES := \
              gnustl_static \

LOCAL_SHARED_LIBRARIES := \
				httpjson
	
	
LOCAL_SRC_FILES := \
				src/main_android.cpp \


#jsoncpp.cpp

LOCAL_C_INCLUDES :=  \
					$(LOCAL_PATH)/src  \


include $(BUILD_EXECUTABLE)
