#ifndef CTTI_OVERLOAD_HPP
#define CTTI_OVERLOAD_HPP

#include <ctti/detail/member_traits.hpp>
#include <ctti/symbol.hpp>

#include <concepts>
#include <type_traits>

namespace ctti {

template <auto Symbol, typename T, typename... Args>
  requires(decltype(Symbol)::template has_overload<T, Args...>())
constexpr decltype(auto) call_overload(T&& obj, Args&&... args) {
  return decltype(Symbol)::template call<T, Args...>(std::forward<T>(obj), std::forward<Args>(args)...);
}

template <auto Symbol, typename T>
struct overload_wrapper {
  T& obj_;

  template <typename... Args>
    requires(decltype(Symbol)::template has_overload<T&, Args...>())
  constexpr decltype(auto) operator()(Args&&... args) const {
    return call_overload<Symbol>(obj_, std::forward<Args>(args)...);
  }
};

template <auto Symbol, typename T>
constexpr overload_wrapper<Symbol, T> get_overload_wrapper(T& obj) noexcept {
  return {obj};
}

template <auto Symbol, typename T>
struct overload_query {
  template <typename... Args>
  static constexpr bool has() noexcept {
    return decltype(Symbol)::template has_overload<T, Args...>();
  }

  template <typename... Args>
  static constexpr auto get() noexcept {
    if constexpr (has<Args...>()) {
      return decltype(Symbol)::template get_member<T>();
    } else {
      return nullptr;
    }
  }
};

template <auto Symbol, typename T>
constexpr overload_query<Symbol, T> query_overloads() noexcept {
  return {};
}

template <typename ReturnType, typename... Args>
struct overload_signature {
  using return_type = ReturnType;
  using args_tuple = std::tuple<Args...>;
  static constexpr std::size_t arity = sizeof...(Args);

  template <std::size_t I>
    requires(I < arity)
  using arg_type = std::tuple_element_t<I, args_tuple>;
};

template <typename T>
concept has_overloads = T::has_overloads;

template <auto Symbol>
  requires(has_overloads<decltype(Symbol)>)
constexpr std::size_t get_overload_count() noexcept {
  return decltype(Symbol)::overload_count;
}

template <auto Symbol, typename... Args>
constexpr bool can_call_with() noexcept {
  return decltype(Symbol)::template has_overload<Args...>();
}

}  // namespace ctti

#endif  // CTTI_OVERLOAD_HPP
