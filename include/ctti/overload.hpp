#ifndef CTTI_OVERLOAD_HPP
#define CTTI_OVERLOAD_HPP

#include <ctti/detail/overload_impl.hpp>

namespace ctti {

template <typename Symbol, typename T, typename... Args>
  requires requires { detail::CallOverload<Symbol>(std::declval<T>(), std::declval<Args>()...); }
constexpr decltype(auto) call_overload(T&& obj, Args&&... args) {
  return detail::CallOverload<Symbol>(std::forward<T>(obj), std::forward<Args>(args)...);
}

template <typename Symbol, typename T>
using overload_set = detail::OverloadSet<Symbol, T>;

template <typename Symbol, typename T>
constexpr overload_set<Symbol, T> get_overload_set(T& obj) noexcept {
  return detail::GetOverloadSet<Symbol, T>(obj);
}

template <typename Symbol, typename T>
struct overload_query {
  template <typename... Args>
  static constexpr bool has() noexcept {
    return detail::OverloadQuery<Symbol, T>::template Has<Args...>();
  }

  template <typename... Args>
  static constexpr auto get() noexcept {
    return detail::OverloadQuery<Symbol, T>::template Get<Args...>();
  }
};

template <typename Symbol, typename T>
constexpr overload_query<Symbol, T> query_overloads() noexcept {
  return {};
}

}  // namespace ctti

#define CTTI_DEFINE_OVERLOADED_SYMBOL(name) CTTI_DETAIL_DEFINE_OVERLOADED_SYMBOL(name)

#endif  // CTTI_OVERLOAD_HPP
