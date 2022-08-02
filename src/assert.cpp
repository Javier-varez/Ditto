#include "ditto/assert.h"

#include <cstdint>
#include <cstdio>
#include <cstdlib>

#include "ditto/stack_trace.h"

namespace Ditto {

void print_stack_trace() {
  printf("Stack trace:\n");
  uint32_t i = 0;
  walk_stack_trace([&i](const StackFrame& frame) {
    printf("[%d]: %p\n", i++, frame.return_address);
  });
}

[[gnu::weak]] void assert_failed(const char* condition, int line,
                                 const char* file) {
  printf("Condition failed: %s at line: %d, file: %s\n", condition, line, file);
  print_stack_trace();
  exit(EXIT_FAILURE);
}

[[gnu::weak]] void unimplemented(const char* function, int line,
                                 const char* file) {
  printf("Unimplemented function: %s at line: %d, file: %s\n", function, line,
         file);
  print_stack_trace();
  exit(EXIT_FAILURE);
}

}  // namespace Ditto
