
#pragma once

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "ditto/assert.h"

namespace Ditto {

class MockAssert {
 public:
  MOCK_METHOD(void, assert_failed, (const char*, int, const char*), ());
  MOCK_METHOD(void, unimplemented, (const char*, int, const char*), ());
};

extern testing::StrictMock<MockAssert>* g_assert;

}  // namespace Ditto
