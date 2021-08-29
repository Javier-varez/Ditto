
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
  /// Constructors and creation static functions
  constexpr FixedPoint() = default;

  template <class U,
            std::enable_if_t<std::is_floating_point_v<U>, bool> = false>
  constexpr static FixedPoint from_floating_point(U value) {
    return FixedPoint{static_cast<T>(value * double{1ULL << FRAC})};
  }

  template <class U,
            std::enable_if_t<std::is_integral_v<U> && std::is_unsigned_v<U>,
                             bool> = false>
  constexpr static FixedPoint from_integer(U value) {
    return FixedPoint{value << FRAC};
  }

  template <class U,
            std::enable_if_t<std::is_integral_v<U> && std::is_unsigned_v<U>,
                             bool> = false>
  constexpr static FixedPoint from_raw(U underlying) {
    return FixedPoint{underlying};
  }

  template <class U, std::uint32_t OTHER_FRAC>
  constexpr explicit FixedPoint(FixedPoint<U, OTHER_FRAC> other) {
    if constexpr (FRAC > OTHER_FRAC) {
      m_underlying = other.m_underlying << (FRAC - OTHER_FRAC);
    } else {
      m_underlying = other.m_underlying >> (OTHER_FRAC - FRAC);
    }
  }

  // Types of the operations
  using primitive_type = T;
  constexpr static inline std::uint32_t fraction_bits = FRAC;

  using add_underlying = std::make_unsigned_t<decltype(T{} + T{})>;
  using sub_underlying = std::make_unsigned_t<decltype(T{} - T{})>;
  template <class U>
  using mul_underlying = std::make_unsigned_t<decltype(T{} * U{})>;
  template <class U>
  using div_underlying = std::make_unsigned_t<decltype(T{} / U{})>;

  using add_result = FixedPoint<add_underlying, FRAC>;
  using sub_result = FixedPoint<sub_underlying, FRAC>;
  template <class U, std::uint32_t OTHER_FRAC>
  using mul_result = FixedPoint<mul_underlying<U>, FRAC + OTHER_FRAC>;
  template <class U, std::uint32_t OTHER_FRAC>
  using div_result = FixedPoint<div_underlying<U>, FRAC - OTHER_FRAC>;

  // Operators

  constexpr inline add_result operator+(FixedPoint other) {
    return add_result{add_underlying{m_underlying} + other.m_underlying};
  }

  constexpr inline FixedPoint& operator+=(FixedPoint other) {
    m_underlying += other.m_underlying;
    return *this;
  }

  constexpr sub_result operator-(FixedPoint other) {
    return sub_result{sub_underlying{m_underlying} - other.m_underlying};
  }

  constexpr FixedPoint& operator-=(FixedPoint other) {
    m_underlying -= other.m_underlying;
    return *this;
  }

  template <class U, std::uint32_t OTHER_FRAC>
  constexpr mul_result<U, OTHER_FRAC> operator*(
      FixedPoint<U, OTHER_FRAC> other) {
    return mul_result<U, OTHER_FRAC>{mul_underlying<U>{m_underlying} *
                                     other.m_underlying};
  }

  template <class U, std::uint32_t OTHER_FRAC>
  constexpr div_result<U, OTHER_FRAC> operator/(
      FixedPoint<U, OTHER_FRAC> other) {
    return div_result<U, OTHER_FRAC>{div_underlying<U>{m_underlying} /
                                     other.m_underlying};
  }

  template <class U, std::enable_if_t<std::is_integral_v<U>, bool> = false>
  constexpr div_result<U, 0> operator/(U value) {
    return div_result<U, 0>{div_underlying<U>{m_underlying} / value};
  }

  // Accessors and conversions
  [[nodiscard]] constexpr T raw() const { return m_underlying; }

  [[nodiscard]] explicit constexpr operator double() {
    return static_cast<double>(m_underlying) / powl(2, FRAC);
  }

 private:
  T m_underlying{};

  template <class U, std::uint32_t OTHER_FRAC>
  friend class FixedPoint;

  // Constructor from underlying is private. Use from_* functions to construct
  // values
  constexpr explicit FixedPoint(T value) : m_underlying(value) {}

  static_assert(std::is_integral_v<T>);
};

template <std::uint32_t FRAC>
using FP8 = FixedPoint<std::uint8_t, FRAC>;

template <std::uint32_t FRAC>
using FP16 = FixedPoint<std::uint16_t, FRAC>;

template <std::uint32_t FRAC>
using FP32 = FixedPoint<std::uint32_t, FRAC>;

template <std::uint32_t FRAC>
using FP64 = FixedPoint<std::uint64_t, FRAC>;

namespace numeric_literals {

constexpr FP8<8> operator"" _uq0_8(long double val) {
  return FP8<8>::from_floating_point(val);
}
constexpr FP8<4> operator"" _uq4_4(long double val) {
  return FP8<4>::from_floating_point(val);
}

constexpr FP16<16> operator"" _uq0_16(long double val) {
  return FP16<16>::from_floating_point(val);
}
constexpr FP16<8> operator"" _uq8_8(long double val) {
  return FP16<8>::from_floating_point(val);
}

constexpr FP32<32> operator"" _uq0_32(long double val) {
  return FP32<32>::from_floating_point(val);
}
constexpr FP32<16> operator"" _uq16_16(long double val) {
  return FP32<16>::from_floating_point(val);
}

}  // namespace numeric_literals

}  // namespace Ditto
