
#include "ditto/resource_lock.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <memory>
#include <string>

using testing::StrictMock;

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

TEST(ResourceLockTest, can_return_value_from_closure) {
  Ditto::ResourceLock<int, std::mutex> int_resource{123};

  auto val = int_resource.lock([](auto& res) { return res; });
  EXPECT_EQ(val, 123);
}

class MutexMock {
 public:
  MOCK_METHOD(void, lock, (), ());
  MOCK_METHOD(void, unlock, (), ());
};

StrictMock<MutexMock>* g_mutex_mock = nullptr;

class DummyMutex {
 public:
  void lock() {
    if (g_mutex_mock) g_mutex_mock->lock();
  }
  void unlock() {
    if (g_mutex_mock) g_mutex_mock->unlock();
  }
};

TEST(ResourceLockTest, locks_and_unlocks_resource) {
  Ditto::ResourceLock<int, DummyMutex> int_resource{123};
  StrictMock<MutexMock> mutex_mock;
  g_mutex_mock = &mutex_mock;

  EXPECT_CALL(mutex_mock, lock());
  int_resource.lock([&](auto& res) {
    // At this point lock must have been called
    testing::Mock::VerifyAndClearExpectations(g_mutex_mock);

    EXPECT_EQ(res, 123);
    EXPECT_CALL(mutex_mock, unlock());
  });
}

TEST(ReadWriteLockTest, can_default_construct) {
  Ditto::ReadWriteLock<int, std::mutex> int_resource;
}

TEST(ReadWriteLockTest, can_construct_resource_forwarding_args) {
  Ditto::ReadWriteLock<int, std::mutex> int_resource{123};
  Ditto::ReadWriteLock<std::string, std::mutex> string_resource{"my_string"};
}

TEST(ReadWriteLockTest, can_lock_resource_for_reading) {
  Ditto::ReadWriteLock<int, std::mutex> int_resource{123};

  int_resource.read_lock([](const int& res) { EXPECT_EQ(res, 123); });

  int_resource.write_lock([](int& res) {});
}

TEST(ReadWriteLockTest, can_lock_resource_for_writing) {
  Ditto::ReadWriteLock<int, std::mutex> int_resource{123};

  int_resource.write_lock([](int& res) { res = 1234; });

  int_resource.read_lock([](const int& res) { EXPECT_EQ(res, 1234); });
}

TEST(ReadWriteLockTest, can_lock_resource_for_reading_twice) {
  Ditto::ReadWriteLock<int, std::mutex> int_resource{123};

  int_resource.read_lock([&](const int& res) {
    int_resource.read_lock([](const int& res) { EXPECT_EQ(res, 123); });
  });
}

TEST(ResourceLockTest, can_return_value_from_read_closure) {
  Ditto::ReadWriteLock<int, std::mutex> int_resource{123};

  int val = int_resource.read_lock([](const auto& res) { return res; });
  EXPECT_EQ(val, 123);
}

TEST(ResourceLockTest, can_return_value_from_write_closure) {
  Ditto::ReadWriteLock<int, std::mutex> int_resource{123};

  auto val = int_resource.write_lock([](auto& res) {
    const auto prev = res;
    res = 0;
    return prev;
  });

  EXPECT_EQ(val, 123);
}

TEST(ResourceLockTest, moves_out_in_write_lock) {
  Ditto::ReadWriteLock<std::unique_ptr<int>, std::mutex> int_resource{
      std::make_unique<int>(123)};

  std::unique_ptr<int> val =
      int_resource.write_lock([](auto& res) { return std::move(res); });
  EXPECT_TRUE(val);
  EXPECT_EQ(*val, 123);
}
