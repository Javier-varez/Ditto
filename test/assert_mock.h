
#pragma once

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "ATE/assert.h"

namespace ATE {

class MockAssert {
 public:
  MOCK_METHOD(void, assert_failed, (const char*, int, const char*), ());
};

extern testing::StrictMock<MockAssert>* g_assert;

}  // namespace ATE
