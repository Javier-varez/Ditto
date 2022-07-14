#ifndef DITTO_RESULT_H_
#define DITTO_RESULT_H_

#include <algorithm>
#include <cstddef>
#include <new>
#include <type_traits>
#include <utility>

#include "ditto/assert.h"

namespace Ditto {

namespace detail {

template <class T>
struct [[nodiscard]] ErrSentinel {
  explicit ErrSentinel(T val) noexcept : value{std::move(val)} {}
  T value;
};

template <class T>
struct [[nodiscard]] OkSentinel {
  explicit OkSentinel(T val) noexcept : value{std::move(val)} {}
  T value;
};

}  // namespace detail

template <class Ok, class Err>
class [[nodiscard]] Result {
 public:
  // Constructors used for propagating errors or values
  Result(detail::OkSentinel<Ok> val) noexcept : m_is_ok{true} {
    new (&m_memory_buffer) Ok{std::move(val.value)};
  }

  Result(detail::ErrSentinel<Err> val) noexcept : m_is_ok{false} {
    new (&m_memory_buffer) Err{std::move(val.value)};
  }

  /// Construct an Ok object with perfect forwarding
  template <class... Args>
  [[nodiscard]] static Result ok(Args... args) noexcept {
    return Result{detail::OkSentinel<Ok>{std::forward<Args>(args)...}};
  }

  /// Construct an Err object with perfect forwarding
  template <class... Args>
  [[nodiscard]] static Result error(Args... args) noexcept {
    return Result{detail::ErrSentinel<Err>{std::forward<Args>(args)...}};
  }

  [[nodiscard]] bool is_error() const noexcept { return !m_is_ok; }
  [[nodiscard]] bool is_ok() const noexcept { return m_is_ok; }

  [[nodiscard]] const Ok& ok_value() const& noexcept {
    DITTO_VERIFY(m_is_ok);
    return *reinterpret_cast<const Ok*>(&m_memory_buffer);
  }

  [[nodiscard]] Ok& ok_value()& noexcept {
    DITTO_VERIFY(m_is_ok);
    return *reinterpret_cast<Ok*>(&m_memory_buffer);
  }

  [[nodiscard]] Ok&& ok_value()&& noexcept {
    DITTO_VERIFY(m_is_ok);
    return std::move(*reinterpret_cast<Ok*>(&m_memory_buffer));
  }

  [[nodiscard]] const Err& error_value() const& noexcept {
    DITTO_VERIFY(!m_is_ok);
    return *reinterpret_cast<const Err*>(&m_memory_buffer);
  }

  [[nodiscard]] Err& error_value()& noexcept {
    DITTO_VERIFY(!m_is_ok);
    return *reinterpret_cast<Err*>(&m_memory_buffer);
  }

  [[nodiscard]] Err&& error_value()&& noexcept {
    DITTO_VERIFY(!m_is_ok);
    return std::move(*reinterpret_cast<Err*>(&m_memory_buffer));
  }

  template <typename Callable>
  [[nodiscard]] auto map_ok(Callable callable) noexcept
      ->Result<decltype(std::declval<Callable>()(std::declval<Ok>())), Err> {
    if (!is_ok()) {
      return {detail::ErrSentinel<Err>{error_value()}};
    }
    return {detail::OkSentinel{callable(ok_value())}};
  }

  template <typename Callable>
  [[nodiscard]] auto map_err(Callable callable) noexcept
      ->Result<Ok, decltype(std::declval<Callable>()(std::declval<Err>()))> {
    if (!is_error()) {
      return {detail::OkSentinel<Ok>{ok_value()}};
    }
    return {detail::ErrSentinel{callable(error_value())}};
  }

  ~Result() noexcept {
    if (m_is_ok) {
      auto* ok = reinterpret_cast<Ok*>(&m_memory_buffer);
      ok->~Ok();
    } else {
      auto* err = reinterpret_cast<Err*>(&m_memory_buffer);
      err->~Err();
    }
  }

  Result(const Result&) noexcept = default;
  Result(Result &&) noexcept = default;

  Result& operator=(const Result&) noexcept = default;
  Result& operator=(Result&&) noexcept = default;

 private:
  constexpr static std::size_t ALIGNMENT = std::max(alignof(Ok), alignof(Err));
  constexpr static std::size_t BUFFER_SIZE = std::max(sizeof(Ok), sizeof(Err));
  bool m_is_ok = false;
  typename std::aligned_storage<BUFFER_SIZE, ALIGNMENT>::type m_memory_buffer;

  Result() noexcept = default;
};

template <class Ok>
class [[nodiscard]] Result<Ok, void> {
 public:
  // Constructors used for propagating errors or values
  Result(detail::OkSentinel<Ok> val) noexcept : m_is_ok{true} {
    new (&m_memory_buffer) Ok{std::move(val.value)};
  }

  template <class... Args>
  [[nodiscard]] static Result ok(Args... args) noexcept {
    return Result{detail::OkSentinel<Ok>{std::forward<Args>(args)...}};
  }

  [[nodiscard]] static Result error() noexcept { return Result{}; }

  [[nodiscard]] bool is_error() const noexcept { return !m_is_ok; }
  [[nodiscard]] bool is_ok() const noexcept { return m_is_ok; }

  [[nodiscard]] const Ok& ok_value() const& noexcept {
    DITTO_VERIFY(m_is_ok);
    return *reinterpret_cast<Ok*>(&m_memory_buffer);
  }

  [[nodiscard]] Ok& ok_value()& noexcept {
    DITTO_VERIFY(m_is_ok);
    return *reinterpret_cast<Ok*>(&m_memory_buffer);
  }

  [[nodiscard]] Ok&& ok_value()&& noexcept {
    DITTO_VERIFY(m_is_ok);
    return std::move(*reinterpret_cast<Ok*>(&m_memory_buffer));
  }

  ~Result() noexcept {
    if (m_is_ok) {
      auto* ok = reinterpret_cast<Ok*>(&m_memory_buffer);
      ok->~Ok();
    }
  }

  Result(const Result&) noexcept = default;
  Result(Result &&) noexcept = default;
  Result& operator=(const Result&) noexcept = default;
  Result& operator=(Result&&) noexcept = default;

  template <typename Callable>
  [[nodiscard]] auto map_ok(Callable callable) noexcept
      ->Result<decltype(std::declval<Callable>()(std::declval<Ok>())), void> {
    if (!is_ok()) {
      return Result<decltype(std::declval<Callable>()(std::declval<Ok>())),
                    void>::error();
    }
    return {detail::OkSentinel{callable(ok_value())}};
  }

 private:
  constexpr static std::size_t ALIGNMENT = alignof(Ok);
  constexpr static std::size_t BUFFER_SIZE = sizeof(Ok);
  bool m_is_ok = false;
  typename std::aligned_storage<BUFFER_SIZE, ALIGNMENT>::type m_memory_buffer;

  Result() = default;
};

template <class Err>
class [[nodiscard]] Result<void, Err> {
 public:
  // Implicit construction from an object of type Err
  Result(detail::ErrSentinel<Err> val) noexcept : m_is_ok{false} {
    new (&m_memory_buffer) Err{std::move(val.value)};
  }

  [[nodiscard]] static Result ok() noexcept { return Result{}; }

  template <class... Args>
  [[nodiscard]] static Result error(Args... args) noexcept {
    return Result{detail::ErrSentinel<Err>{std::forward<Args>(args)...}};
  }

  [[nodiscard]] bool is_error() const noexcept { return !m_is_ok; }
  [[nodiscard]] bool is_ok() const noexcept { return m_is_ok; }

  [[nodiscard]] const Err& error_value() const& noexcept {
    DITTO_VERIFY(!m_is_ok);
    return *reinterpret_cast<Err*>(&m_memory_buffer);
  }

  [[nodiscard]] Err& error_value()& noexcept {
    DITTO_VERIFY(!m_is_ok);
    return *reinterpret_cast<Err*>(&m_memory_buffer);
  }

  [[nodiscard]] Err&& error_value()&& noexcept {
    DITTO_VERIFY(!m_is_ok);
    return std::move(*reinterpret_cast<Err*>(&m_memory_buffer));
  }

  ~Result() noexcept {
    if (!m_is_ok) {
      auto* err = reinterpret_cast<Err*>(&m_memory_buffer);
      err->~Err();
    }
  }

  Result(const Result&) noexcept = default;
  Result(Result &&) noexcept = default;
  Result& operator=(const Result&) noexcept = default;
  Result& operator=(Result&&) noexcept = default;

  template <typename Callable>
  [[nodiscard]] auto map_err(Callable callable) noexcept
      ->Result<void, decltype(std::declval<Callable>()(std::declval<Err>()))> {
    if (!is_error()) {
      return Result<void, decltype(std::declval<Callable>()(
                              std::declval<Err>()))>::ok();
    }
    return {detail::ErrSentinel{callable(error_value())}};
  }

 private:
  constexpr static std::size_t ALIGNMENT = alignof(Err);
  constexpr static std::size_t BUFFER_SIZE = sizeof(Err);
  bool m_is_ok = true;
  typename std::aligned_storage<BUFFER_SIZE, ALIGNMENT>::type m_memory_buffer;

  Result() noexcept = default;
};

template <>
class [[nodiscard]] Result<void, void> {
 public:
  [[nodiscard]] static Result ok() noexcept {
    return Result{true};
    ;
  }

  [[nodiscard]] static Result error() noexcept { return Result{false}; }

  [[nodiscard]] bool is_error() const noexcept { return !m_is_ok; }
  [[nodiscard]] bool is_ok() const noexcept { return m_is_ok; }

  [[nodiscard]] explicit operator bool() const noexcept { return m_is_ok; }

  ~Result() noexcept {}

  Result(const Result&) noexcept = default;
  Result(Result &&) noexcept = default;
  Result& operator=(const Result&) noexcept = default;
  Result& operator=(Result&&) noexcept = default;

 private:
  bool m_is_ok = false;
  explicit Result(bool ok) : m_is_ok{ok} {}
};

#define DITTO_VERIFY_OK(expression)               \
  {                                               \
    Ditto::Result _evaluated_result = expression; \
    DITTO_VERIFY(_evaluated_result.is_ok());      \
  }

#define DITTO_VERIFY_ERR(expression)              \
  {                                               \
    Ditto::Result _evaluated_result = expression; \
    DITTO_VERIFY(_evaluated_result.is_error());   \
  }

#define DITTO_PROPAGATE(expression)                                          \
  ({                                                                         \
    Ditto::Result _result = expression;                                      \
    if (_result.is_error()) {                                                \
      return {Ditto::detail::ErrSentinel{std::move(_result).error_value()}}; \
    }                                                                        \
    std::move(_result).ok_value();                                           \
  })

#define DITTO_PROPAGATE_OK(expression)                                   \
  ({                                                                     \
    Ditto::Result _result = expression;                                  \
    if (_result.is_ok()) {                                               \
      return {Ditto::detail::OkSentinel{std::move(_result).ok_value()}}; \
    }                                                                    \
    std::move(_result).error_value();                                    \
  })

#define DITTO_UNWRAP(expression)        \
  ({                                    \
    Ditto::Result _result = expression; \
    DITTO_VERIFY(_result.is_ok())       \
    std::move(_result).ok_value();      \
  })

#define DITTO_UNWRAP_OR(expression, alternative) \
  ({                                             \
    Ditto::Result _result = expression;          \
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
    Ditto::Result _result = expression;           \
    auto unwrap_or = [&]() {                      \
      if (_result.is_ok()) {                      \
        return std::move(_result).ok_value();     \
      } else {                                    \
        return functor();                         \
      }                                           \
    };                                            \
    unwrap_or();                                  \
  })

#define DITTO_UNWRAP_ERR(expression)    \
  ({                                    \
    Ditto::Result _result = expression; \
    DITTO_VERIFY(_result.is_error())    \
    std::move(_result).error_value();   \
  })

#define DITTO_UNWRAP_ERR_OR(expression, alternative) \
  ({                                                 \
    Ditto::Result _result = expression;              \
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
    Ditto::Result _result = expression;               \
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
