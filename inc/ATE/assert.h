
#pragma once

namespace ATE {

void assert_failed(const char* condition, int line, const char* file);

#define ATE_STRINGIFY(s) #s

#define ATE_VERIFY(x)                                    \
  if (!(x)) {                                            \
    assert_failed(ATE_STRINGIFY(x), __LINE__, __FILE__); \
  }

}  // namespace ATE
