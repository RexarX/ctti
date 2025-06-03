#ifndef CTTI_DETAIL_OVERLOAD_RESOLUTION_HPP
#define CTTI_DETAIL_OVERLOAD_RESOLUTION_HPP

#include <concepts>
#include <ctti/detail/meta.hpp>
#include <functional>
#include <tuple>
#include <type_traits>

namespace ctti::detail {

template <typename T>
struct FunctionTraits;

template <typename R, typename... Args>
struct FunctionTraits<R(Args...)> {
  using return_type = R;
  using args_tuple = std::tuple<Args...>;
  static constexpr std::size_t arity = sizeof...(Args);

  template <std::size_t I>
  using arg_type = std::tuple_element_t<I, args_tuple>;
};

template <typename R, typename C, typename... Args>
struct FunctionTraits<R (C::*)(Args...)> : FunctionTraits<R(Args...)> {
  using class_type = C;
  using pointer_type = R (C::*)(Args...);
  static constexpr bool is_const = false;
  static constexpr bool is_member_function = true;
};

template <typename R, typename C, typename... Args>
struct FunctionTraits<R (C::*)(Args...) const> : FunctionTraits<R(Args...)> {
  using class_type = C;
  using pointer_type = R (C::*)(Args...) const;
  static constexpr bool is_const = true;
  static constexpr bool is_member_function = true;
};

template <typename R, typename C, typename... Args>
struct FunctionTraits<R (C::*)(Args...) noexcept> : FunctionTraits<R(Args...)> {
  using class_type = C;
  using pointer_type = R (C::*)(Args...) noexcept;
  static constexpr bool is_const = false;
  static constexpr bool is_member_function = true;
  static constexpr bool is_noexcept = true;
};

template <typename R, typename C, typename... Args>
struct FunctionTraits<R (C::*)(Args...) const noexcept> : FunctionTraits<R(Args...)> {
  using class_type = C;
  using pointer_type = R (C::*)(Args...) const noexcept;
  static constexpr bool is_const = true;
  static constexpr bool is_member_function = true;
  static constexpr bool is_noexcept = true;
};

template <typename T, typename... Args>
concept CallableWith = std::invocable<T, Args...>;

template <typename T, typename C, typename... Args>
concept MemberCallableWith = std::invocable<T, C&&, Args&&...>;

template <typename T, typename Name, typename... Args>
concept HasMemberOverload = requires(T&& obj, Args&&... args) {
  { std::forward<T>(obj).*Name{}(std::forward<Args>(args)...) };
};

// Overload signature storage
template <typename Signature>
struct OverloadSignature;

template <typename R, typename... Args>
struct OverloadSignature<R(Args...)> {
  using return_type = R;
  using args_tuple = std::tuple<Args...>;
  static constexpr std::size_t arity = sizeof...(Args);

  template <std::size_t I>
  using arg_type = std::tuple_element_t<I, args_tuple>;
};

}  // namespace ctti::detail

#endif  // CTTI_DETAIL_OVERLOAD_RESOLUTION_HPP
