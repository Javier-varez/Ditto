#ifndef DITTO_TYPE_TRAITS_H_
#define DITTO_TYPE_TRAITS_H_

#include <type_traits>

namespace Ditto {

/*
 * @brief Type trait to check whether F is a type in the list of S, T...
 */
template <typename...>
struct is_one_of {
  static constexpr bool value = false;
};

template <typename F, typename S, typename... T>
struct is_one_of<F, S, T...> {
  static constexpr bool value =
      std::is_same<F, S>::value || is_one_of<F, T...>::value;
};

template <class F, class S, class... T>
constexpr inline bool is_one_of_v = is_one_of<F, S, T...>::value;

/*
 * @brief Type trait to check wether an invokable object returns void or not.
 */
template <class Action, class... Args>
using returns_void = std::is_void<::std::result_of_t<Action(Args...)>>;

template <class Action, class... Args>
inline constexpr bool returns_void_v = returns_void<Action, Args...>::value;

}  // namespace Ditto

#endif  // DITTO_TYPE_TRAITS_H_
