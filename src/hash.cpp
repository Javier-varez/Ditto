
#include "ditto/hash.h"

namespace {

auto calculate_integer_hash(std::uint64_t key) -> std::size_t {
  uint64_t hash = key;
  hash ^= 0xDEADC0DE;
  hash ^= key << 13;
  hash += key >> 1;
  hash += key << 22;
  hash ^= key << 3;
  hash ^= ~(key << 2);
  hash += key << 8;
  hash += key >> 20;
  hash ^= key << 7;
  return hash;
}

}  // namespace

namespace Ditto::Hash {

// These are random hashing functions, I should invest some more time into
// improving them.
//
auto calculate(std::uint64_t key) -> std::size_t {
  return calculate_integer_hash(key);
}
auto calculate(std::uint32_t key) -> std::size_t {
  return calculate_integer_hash(key);
}
auto calculate(std::uint16_t key) -> std::size_t {
  return calculate_integer_hash(key);
}
auto calculate(std::uint8_t key) -> std::size_t {
  return calculate_integer_hash(key);
}
auto calculate(std::int64_t key) -> std::size_t {
  return calculate_integer_hash(key);
}
auto calculate(std::int32_t key) -> std::size_t {
  return calculate_integer_hash(key);
}
auto calculate(std::int16_t key) -> std::size_t {
  return calculate_integer_hash(key);
}
auto calculate(std::int8_t key) -> std::size_t {
  return calculate_integer_hash(key);
}

}  // namespace Ditto::Hash
