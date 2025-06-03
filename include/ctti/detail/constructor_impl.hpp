#ifndef CTTI_DETAIL_CONSTRUCTOR_IMPL_HPP
#define CTTI_DETAIL_CONSTRUCTOR_IMPL_HPP

#include <ctti/detail/meta.hpp>
#include <ctti/detail/name_impl.hpp>
#include <ctti/type_tag.hpp>

#include <concepts>
#include <memory>
#include <type_traits>
#include <utility>

namespace ctti::detail {

template <typename T, typename... Args>
concept Constructible = std::constructible_from<T, Args...>;

template <typename T, typename... Args>
concept NothrowConstructible = std::is_nothrow_constructible_v<T, Args...>;

template <typename T>
concept DefaultConstructible = std::default_initializable<T>;

template <typename T>
concept CopyConstructible = std::copy_constructible<T>;

template <typename T>
concept MoveConstructible = std::move_constructible<T>;

template <typename T>
concept AggregateType = std::is_aggregate_v<T>;

template <typename T>
struct ConstructorInfo {
  using type = T;

  template <typename... Args>
    requires Constructible<T, Args...>
  static constexpr T Construct(Args&&... args) noexcept(NothrowConstructible<T, Args...>) {
    if constexpr (AggregateType<T> && sizeof...(Args) > 0) {
      return T{std::forward<Args>(args)...};
    } else {
      return T(std::forward<Args>(args)...);
    }
  }

  template <typename... Args>
    requires Constructible<T, Args...>
  static constexpr std::unique_ptr<T> MakeUnique(Args&&... args) {
    return std::make_unique<T>(std::forward<Args>(args)...);
  }

  template <typename... Args>
    requires Constructible<T, Args...>
  static constexpr std::shared_ptr<T> MakeShared(Args&&... args) {
    return std::make_shared<T>(std::forward<Args>(args)...);
  }

  template <typename... Args>
  static constexpr bool CanConstruct() noexcept {
    return Constructible<T, Args...>;
  }

  template <typename... Args>
  static constexpr bool CanConstructNothrow() noexcept {
    if constexpr (Constructible<T, Args...>) {
      return NothrowConstructible<T, Args...>;
    }
    return false;
  }

  static constexpr bool IsDefaultConstructible() noexcept { return DefaultConstructible<T>; }

  static constexpr bool IsCopyConstructible() noexcept { return CopyConstructible<T>; }

  static constexpr bool IsMoveConstructible() noexcept { return MoveConstructible<T>; }

  static constexpr bool IsAggregate() noexcept { return AggregateType<T>; }
};

template <typename T, typename... Args>
struct ConstructorSignature {
  using type = T;
  using args_tuple = std::tuple<Args...>;
  static constexpr std::size_t kArity = sizeof...(Args);

  template <std::size_t I>
    requires(I < kArity)
  using ArgType = std::tuple_element_t<I, args_tuple>;

  static constexpr bool kIsValid = Constructible<T, Args...>;
  static constexpr bool kIsNothrow = NothrowConstructible<T, Args...>;
};

}  // namespace ctti::detail

#endif  // CTTI_DETAIL_CONSTRUCTOR_IMPL_HPP
