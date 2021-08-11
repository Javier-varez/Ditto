
#pragma once

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <type_traits>

#include "ditto/type_traits.h"

namespace Ditto {

template <class T, std::uint32_t FRAC>
class FixedPoint {
 public:
  constexpr FixedPoint() = default;
  constexpr explicit FixedPoint(T value) : m_underlying(value) {}

  template <class U, std::uint32_t OTHER_FRAC>
  constexpr explicit FixedPoint(FixedPoint<U, OTHER_FRAC> other) {
    if constexpr (FRAC > OTHER_FRAC) {
      m_underlying = other.m_underlying << (FRAC - OTHER_FRAC);
    } else {
      m_underlying = other.m_underlying >> (OTHER_FRAC - FRAC);
    }
  }

  explicit operator double() {
    return static_cast<double>(m_underlying) / powl(2, FRAC);
  }

  constexpr FixedPoint<decltype(T{} + T{}), FRAC> operator+(FixedPoint other) {
    return FixedPoint<decltype(T{} + T{}), FRAC>{
        decltype(T{} + T{}){m_underlying} + other.m_underlying};
  }

  constexpr FixedPoint<decltype(T{} - T{}), FRAC> operator-(FixedPoint other) {
    return FixedPoint<decltype(T{} - T{}), FRAC>{
        decltype(T{} - T{}){m_underlying} - other.m_underlying};
  }

  template <class U, std::uint32_t OTHER_FRAC>
  constexpr FixedPoint<decltype(T{} * U{}), FRAC + OTHER_FRAC> operator*(
      FixedPoint<U, OTHER_FRAC> other) {
    return FixedPoint<decltype(U{} * T{}), FRAC + OTHER_FRAC>{
        decltype(T{} * T{}){m_underlying} * other.m_underlying};
  }

  template <class U, std::uint32_t OTHER_FRAC>
  constexpr FixedPoint<decltype(T{} / U{}), FRAC - OTHER_FRAC> operator/(
      FixedPoint<U, OTHER_FRAC> other) {
    return FixedPoint<decltype(T{} / U{}), FRAC - OTHER_FRAC>{
        m_underlying / other.m_underlying};
  }

  using primitive_type = T;
  constexpr static std::uint32_t fraction_bits = FRAC;

 private:
  T m_underlying{};

  template <class U, std::uint32_t OTHER_FRAC>
  friend class FixedPoint;

  static_assert(std::is_integral_v<T>);
};

template <std::uint32_t FRAC>
using FP8 = FixedPoint<std::uint8_t, FRAC>;

template <std::uint32_t FRAC>
using FP16 = FixedPoint<std::uint16_t, FRAC>;

template <std::uint32_t FRAC>
using FP32 = FixedPoint<std::uint32_t, FRAC>;

namespace numeric_literals {

constexpr FP8<8> operator"" _uq0_8(long double val) {
  auto int_val = static_cast<std::uint8_t>(val * 256.0);
  return FP8<8>{int_val};
}

constexpr FP16<16> operator"" _uq0_16(long double val) {
  auto int_val = static_cast<std::uint16_t>(val * 65536.0);
  return FP16<16>{int_val};
}

constexpr FP32<32> operator"" _uq0_32(long double val) {
  auto int_val = static_cast<std::uint32_t>(val * 4294967296.0);
  return FP32<32>{int_val};
}

}  // namespace numeric_literals

}  // namespace Ditto
