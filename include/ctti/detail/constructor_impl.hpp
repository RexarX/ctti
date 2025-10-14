#pragma once

#include <concepts>
#include <cstddef>
#include <memory>
#include <tuple>
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
  [[nodiscard]] static constexpr T Construct(Args&&... args) noexcept(NothrowConstructible<T, Args...>) {
    if constexpr (AggregateType<T> && sizeof...(Args) > 0) {
      return T{std::forward<Args>(args)...};
    } else {
      return T(std::forward<Args>(args)...);
    }
  }

  template <typename... Args>
    requires Constructible<T, Args...>
  [[nodiscard]] static constexpr auto MakeUnique(Args&&... args) -> std::unique_ptr<T> {
    return std::make_unique<T>(std::forward<Args>(args)...);
  }

  template <typename... Args>
    requires Constructible<T, Args...>
  [[nodiscard]] static constexpr auto MakeShared(Args&&... args) -> std::shared_ptr<T> {
    return std::make_shared<T>(std::forward<Args>(args)...);
  }

  template <typename... Args>
  [[nodiscard]] static constexpr bool CanConstruct() noexcept {
    return Constructible<T, Args...>;
  }

  template <typename... Args>
  [[nodiscard]] static constexpr bool CanConstructNothrow() noexcept {
    if constexpr (Constructible<T, Args...>) {
      return NothrowConstructible<T, Args...>;
    }
    return false;
  }

  [[nodiscard]] static constexpr bool IsDefaultConstructible() noexcept { return DefaultConstructible<T>; }
  [[nodiscard]] static constexpr bool IsCopyConstructible() noexcept { return CopyConstructible<T>; }
  [[nodiscard]] static constexpr bool IsMoveConstructible() noexcept { return MoveConstructible<T>; }
  [[nodiscard]] static constexpr bool IsAggregate() noexcept { return AggregateType<T>; }
};

template <typename T, typename... Args>
struct ConstructorSignature {
  using type = T;
  using args_tuple = std::tuple<Args...>;

  static constexpr std::size_t kArity = sizeof...(Args);
  static constexpr bool kIsValid = Constructible<T, Args...>;
  static constexpr bool kIsNothrow = NothrowConstructible<T, Args...>;

  template <std::size_t I>
    requires(I < kArity)
  using ArgType = std::tuple_element_t<I, args_tuple>;
};

}  // namespace ctti::detail
