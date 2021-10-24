
#include "ditto/non_null_ptr.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <memory>

#include "assert_mock.h"

using testing::_;
using testing::StrictMock;

TEST(NonNullPtrTest, Nullptr) {
  StrictMock<Ditto::MockAssert> assert;
  Ditto::g_assert = &assert;
  EXPECT_CALL(assert, assert_failed(_, _, _));
  int* null_int = nullptr;
  Ditto::NonNullPtr<int> ptr{null_int};
  Ditto::g_assert = nullptr;
}

TEST(NonNullPtrTest, FromRawPointer) {
  int val = 0;
  auto mutate = [](Ditto::NonNullPtr<int> int_ptr) { *int_ptr = 7; };

  mutate(&val);

  EXPECT_EQ(val, 7);
}

TEST(NonNullPtrTest, FromUniquePtr) {
  std::unique_ptr<int> val_ptr = std::make_unique<int>(0);
  auto mutate = [](Ditto::NonNullPtr<int> int_ptr) { *int_ptr = 7; };

  mutate(val_ptr);

  EXPECT_EQ(*val_ptr, 7);
}

TEST(NonNullPtrTest, FromSharedPointer) {
  std::shared_ptr<int> val_ptr = std::make_shared<int>(0);
  auto mutate = [](Ditto::NonNullPtr<int> int_ptr) { *int_ptr = 7; };

  mutate(val_ptr);

  EXPECT_EQ(*val_ptr, 7);
}
