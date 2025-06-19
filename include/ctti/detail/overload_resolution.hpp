#pragma once

#include <ctti/detail/meta.hpp>

#include <concepts>
#include <cstddef>
#include <tuple>
#include <utility>

namespace ctti::detail {

template <typename T>
struct FunctionTraits;

template <typename R, typename... Args>
struct FunctionTraits<R(Args...)> {
  using return_type = R;
  using args_tuple = std::tuple<Args...>;

  template <std::size_t I>
  using arg_type = std::tuple_element_t<I, args_tuple>;

  static constexpr std::size_t kArity = sizeof...(Args);
};

template <typename R, typename C, typename... Args>
struct FunctionTraits<R (C::*)(Args...)> : FunctionTraits<R(Args...)> {
  using class_type = C;
  using pointer_type = R (C::*)(Args...);

  static constexpr bool kIsConst = false;
  static constexpr bool kIsMemberFunction = true;
};

template <typename R, typename C, typename... Args>
struct FunctionTraits<R (C::*)(Args...) const> : FunctionTraits<R(Args...)> {
  using class_type = C;
  using pointer_type = R (C::*)(Args...) const;

  static constexpr bool kIsConst = true;
  static constexpr bool kIsMemberFunction = true;
};

template <typename R, typename C, typename... Args>
struct FunctionTraits<R (C::*)(Args...) noexcept> : FunctionTraits<R(Args...)> {
  using class_type = C;
  using pointer_type = R (C::*)(Args...) noexcept;

  static constexpr bool kIsConst = false;
  static constexpr bool kIsMemberFunction = true;
  static constexpr bool kIsNoexcept = true;
};

template <typename R, typename C, typename... Args>
struct FunctionTraits<R (C::*)(Args...) const noexcept> : FunctionTraits<R(Args...)> {
  using class_type = C;
  using pointer_type = R (C::*)(Args...) const noexcept;

  static constexpr bool kIsConst = true;
  static constexpr bool kIsMemberFunction = true;
  static constexpr bool kIsNoexcept = true;
};

template <typename T, typename... Args>
concept CallableWith = std::invocable<T, Args...>;

template <typename T, typename C, typename... Args>
concept MemberCallableWith = std::invocable<T, C&&, Args&&...>;

template <typename T, typename Name, typename... Args>
concept HasMemberOverload = requires(T&& obj, Args&&... args) {
  { std::forward<T>(obj).*Name{}(std::forward<Args>(args)...) };
};

template <typename Signature>
struct OverloadSignature;

template <typename R, typename... Args>
struct OverloadSignature<R(Args...)> {
  using return_type = R;
  using args_tuple = std::tuple<Args...>;

  template <std::size_t I>
  using arg_type = std::tuple_element_t<I, args_tuple>;

  static constexpr std::size_t kArity = sizeof...(Args);
};

}  // namespace ctti::detail
