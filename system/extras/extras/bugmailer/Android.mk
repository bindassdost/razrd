# Copyright 2011 The Android Open Source Project
#
LOCAL_PATH:= $(call my-dir)
#<2013/01/24-hwcheng, Fixed BU2SC00142323 take bug report can work
#ifneq ($(TARGET_BUILD_PDK), true)
#>2013/01/24-hwcheng
include $(CLEAR_VARS)

LOCAL_SRC_FILES := $(call all-java-files-under, src)
LOCAL_MODULE := send_bug
LOCAL_MODULE_TAGS := optional
include $(BUILD_JAVA_LIBRARY)
#<2013/01/24-hwcheng, Fixed BU2SC00142323 take bug report can work
#endif
#>2013/01/24-hwcheng
