
#ifndef DITTO_CRC32C_H_
#define DITTO_CRC32C_H_

#include <cstdint>

#include "ditto/span.h"

constexpr std::uint32_t crc32c(Ditto::span<const std::uint8_t> data);

class Crc32cCalculator {
 public:
  constexpr Crc32cCalculator() = default;

  void hash(Ditto::span<const std::uint8_t> data);
  void hash(int value);
  std::uint32_t finish();

 private:
  std::uint32_t m_current_value = 0xFFFFFFFF;
};

#endif  // DITTO_CRC32C_H_
