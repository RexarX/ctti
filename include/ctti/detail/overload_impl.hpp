#ifndef CTTI_DETAIL_OVERLOAD_IMPL_HPP
#define CTTI_DETAIL_OVERLOAD_IMPL_HPP

#include <ctti/detail/concepts_impl.hpp>
#include <ctti/detail/hash_impl.hpp>
#include <ctti/detail/meta.hpp>
#include <ctti/detail/name_impl.hpp>
#include <ctti/detail/preprocessor.hpp>
#include <ctti/detail/symbol_impl.hpp>

#include <concepts>
#include <functional>
#include <type_traits>

namespace ctti::detail {

template <typename Symbol, typename T, typename... Args>
  requires requires { Symbol::template Call<Args...>(std::declval<T>(), std::declval<Args>()...); }
constexpr decltype(auto) CallOverload(T&& obj, Args&&... args) {
  return Symbol::template Call<Args...>(std::forward<T>(obj), std::forward<Args>(args)...);
}

template <typename Symbol, typename T>
struct SymbolOverloadWrapper {
  T& obj_;

  template <typename... Args>
    requires requires { CallOverload<Symbol>(obj_, std::declval<Args>()...); }
  constexpr decltype(auto) operator()(Args&&... args) const {
    return CallOverload<Symbol>(obj_, std::forward<Args>(args)...);
  }
};

template <typename Symbol, typename T>
constexpr SymbolOverloadWrapper<Symbol, T> GetOverloadWrapper(T& obj) noexcept {
  return {obj};
}

template <typename Symbol, typename T>
struct OverloadQuery {
  template <typename... Args>
  static constexpr bool Has() noexcept {
    return Symbol::template HasOverload<Args...>();
  }

  template <typename... Args>
  static constexpr auto Get() noexcept {
    if constexpr (Has<Args...>()) {
      return Symbol::template GetMember<T>();
    } else {
      return nullptr;
    }
  }
};

}  // namespace ctti::detail

#endif  // CTTI_DETAIL_OVERLOAD_IMPL_HPP
