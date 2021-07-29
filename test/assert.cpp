
#include "ATE/assert.h"

#include <gmock/gmock.h>

#include "assert_mock.h"

// IWYU pragma: no_forward_declare ATE::MockAssert

namespace ATE {

testing::StrictMock<MockAssert>* g_assert;

void assert_failed(const char* condition, int line, const char* file) {
  g_assert->assert_failed(condition, line, file);
}

}  // namespace ATE
