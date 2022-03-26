#ifndef DITTO_HASH_H_
#define DITTO_HASH_H_

#include <cstddef>
#include <cstdint>
#include <type_traits>

#include "ditto/crc32c.h"

namespace Ditto {

template <class T>
concept Hasher = requires(T hasher) {
  // Must be default constructible
  T{};

  // Must have a finish method
  std::uint32_t{hasher.finish()};

  // Must be able to hash common types
  hasher.hash(std::int64_t{});
  hasher.hash(std::uint64_t{});
  hasher.hash(std::int32_t{});
  hasher.hash(std::uint32_t{});
  hasher.hash(std::int16_t{});
  hasher.hash(std::uint16_t{});
  hasher.hash(std::int8_t{});
  hasher.hash(std::uint8_t{});
};

template <class H, class T>
concept Hashable = requires(H hasher, T hashable) {
  // H must be a hasher
  requires Hasher<H>;

  // The hasher must support the key
  hasher.hash(hashable);
};

// CRC32 Hasher accepts common types
class SimpleHasher {
 public:
  SimpleHasher() = default;

  void hash(std::uint64_t value);
  void hash(std::int64_t value);
  void hash(std::uint32_t value);
  void hash(std::int32_t value);
  void hash(std::uint16_t value);
  void hash(std::int16_t value);
  void hash(std::uint8_t value);
  void hash(std::int8_t value);
  void hash(const char* value);

  auto finish() -> std::uint32_t;

 private:
  Crc32cCalculator m_inner;
};

}  // namespace Ditto

#endif  // DITTO_HASH_H_
