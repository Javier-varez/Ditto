
#include "ditto/assert.h"

#include <gmock/gmock.h>

#include "assert_mock.h"

// IWYU pragma: no_forward_declare Ditto::MockAssert

namespace Ditto {

testing::StrictMock<MockAssert>* g_assert;

void assert_failed(const char* condition, int line, const char* file) {
  g_assert->assert_failed(condition, line, file);
}

void unimplemented(const char* function, int line, const char* file) {
  g_assert->unimplemented(function, line, file);
}

}  // namespace Ditto
