
#include "ATE/non_null_ptr.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "assert_mock.h"

using testing::_;
using testing::StrictMock;

TEST(NonNullPtrTest, Nullptr) {
  StrictMock<ATE::MockAssert> assert;
  ATE::g_assert = &assert;
  EXPECT_CALL(assert, assert_failed(_, _, _));
  ATE::NonNullPtr<int> ptr{nullptr};
  ATE::g_assert = nullptr;
}
