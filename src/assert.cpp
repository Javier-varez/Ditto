
#include "ditto/assert.h"

#include <cstdio>
#include <cstdlib>

namespace Ditto {

__attribute__((weak)) void assert_failed(const char* condition, int line,
                                         const char* file) {
  printf("Condition failed: %s at line: %d, file: %s\n", condition, line, file);
  exit(EXIT_FAILURE);
}

__attribute__((weak)) void unimplemented(const char* function, int line,
                                         const char* file) {
  printf("Unimplemented function: %s at line: %d, file: %s\n", function, line,
         file);
  exit(EXIT_FAILURE);
}

}  // namespace Ditto
