#ifndef DITTO_ASSERT_H_
#define DITTO_ASSERT_H_

#define DITTO_STRINGIFY(s) #s

#ifndef NDEBUG

namespace Ditto {
void assert_failed(const char* condition, int line, const char* file);

void unimplemented(const char* function, int line, const char* file);
}  // namespace Ditto

#define DITTO_VERIFY(x)                                           \
  if (!(x)) {                                                     \
    Ditto::assert_failed(DITTO_STRINGIFY(x), __LINE__, __FILE__); \
  }

#define DITTO_UNIMPLEMENTED() \
  Ditto::unimplemented(__PRETTY_FUNCTION__, __LINE__, __FILE__)

#else

#define DITTO_VERIFY(x) ((void)(x))
#define DITTO_UNIMPLEMENTED()

#endif

#endif  // DITTO_ASSERT_H_
