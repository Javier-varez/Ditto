LOCAL_DIR := $(call current-dir)

DITTO_CFLAGS := \
    -I$(LOCAL_DIR)/include \
    -Os \
    -g3 \
    -Wall \
    -Werror \
    -Wextra \
    -ffunction-sections \
    -fdata-sections

DITTO_CXXFLAGS := \
    -std=gnu++20 \
    -fno-exceptions \
    -fno-rtti

DITTO_SRC := \
    $(LOCAL_DIR)/src/assert.cpp \
    $(LOCAL_DIR)/src/hash.cpp

include $(CLEAR_VARS)
LOCAL_NAME := ditto
TARGET_CFLAGS := \
    -mcpu=cortex-m3 \
    -mfloat-abi=soft \
    -mthumb
LOCAL_CFLAGS := \
    $(TARGET_CFLAGS) \
    $(DITTO_CFLAGS)
LOCAL_CXXFLAGS := \
    $(LOCAL_CFLAGS) \
    $(DITTO_CXXFLAGS)
LOCAL_SRC := \
    $(DITTO_SRC)
LOCAL_ARM_ARCHITECTURE := v7-m
LOCAL_ARM_FPU := nofp
LOCAL_COMPILER := arm_clang
LOCAL_ARFLAGS := -rcs
LOCAL_EXPORTED_DIRS := \
    $(LOCAL_DIR)/include
include $(BUILD_STATIC_LIB)

include $(CLEAR_VARS)
CC := clang
CXX := clang++
LOCAL_NAME := ditto_host
LOCAL_CFLAGS := \
    $(DITTO_CFLAGS)
LOCAL_CXXFLAGS := \
    $(LOCAL_CFLAGS) \
    $(DITTO_CXXFLAGS)
LOCAL_SRC := \
    $(DITTO_SRC)
LOCAL_ARFLAGS := -rcs
LOCAL_EXPORTED_DIRS := \
    $(LOCAL_DIR)/include
include $(BUILD_STATIC_LIB)
