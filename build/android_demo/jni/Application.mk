APP_PROJECT_PATH := $(call my-dir)/
libjingle-root-dir:=$(APP_PROJECT_PATH)
APP_BUILD_SCRIPT:=$(call my-dir)/Android.mk
#APP_MODULES := libhttp-json
APP_STL := gnustl_static
APP_ABI := armeabi-v7a 
#APP_ABI := armeabi
APP_PLATFORM := android-14 
#APP_CPPFLAGS += -fexceptions
APP_CPPFLAGS += -frtti
#APP_CFLAGS:=-DDISABLE_NEON



