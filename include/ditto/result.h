#ifndef DITTO_RESULT_H_
#define DITTO_RESULT_H_

#include <algorithm>
#include <cstddef>
#include <cstdint>
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

template <typename T>
concept NonVoid = !std::same_as<T, void>;

}  // namespace detail

template <class Ok, class Err>
class [[nodiscard]] Result {
 private:
  template <typename T>
  struct Instantiable {
    using type = T;
  };

  template <>
  struct Instantiable<void> {
    using type = std::uint8_t;
  };

 public:
  // Constructors used for propagating errors or values
  template <class... Args, detail::NonVoid T = Ok>
  Result(detail::OkSentinel<T> val) noexcept : m_is_ok{true} {
    new (&m_memory_buffer) T{std::move(val.value)};
  }

  template <class... Args, detail::NonVoid T = Err>
  Result(detail::ErrSentinel<T> val) noexcept : m_is_ok{false} {
    new (&m_memory_buffer) T{std::move(val.value)};
  }

  /// Construct an Ok object with perfect forwarding
  template <class... Args>
  [[nodiscard]] static Result ok(Args... args) noexcept {
    if constexpr (std::is_same_v<void, Ok>) {
      static_assert(sizeof...(Args) == 0,
                    "Result::ok() does not take arguments for 'void'");
      Result res{};
      res.m_is_ok = true;
      return res;
    } else {
      return Result{detail::OkSentinel<Ok>{std::forward<Args>(args)...}};
    }
  }

  /// Construct an Err object with perfect forwarding
  template <class... Args>
  [[nodiscard]] static Result error(Args... args) noexcept {
    if constexpr (std::is_same_v<void, Err>) {
      static_assert(sizeof...(Args) == 0,
                    "Result::error() does not take arguments for 'void'");
      Result res{};
      res.m_is_ok = false;
      return res;
    } else {
      return Result{detail::ErrSentinel<Err>{std::forward<Args>(args)...}};
    }
  }

  [[nodiscard]] bool is_error() const noexcept { return !m_is_ok; }
  [[nodiscard]] bool is_ok() const noexcept { return m_is_ok; }

  template <detail::NonVoid T = Ok>
  [[nodiscard]] const T& ok_value() const& noexcept {
    DITTO_VERIFY(m_is_ok);
    return *reinterpret_cast<const Ok*>(&m_memory_buffer);
  }

  template <detail::NonVoid T = Ok>
  [[nodiscard]] T& ok_value()& noexcept {
    DITTO_VERIFY(m_is_ok);
    return *reinterpret_cast<Ok*>(&m_memory_buffer);
  }

  template <detail::NonVoid T = Ok>
  [[nodiscard]] T&& ok_value()&& noexcept {
    DITTO_VERIFY(m_is_ok);
    return std::move(*reinterpret_cast<Ok*>(&m_memory_buffer));
  }

  template <detail::NonVoid T = Err>
  [[nodiscard]] const T& error_value() const& noexcept {
    DITTO_VERIFY(!m_is_ok);
    return *reinterpret_cast<const Err*>(&m_memory_buffer);
  }

  template <detail::NonVoid T = Err>
  [[nodiscard]] T& error_value()& noexcept {
    DITTO_VERIFY(!m_is_ok);
    return *reinterpret_cast<Err*>(&m_memory_buffer);
  }

  template <detail::NonVoid T = Err>
  [[nodiscard]] T&& error_value()&& noexcept {
    DITTO_VERIFY(!m_is_ok);
    return std::move(*reinterpret_cast<Err*>(&m_memory_buffer));
  }

  template <typename Callable, detail::NonVoid T = Ok>
  [[nodiscard]] auto map_ok(Callable callable) noexcept
      ->Result<decltype(std::declval<Callable>()(std::declval<Ok>())), Err> {
    if (!is_ok()) {
      return {detail::ErrSentinel<Err>{error_value()}};
    }
    return {detail::OkSentinel{callable(ok_value())}};
  }

  template <typename Callable, detail::NonVoid T = Err>
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
      if constexpr (!std::is_same_v<void, Ok>) {
        ok->~Ok();
      }
    } else {
      auto* err = reinterpret_cast<Err*>(&m_memory_buffer);
      if constexpr (!std::is_same_v<void, Err>) {
        err->~Err();
      }
    }
  }

  Result(const Result&) noexcept = default;
  Result(Result &&) noexcept = default;

  Result& operator=(const Result&) noexcept = default;
  Result& operator=(Result&&) noexcept = default;

 private:
  using InstantiableOk = typename Instantiable<Ok>::type;
  using InstantiableErr = typename Instantiable<Err>::type;
  constexpr static std::size_t ALIGNMENT =
      std::max(alignof(InstantiableOk), alignof(InstantiableErr));
  constexpr static std::size_t BUFFER_SIZE =
      std::max(sizeof(InstantiableOk), sizeof(InstantiableErr));
  bool m_is_ok = false;
  typename std::aligned_storage<BUFFER_SIZE, ALIGNMENT>::type m_memory_buffer;

  Result() noexcept = default;
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
