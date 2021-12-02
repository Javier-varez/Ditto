#ifndef DITTO_FIXED_H_
#define DITTO_FIXED_H_

#include <cmath>
#include <cstdint>
#include <type_traits>

namespace Ditto {

template <class T, std::uint32_t FRAC>
class FixedPoint {
 public:
  /// Constructors and creation static functions
  constexpr FixedPoint() = default;

  template <class U,
            std::enable_if_t<std::is_floating_point_v<U>, bool> = false>
  constexpr static auto from_floating_point(U value) -> FixedPoint {
    return FixedPoint{static_cast<T>(value * double{1ULL << FRAC})};
  }

  constexpr static auto from_integer(T value) -> FixedPoint {
    return FixedPoint{value << FRAC};
  }

  template <class U,
            std::enable_if_t<std::is_integral_v<U> && std::is_unsigned_v<U>,
                             bool> = false>
  constexpr static auto from_raw(U underlying) -> FixedPoint {
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

  template <class U>
  using add_underlying = std::make_unsigned_t<decltype(T{} + U{})>;
  template <class U>
  using sub_underlying = std::make_unsigned_t<decltype(T{} - U{})>;
  template <class U>
  using mul_underlying = std::make_unsigned_t<decltype(T{} * U{})>;
  template <class U>
  using div_underlying = std::make_unsigned_t<decltype(T{} / U{})>;

  template <class U>
  using add_result = FixedPoint<add_underlying<U>, FRAC>;
  template <class U>
  using sub_result = FixedPoint<sub_underlying<U>, FRAC>;
  template <class U, std::uint32_t OTHER_FRAC>
  using mul_result = FixedPoint<mul_underlying<U>, FRAC + OTHER_FRAC>;
  template <class U, std::uint32_t OTHER_FRAC>
  using div_result = FixedPoint<div_underlying<U>, FRAC - OTHER_FRAC>;

  // Operators

  [[nodiscard]] constexpr inline auto operator+(FixedPoint other)
      -> add_result<T> {
    return add_result<T>{add_underlying<T>{m_underlying} + other.m_underlying};
  }

  template <class U, std::enable_if_t<std::is_integral_v<U>, bool> = false>
  [[nodiscard]] constexpr inline auto operator+(U value) -> add_result<U> {
    return add_result<U>{add_underlying<U>{m_underlying} +
                         (static_cast<add_underlying<U>>(value) << FRAC)};
  }

  constexpr inline auto operator+=(FixedPoint other) -> FixedPoint& {
    m_underlying += other.m_underlying;
    return *this;
  }

  constexpr inline auto operator+=(T value) -> FixedPoint& {
    m_underlying += value << FRAC;
    return *this;
  }

  [[nodiscard]] constexpr inline auto operator-(FixedPoint other)
      -> sub_result<T> {
    return sub_result<T>{sub_underlying<T>{m_underlying} - other.m_underlying};
  }

  template <class U, std::enable_if_t<std::is_integral_v<U>, bool> = false>
  [[nodiscard]] constexpr inline auto operator-(U value) -> sub_result<U> {
    return sub_result<U>{sub_underlying<U>{m_underlying} -
                         (static_cast<sub_underlying<U>>(value) << FRAC)};
  }

  constexpr inline auto operator-=(FixedPoint other) -> FixedPoint& {
    m_underlying -= other.m_underlying;
    return *this;
  }

  constexpr inline auto operator-=(T value) -> FixedPoint& {
    m_underlying -= value << FRAC;
    return *this;
  }

  template <class U, std::uint32_t OTHER_FRAC>
  [[nodiscard]] constexpr inline auto operator*(FixedPoint<U, OTHER_FRAC> other)
      -> mul_result<U, OTHER_FRAC> {
    return mul_result<U, OTHER_FRAC>{
        static_cast<mul_underlying<U>>(m_underlying) * other.m_underlying};
  }

  template <class U, std::enable_if_t<std::is_integral_v<U>, bool> = false>
  [[nodiscard]] constexpr inline auto operator*(U value) -> mul_result<U, 0> {
    return mul_result<U, 0>{m_underlying * value};
  }

  template <class U, std::uint32_t OTHER_FRAC>
  constexpr inline auto operator*=(FixedPoint<U, OTHER_FRAC> other)
      -> FixedPoint& {
    auto val = m_underlying * other.m_underlying;
    m_underlying = val >> OTHER_FRAC;
    return *this;
  }

  template <class U, std::enable_if_t<std::is_integral_v<U>, bool> = false>
  constexpr inline auto operator*=(U value) -> FixedPoint& {
    m_underlying *= value;
    return *this;
  }

  template <class U, std::uint32_t OTHER_FRAC>
  [[nodiscard]] constexpr inline auto operator/(FixedPoint<U, OTHER_FRAC> other)
      -> div_result<U, OTHER_FRAC> {
    return div_result<U, OTHER_FRAC>{m_underlying / other.m_underlying};
  }

  template <class U, std::enable_if_t<std::is_integral_v<U>, bool> = false>
  [[nodiscard]] constexpr inline auto operator/(U value) -> div_result<U, 0> {
    return div_result<U, 0>{m_underlying / value};
  }

  template <class U, std::uint32_t OTHER_FRAC>
  constexpr inline auto operator/=(FixedPoint<U, OTHER_FRAC> other)
      -> FixedPoint& {
    auto div = m_underlying / other.m_underlying;
    m_underlying = div << OTHER_FRAC;
    return *this;
  }

  template <class U, std::enable_if_t<std::is_integral_v<U>, bool> = false>
  constexpr inline auto operator/=(U value) -> FixedPoint& {
    auto div = m_underlying / value;
    m_underlying = div;
    return *this;
  }

  // Comparison operators
  [[nodiscard]] constexpr inline auto operator==(FixedPoint other) -> bool {
    return m_underlying == other.m_underlying;
  }

  [[nodiscard]] constexpr inline auto operator!=(FixedPoint other) -> bool {
    return m_underlying != other.m_underlying;
  }

  [[nodiscard]] constexpr inline auto operator>=(FixedPoint other) -> bool {
    return m_underlying >= other.m_underlying;
  }

  [[nodiscard]] constexpr inline auto operator>(FixedPoint other) -> bool {
    return m_underlying > other.m_underlying;
  }

  [[nodiscard]] constexpr inline auto operator<=(FixedPoint other) -> bool {
    return m_underlying <= other.m_underlying;
  }

  [[nodiscard]] constexpr inline auto operator<(FixedPoint other) -> bool {
    return m_underlying < other.m_underlying;
  }

  template <class U, std::enable_if_t<std::is_integral_v<U>, bool> = false>
  [[nodiscard]] constexpr inline auto operator==(U value) -> bool {
    return m_underlying == (static_cast<uint64_t>(value) << FRAC);
  }

  template <class U, std::enable_if_t<std::is_integral_v<U>, bool> = false>
  [[nodiscard]] constexpr inline auto operator!=(U value) -> bool {
    return m_underlying != (static_cast<uint64_t>(value) << FRAC);
  }

  template <class U, std::enable_if_t<std::is_integral_v<U>, bool> = false>
  [[nodiscard]] constexpr inline auto operator>=(U value) -> bool {
    return m_underlying >= (static_cast<uint64_t>(value) << FRAC);
  }

  template <class U, std::enable_if_t<std::is_integral_v<U>, bool> = false>
  [[nodiscard]] constexpr inline auto operator>(U value) -> bool {
    return m_underlying > (static_cast<uint64_t>(value) << FRAC);
  }

  template <class U, std::enable_if_t<std::is_integral_v<U>, bool> = false>
  [[nodiscard]] constexpr inline auto operator<=(U value) -> bool {
    return m_underlying <= (static_cast<uint64_t>(value) << FRAC);
  }

  template <class U, std::enable_if_t<std::is_integral_v<U>, bool> = false>
  [[nodiscard]] constexpr inline auto operator<(U value) -> bool {
    return m_underlying < (static_cast<uint64_t>(value) << FRAC);
  }

  [[nodiscard]] constexpr inline auto round() const -> FixedPoint {
    constexpr T half_unit = T{1} << (FRAC - 1);
    constexpr T INT_MASK = ~((T{1} << FRAC) - 1);
    return FixedPoint{(m_underlying + half_unit) & INT_MASK};
  }

  // Accessors and conversions
  [[nodiscard]] constexpr inline auto as_raw() const -> T {
    return m_underlying;
  }
  [[nodiscard]] constexpr inline auto as_integer() const -> T {
    return m_underlying >> FRAC;
  }
  [[nodiscard]] constexpr inline auto as_double() const -> double {
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

template <class T, uint32_t FRAC, class U>
constexpr inline auto operator+(U val, FixedPoint<T, FRAC> fp)
    -> FixedPoint<T, FRAC> {
  return fp + val;
}

template <class T, uint32_t FRAC, class U>
constexpr inline auto operator-(U val, FixedPoint<T, FRAC> fp)
    -> FixedPoint<T, FRAC> {
  return fp - val;
}

template <class T, uint32_t FRAC, class U>
constexpr inline auto operator*(U val, FixedPoint<T, FRAC> fp)
    -> FixedPoint<T, FRAC> {
  return fp * val;
}

template <class T, uint32_t FRAC, class U>
constexpr inline auto operator/(U val, FixedPoint<T, FRAC> fp)
    -> FixedPoint<T, FRAC> {
  return fp / val;
}

namespace numeric_literals {

constexpr auto operator"" _uq0_8(long double val) -> FP8<8> {
  return FP8<8>::from_floating_point(val);
}
constexpr auto operator"" _uq4_4(long double val) -> FP8<4> {
  return FP8<4>::from_floating_point(val);
}

constexpr auto operator"" _uq0_16(long double val) -> FP16<16> {
  return FP16<16>::from_floating_point(val);
}
constexpr auto operator"" _uq8_8(long double val) -> FP16<8> {
  return FP16<8>::from_floating_point(val);
}

constexpr auto operator"" _uq0_32(long double val) -> FP32<32> {
  return FP32<32>::from_floating_point(val);
}
constexpr auto operator"" _uq16_16(long double val) -> FP32<16> {
  return FP32<16>::from_floating_point(val);
}

}  // namespace numeric_literals

}  // namespace Ditto

#endif  // DITTO_FIXED_H_
