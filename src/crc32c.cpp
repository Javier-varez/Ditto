
#include "ditto/crc32c.h"

#include "ditto/non_null_ptr.h"

constexpr auto generate_coefficient(std::uint8_t byte) -> std::uint32_t {
  constexpr std::uint32_t POLYNOMIAL = 0x82F63B78;  // CRC32C
  std::uint32_t value = byte;

  for (std::uint32_t i = 0; i < 8; i++) {
    if ((0x1 & value) != 0) {
      value >>= 1;
      value ^= POLYNOMIAL;
    } else {
      value >>= 1;
    }
  }
  return value;
}

constexpr auto calculate_crc32c_table() noexcept
    -> std::array<std::uint32_t, 256> {
  std::array<std::uint32_t, 256> table{};
  for (std::uint32_t i = 0; i < table.size(); i++) {
    table[i] = generate_coefficient(i);
  }
  return table;
}

static auto CRC32C_TABLE = calculate_crc32c_table();

constexpr std::uint32_t crc32c(Ditto::span<const std::uint8_t> data) {
  std::uint32_t value = 0xFFFFFFFF;
  for (const auto byte : data) {
    const std::uint32_t index = byte ^ (value & 0xff);
    value = (value >> 8) ^ CRC32C_TABLE[index];
  }
  return value ^ 0xFFFFFFFF;
}

void Crc32cCalculator::hash(Ditto::span<const std::uint8_t> data) {
  for (const auto byte : data) {
    const std::uint32_t index = byte ^ (m_current_value & 0xff);
    m_current_value = (m_current_value >> 8) ^ CRC32C_TABLE[index];
  }
}

void Crc32cCalculator::hash(int value) {
  const Ditto::NonNullPtr ptr = reinterpret_cast<const uint8_t*>(&value);
  hash(Ditto::span{ptr, sizeof(int)});
}

std::uint32_t Crc32cCalculator::finish() {
  const auto crc = m_current_value ^ 0xFFFFFFFF;
  m_current_value = 0xFFFFFFFF;
  return crc;
}
