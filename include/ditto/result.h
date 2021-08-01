
#pragma once

#include <algorithm>
#include <cstddef>
#include <new>
#include <type_traits>
#include <utility>

#include "ditto/assert.h"

namespace Ditto {

template <class Ok, class Err>
class Result {
 public:
  template <class... Args>
  [[nodiscard]] static Result ok(Args... args) {
    Result result;
    new (&result.m_memory_buffer) Ok{std::forward<Args>(args)...};
    result.m_is_ok = true;
    return result;
  }

  template <class... Args>
  [[nodiscard]] static Result error(Args... args) {
    Result result;
    new (&result.m_memory_buffer) Err{std::forward<Args>(args)...};
    result.m_is_ok = false;
    return result;
  }

  [[nodiscard]] bool is_error() const { return !m_is_ok; }
  [[nodiscard]] bool is_ok() const { return m_is_ok; }

  [[nodiscard]] Ok& ok_value() & {
    DITTO_VERIFY(m_is_ok);
    return *reinterpret_cast<Ok*>(&m_memory_buffer);
  }

  [[nodiscard]] Ok&& ok_value() && {
    DITTO_VERIFY(m_is_ok);
    return *reinterpret_cast<Ok*>(&m_memory_buffer);
  }

  [[nodiscard]] Err& error_value() & {
    DITTO_VERIFY(!m_is_ok);
    return *reinterpret_cast<Err*>(&m_memory_buffer);
  }

  [[nodiscard]] Err&& error_value() && {
    DITTO_VERIFY(!m_is_ok);
    return *reinterpret_cast<Err*>(&m_memory_buffer);
  }

  ~Result() {
    if (m_is_ok) {
      auto* ok = reinterpret_cast<Ok*>(&m_memory_buffer);
      ok->~Ok();
    } else {
      auto* err = reinterpret_cast<Err*>(&m_memory_buffer);
      err->~Err();
    }
  }

  Result(const Result&) = default;
  Result(Result&&) = default;
  Result& operator=(const Result&) = default;
  Result& operator=(Result&&) = default;

 private:
  constexpr static std::size_t ALIGNMENT = std::max(alignof(Ok), alignof(Err));
  constexpr static std::size_t BUFFER_SIZE = std::max(sizeof(Ok), sizeof(Err));
  bool m_is_ok = false;
  typename std::aligned_storage<ALIGNMENT, BUFFER_SIZE>::type m_memory_buffer;

  Result() = default;
};

template <class Ok>
class Result<Ok, void> {
 public:
  template <class... Args>
  [[nodiscard]] static Result ok(Args... args) {
    auto result = Result{};
    new (&result.m_memory_buffer) Ok{std::forward<Args>(args)...};
    result.m_is_ok = true;
    return result;
  }

  [[nodiscard]] static Result error() { return Result{}; }

  [[nodiscard]] bool is_error() const { return !m_is_ok; }
  [[nodiscard]] bool is_ok() const { return m_is_ok; }

  [[nodiscard]] Ok& ok_value() & {
    DITTO_VERIFY(m_is_ok);
    return *reinterpret_cast<Ok*>(&m_memory_buffer);
  }

  [[nodiscard]] Ok&& ok_value() && {
    DITTO_VERIFY(m_is_ok);
    return *reinterpret_cast<Ok*>(&m_memory_buffer);
  }

  ~Result() {
    if (m_is_ok) {
      auto* ok = reinterpret_cast<Ok*>(&m_memory_buffer);
      ok->~Ok();
    }
  }

  Result(const Result&) = default;
  Result(Result&&) = default;
  Result& operator=(const Result&) = default;
  Result& operator=(Result&&) = default;

 private:
  constexpr static std::size_t ALIGNMENT = alignof(Ok);
  constexpr static std::size_t BUFFER_SIZE = sizeof(Ok);
  bool m_is_ok = false;
  typename std::aligned_storage<ALIGNMENT, BUFFER_SIZE>::type m_memory_buffer;

  Result() = default;
};

template <class Err>
class Result<void, Err> {
 public:
  [[nodiscard]] static Result ok() {
    Result result;
    result.m_is_ok = true;
    return result;
  }

  template <class... Args>
  [[nodiscard]] static Result error(Args... args) {
    Result result;
    new (&result.m_memory_buffer) Err{std::forward<Args>(args)...};
    result.m_is_ok = false;
    return result;
  }

  [[nodiscard]] bool is_error() const { return !m_is_ok; }
  [[nodiscard]] bool is_ok() const { return m_is_ok; }

  [[nodiscard]] Err& error_value() & {
    DITTO_VERIFY(!m_is_ok);
    return *reinterpret_cast<Err*>(&m_memory_buffer);
  }

  [[nodiscard]] Err&& error_value() && {
    DITTO_VERIFY(!m_is_ok);
    return *reinterpret_cast<Err*>(&m_memory_buffer);
  }

  ~Result() {
    if (!m_is_ok) {
      auto* err = reinterpret_cast<Err*>(&m_memory_buffer);
      err->~Err();
    }
  }

  Result(const Result&) = default;
  Result(Result&&) = default;
  Result& operator=(const Result&) = default;
  Result& operator=(Result&&) = default;

 private:
  constexpr static std::size_t ALIGNMENT = alignof(Err);
  constexpr static std::size_t BUFFER_SIZE = sizeof(Err);
  bool m_is_ok = false;
  typename std::aligned_storage<ALIGNMENT, BUFFER_SIZE>::type m_memory_buffer;

  Result() = default;
};

}  // namespace Ditto
