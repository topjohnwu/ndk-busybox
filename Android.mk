LOCAL_PATH := $(call my-dir)
VERSION = 1
PATCHLEVEL = 29
SUBLEVEL = 3
EXTRAVERSION = -topjohnwu
BB_VER = $(VERSION).$(PATCHLEVEL).$(SUBLEVEL)$(EXTRAVERSION)

include $(CLEAR_VARS)
LOCAL_MODULE := busybox

LOCAL_C_INCLUDES := $(LOCAL_PATH)/include
include $(LOCAL_PATH)/Android_src.mk

LOCAL_LDFLAGS := -static
LOCAL_CFLAGS := \
-w -include include/autoconf.h -D__USE_BSD \
-DBB_VER=\"$(BB_VER)\" -DBB_BT=AUTOCONF_TIMESTAMP

include $(BUILD_EXECUTABLE)
