
#pragma once

#include <type_traits>

namespace Ditto {

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

}  // namespace Ditto
