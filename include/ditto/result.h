#ifndef DITTO_RESULT_H_
#define DITTO_RESULT_H_

#include <algorithm>
#include <cstddef>
#include <new>
#include <type_traits>
#include <utility>

#include "ditto/assert.h"

namespace Ditto {

template <class Ok, class Err>
class [[nodiscard]] Result {
 public:
  // Implicit construction from an object of type Ok
  Result(Ok val) {
    new (&m_memory_buffer) Ok{std::move(val)};
    m_is_ok = true;
  }

  Result(Err val) {
    new (&m_memory_buffer) Err{std::move(val)};
    m_is_ok = false;
  }

  /// Construct an Ok object with perfect forwarding
  template <class... Args>
  [[nodiscard]] static Result ok(Args... args) {
    Result result;
    new (&result.m_memory_buffer) Ok{std::forward<Args>(args)...};
    result.m_is_ok = true;
    return result;
  }

  /// Construct an Err object with perfect forwarding
  template <class... Args>
  [[nodiscard]] static Result error(Args... args) {
    Result result;
    new (&result.m_memory_buffer) Err{std::forward<Args>(args)...};
    result.m_is_ok = false;
    return result;
  }

  [[nodiscard]] bool is_error() const { return !m_is_ok; }
  [[nodiscard]] bool is_ok() const { return m_is_ok; }

  [[nodiscard]] Ok& ok_value()& {
    DITTO_VERIFY(m_is_ok);
    return *reinterpret_cast<Ok*>(&m_memory_buffer);
  }

  [[nodiscard]] Ok&& ok_value()&& {
    DITTO_VERIFY(m_is_ok);
    return std::move(*reinterpret_cast<Ok*>(&m_memory_buffer));
  }

  [[nodiscard]] Err& error_value()& {
    DITTO_VERIFY(!m_is_ok);
    return *reinterpret_cast<Err*>(&m_memory_buffer);
  }

  [[nodiscard]] Err&& error_value()&& {
    DITTO_VERIFY(!m_is_ok);
    return std::move(*reinterpret_cast<Err*>(&m_memory_buffer));
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
  Result(Result &&) = default;
  Result& operator=(const Result&) = default;
  Result& operator=(Result&&) = default;

 private:
  constexpr static std::size_t ALIGNMENT = std::max(alignof(Ok), alignof(Err));
  constexpr static std::size_t BUFFER_SIZE = std::max(sizeof(Ok), sizeof(Err));
  bool m_is_ok = false;
  typename std::aligned_storage<BUFFER_SIZE, ALIGNMENT>::type m_memory_buffer;

  static_assert(
      !std::is_same_v<Ok, Err>,
      "Cannot create a result where the Ok type and Err types are the same");

  Result() = default;
};

template <class Ok>
class [[nodiscard]] Result<Ok, void> {
 public:
  // Implicit construction from an object of type Ok
  Result(Ok val) {
    new (&m_memory_buffer) Ok{std::move(val)};
    m_is_ok = true;
  }

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

  [[nodiscard]] Ok& ok_value()& {
    DITTO_VERIFY(m_is_ok);
    return *reinterpret_cast<Ok*>(&m_memory_buffer);
  }

  [[nodiscard]] Ok&& ok_value()&& {
    DITTO_VERIFY(m_is_ok);
    return std::move(*reinterpret_cast<Ok*>(&m_memory_buffer));
  }

  ~Result() {
    if (m_is_ok) {
      auto* ok = reinterpret_cast<Ok*>(&m_memory_buffer);
      ok->~Ok();
    }
  }

  Result(const Result&) = default;
  Result(Result &&) = default;
  Result& operator=(const Result&) = default;
  Result& operator=(Result&&) = default;

 private:
  constexpr static std::size_t ALIGNMENT = alignof(Ok);
  constexpr static std::size_t BUFFER_SIZE = sizeof(Ok);
  bool m_is_ok = false;
  typename std::aligned_storage<BUFFER_SIZE, ALIGNMENT>::type m_memory_buffer;

  static_assert(
      !std::is_same_v<Ok, void>,
      "Cannot create a result where the Ok type and Err types are the same");

  Result() = default;
};

template <class Err>
class [[nodiscard]] Result<void, Err> {
 public:
  // Implicit construction from an object of type Err
  Result(Err val) {
    new (&m_memory_buffer) Err{std::move(val)};
    m_is_ok = false;
  }

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

  [[nodiscard]] Err& error_value()& {
    DITTO_VERIFY(!m_is_ok);
    return *reinterpret_cast<Err*>(&m_memory_buffer);
  }

  [[nodiscard]] Err&& error_value()&& {
    DITTO_VERIFY(!m_is_ok);
    return std::move(*reinterpret_cast<Err*>(&m_memory_buffer));
  }

  ~Result() {
    if (!m_is_ok) {
      auto* err = reinterpret_cast<Err*>(&m_memory_buffer);
      err->~Err();
    }
  }

  Result(const Result&) = default;
  Result(Result &&) = default;
  Result& operator=(const Result&) = default;
  Result& operator=(Result&&) = default;

 private:
  constexpr static std::size_t ALIGNMENT = alignof(Err);
  constexpr static std::size_t BUFFER_SIZE = sizeof(Err);
  bool m_is_ok = false;
  typename std::aligned_storage<BUFFER_SIZE, ALIGNMENT>::type m_memory_buffer;

  static_assert(
      !std::is_same_v<Err, void>,
      "Cannot create a result where the Ok type and Err types are the same");

  Result() = default;
};

#define DITTO_VERIFY_OK(expression)          \
  {                                          \
    auto _evaluated_result = expression;     \
    DITTO_VERIFY(_evaluated_result.is_ok()); \
  }

#define DITTO_VERIFY_ERR(expression)            \
  {                                             \
    auto _evaluated_result = expression;        \
    DITTO_VERIFY(_evaluated_result.is_error()); \
  }

#define DITTO_PROPAGATE(expression)            \
  ({                                           \
    auto _result = expression;                 \
    if (_result.is_error()) {                  \
      /* This forces a move of the value*/     \
      return std::move(_result).error_value(); \
    }                                          \
    /* This forces a move of the value*/       \
    std::move(_result).ok_value();             \
  })

#define DITTO_UNWRAP(expression)   \
  ({                               \
    auto _result = expression;     \
    std::move(_result).ok_value(); \
  })

#define DITTO_UNWRAP_OR(expression, alternative) \
  ({                                             \
    auto _result = expression;                   \
    auto unwrap_or = [&]() {                     \
      if (_result.is_ok()) {                     \
        return std::move(_result).ok_value();    \
      } else {                                   \
        return alternative;                      \
      }                                          \
    };                                           \
    unwrap_or();                                 \
  })

#define DITTO_UNWRAP_OR_ELSE(expression, functor) \
  ({                                              \
    auto _result = expression;                    \
    auto unwrap_or = [&]() {                      \
      if (_result.is_ok()) {                      \
        return std::move(_result).ok_value();     \
      } else {                                    \
        return functor();                         \
      }                                           \
    };                                            \
    unwrap_or();                                  \
  })

#define DITTO_UNWRAP_ERR(expression)  \
  ({                                  \
    auto _result = expression;        \
    std::move(_result).error_value(); \
  })

#define DITTO_UNWRAP_ERR_OR(expression, alternative) \
  ({                                                 \
    auto _result = expression;                       \
    auto unwrap_or = [&]() {                         \
      if (_result.is_error()) {                      \
        return std::move(_result).error_value();     \
      } else {                                       \
        return alternative;                          \
      }                                              \
    };                                               \
    unwrap_or();                                     \
  })

#define DITTO_UNWRAP_ERR_OR_ELSE(expression, functor) \
  ({                                                  \
    auto _result = expression;                        \
    auto unwrap_or = [&]() {                          \
      if (_result.is_error()) {                       \
        return std::move(_result).error_value();      \
      } else {                                        \
        return functor();                             \
      }                                               \
    };                                                \
    unwrap_or();                                      \
  })

}  // namespace Ditto

#endif  // DITTO_RESULT_H_
