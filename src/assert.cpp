
#include "ATE/assert.h"

#include <cstdio>

namespace ATE {

__attribute__((weak)) void assert_failed(const char* condition, int line,
                                         const char* file) {
  printf("Condition failed: %s at line: %d, file: %s\n", condition, line, file);
}

}  // namespace ATE
