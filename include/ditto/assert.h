
#pragma once

namespace Ditto {

void assert_failed(const char* condition, int line, const char* file);

#define DITTO_STRINGIFY(s) #s

#define DITTO_VERIFY(x)                                    \
  if (!(x)) {                                              \
    assert_failed(DITTO_STRINGIFY(x), __LINE__, __FILE__); \
  }

}  // namespace Ditto
