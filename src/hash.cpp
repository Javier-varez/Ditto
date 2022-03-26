
#include "ditto/hash.h"

#include <cstring>

#include "ditto/non_null_ptr.h"

namespace Ditto {

void SimpleHasher::hash(std::uint64_t value) {
  const Ditto::NonNullPtr ptr = reinterpret_cast<const uint8_t*>(&value);
  m_inner.hash(Ditto::span{ptr, sizeof(value)});
}

void SimpleHasher::hash(std::int64_t value) {
  const Ditto::NonNullPtr ptr = reinterpret_cast<const uint8_t*>(&value);
  m_inner.hash(Ditto::span{ptr, sizeof(value)});
}

void SimpleHasher::hash(std::uint32_t value) {
  const Ditto::NonNullPtr ptr = reinterpret_cast<const uint8_t*>(&value);
  m_inner.hash(Ditto::span{ptr, sizeof(value)});
}

void SimpleHasher::hash(std::int32_t value) {
  const Ditto::NonNullPtr ptr = reinterpret_cast<const uint8_t*>(&value);
  m_inner.hash(Ditto::span{ptr, sizeof(value)});
}

void SimpleHasher::hash(std::uint16_t value) {
  const Ditto::NonNullPtr ptr = reinterpret_cast<const uint8_t*>(&value);
  m_inner.hash(Ditto::span{ptr, sizeof(value)});
}

void SimpleHasher::hash(std::int16_t value) {
  const Ditto::NonNullPtr ptr = reinterpret_cast<const uint8_t*>(&value);
  m_inner.hash(Ditto::span{ptr, sizeof(value)});
}

void SimpleHasher::hash(std::uint8_t value) {
  const Ditto::NonNullPtr ptr = reinterpret_cast<const uint8_t*>(&value);
  m_inner.hash(Ditto::span{ptr, sizeof(value)});
}

void SimpleHasher::hash(std::int8_t value) {
  const Ditto::NonNullPtr ptr = reinterpret_cast<const uint8_t*>(&value);
  m_inner.hash(Ditto::span{ptr, sizeof(value)});
}

void SimpleHasher::hash(const char* value) {
  const auto length = strlen(value);
  const Ditto::NonNullPtr ptr = reinterpret_cast<const uint8_t*>(&value);
  m_inner.hash(Ditto::span{ptr, length});
}

auto SimpleHasher::finish() -> std::uint32_t { return m_inner.finish(); }

}  // namespace Ditto
