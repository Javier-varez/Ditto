#ifndef DITTO_OPTIONAL_H_
#define DITTO_OPTIONAL_H_

#if defined(USE_STD_TEMPLATES) && defined(__has_include) && \
    __has_include(<optional>) && defined(__cplusplus) && __cplusplus >= 201703L
#include <optional>
#include <utility>

namespace Ditto {
using std::make_optional;
using std::optional;
}  // namespace Ditto

#else  // std::optional not available, define our own

#include <new>
#include <type_traits>
#include <utility>

#include "ditto/assert.h"

namespace Ditto {

template <class T>
class [[nodiscard]] optional {
 public:
  constexpr optional() noexcept : m_valid(false), m_dummy(false) {}

  constexpr optional(const optional& other) noexcept
      : m_valid(false), m_dummy(false) {
    if (other.has_value()) {
      m_member = T{other.m_member};
      m_valid = true;
    }
  }
  constexpr optional(optional && other) noexcept
      : m_valid(false), m_dummy(false) {
    if (other.has_value()) {
      m_member = T{std::move(other).value()};
      m_valid = true;
    }
  }

  template <class U>
  constexpr explicit optional(const optional<U>& other)
      : m_valid(false), m_dummy(false) {
    if (other.has_value()) {
      m_valid = true;
      m_member = T{other.value()};
    }
  }

  template <class U>
  constexpr explicit optional(optional<U> && other)
      : m_valid(false), m_dummy(false) {
    if (other.has_value()) {
      m_valid = true;
      m_member = T{std::move(other).value()};
    }
  }

  template <class... Args>
  constexpr explicit optional(Args... args)
      : m_valid(true), m_member(std::forward<Args>(args)...) {}

  template <class U = T>
  constexpr explicit optional(U && value)
      : m_valid(true), m_member(std::forward<U>(value)) {}

  constexpr optional& operator=(const optional& other) noexcept {
    if (this != &other) {
      reset();
      if (other.has_value()) {
        new (&m_dummy) T{other.m_member};
        m_valid = true;
      }
    }
    return *this;
  }

  constexpr optional& operator=(optional&& other) noexcept {
    if (this != &other) {
      reset();
      if (other.has_value()) {
        new (&m_dummy) T{std::move(other).m_member};
        m_valid = true;
      }
    }
    return *this;
  }

  template <class U = T,
            typename std::enable_if<std::is_convertible<U, T>::value,
                                    bool>::type = false>
  constexpr optional& operator=(U&& value) noexcept {
    reset();
    m_member = T{value};
    m_valid = true;
    return *this;
  }

  [[nodiscard]] constexpr bool has_value() const noexcept { return m_valid; }
  [[nodiscard]] constexpr operator bool() const noexcept { return m_valid; }

  [[nodiscard]] constexpr T value() const& noexcept {
    DITTO_VERIFY(m_valid);
    return m_member;
  }

  [[nodiscard]] constexpr T&& value()&& noexcept {
    DITTO_VERIFY(m_valid);
    return std::move(m_member);
  }

  template <class U>
  [[nodiscard]] constexpr T value_or(U && def) const& {
    if (m_valid) {
      return m_member;
    }
    return std::forward<U>(def);
  }

  template <class U>
  [[nodiscard]] constexpr T value_or(U && def)&& {
    if (m_valid) {
      return std::move(m_member);
    }
    return std::forward<U>(def);
  }

  [[nodiscard]] constexpr const T* operator->() const noexcept {
    if (!m_valid) {
      return nullptr;
    }
    return &m_member;
  }

  [[nodiscard]] constexpr T* operator->() noexcept {
    if (!m_valid) {
      return nullptr;
    }
    return &m_member;
  }

  [[nodiscard]] constexpr const T& operator*() const& noexcept {
    return m_member;
  }

  [[nodiscard]] constexpr T& operator*()& noexcept { return m_member; }

  [[nodiscard]] constexpr const T&& operator*() const&& noexcept {
    return std::move(m_member);
  }

  [[nodiscard]] constexpr T&& operator*()&& noexcept {
    return std::move(m_member);
  }

  constexpr void reset() {
    if (has_value()) {
      m_member.~T();
      m_valid = false;
    }
  }

  constexpr ~optional() { reset(); }

 private:
  union {
    T m_member;
    bool m_dummy;  // Just to make the union happy and make sure that the
                   // lifetime of this union is explicitly controlled.
  };
  bool m_valid{false};
};  // namespace Ditto

}  // namespace Ditto

#endif

#endif  // DITTO_OPTIONAL_H_
