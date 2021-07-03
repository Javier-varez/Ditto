
LOCAL_DIR := $(call current-dir)

include $(CLEAR_VARS)

CC := clang
CXX := clang++

LOCAL_NAME := container_tests
LOCAL_SRC := \
    $(LOCAL_DIR)/test/linked_list.cpp

LOCAL_CXXFLAGS := \
	-I$(LOCAL_DIR)/inc \
	-Wall \
	-Wextra \
	-Werror \
	-std=c++17

include $(BUILD_HOST_TEST)
