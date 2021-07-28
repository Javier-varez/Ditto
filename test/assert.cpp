
#include "assert_mock.h"

namespace ATE {

testing::StrictMock<MockAssert>* g_assert;

void assert_failed(const char* condition, int line, const char* file) {
  g_assert->assert_failed(condition, line, file);
}

}  // namespace ATE
