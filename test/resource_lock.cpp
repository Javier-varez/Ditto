
#include "ditto/resource_lock.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <string>

TEST(ResourceLockTest, can_default_construct) {
  Ditto::ResourceLock<int, std::mutex> resource;
}

TEST(ResourceLockTest, can_construct_resource_forwarding_args) {
  Ditto::ResourceLock<int, std::mutex> int_resource{123};
  Ditto::ResourceLock<std::string, std::mutex> str_resource{"my_string"};
}

TEST(ResourceLockTest, can_lock_resource) {
  Ditto::ResourceLock<int, std::mutex> int_resource{123};

  int_resource.lock([](auto& res) {
    EXPECT_EQ(res, 123);
    res = 3;
  });

  int_resource.lock([](auto& res) { EXPECT_EQ(res, 3); });
}
