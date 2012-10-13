LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := FLUID

# change if it's different
SDL_PATH := $(LOCAL_PATH)/../SDL

LOCAL_C_INCLUDES := $(LOCAL_PATH)/src $(SDL_PATH)/include

LOCAL_SRC_FILES := \
	$(subst $(LOCAL_PATH)/,, \
	$(wildcard $(LOCAL_PATH)/src/*.cpp) \
	$(wildcard $(LOCAL_PATH)/src/audio/*.cpp) \
	$(wildcard $(LOCAL_PATH)/src/3rdparty/*.cpp) \
	$(wildcard $(LOCAL_PATH)/src/utils/*.cpp))

LOCAL_CFLAGS += -DFL_SDL_BACKEND -DFL_OPENGL_ES2 -DFL_ANDROID
LOCAL_LDLIBS := -lGLESv1_CM -lGLESv2

LOCAL_SHARED_LIBRARIES := SDL2

include $(BUILD_STATIC_LIBRARY)
