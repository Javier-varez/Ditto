
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
  Ditto::NonNullPtr<int> ptr{nullptr};
  Ditto::g_assert = nullptr;
}
