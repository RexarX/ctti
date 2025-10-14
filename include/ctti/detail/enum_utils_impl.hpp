#pragma once

#include <ctti/detail/meta.hpp>
#include <ctti/detail/name_impl.hpp>

#include <array>
#include <concepts>
#include <cstddef>
#include <optional>
#include <string_view>
#include <type_traits>

namespace ctti::detail {

template <typename E>
concept ScopedEnum = std::is_enum_v<E> && !std::convertible_to<E, std::underlying_type_t<E>>;

template <typename E>
concept UnscopedEnum = std::is_enum_v<E> && std::convertible_to<E, std::underlying_type_t<E>>;

template <auto... Values>
concept SameEnumValues =
    (std::same_as<decltype(Values), decltype((Values, ...))> && ...) && std::is_enum_v<decltype((Values, ...))>;

template <typename E>
  requires std::is_enum_v<E>
struct EnumInfo {
  using enum_type = E;
  using underlying_type = std::underlying_type_t<E>;

  template <E Value>
  [[nodiscard]] static constexpr std::string_view NameOfValue() noexcept {
    return ValueNameOfImpl<E, Value>::Apply();
  }

  template <E Value>
  [[nodiscard]] static constexpr underlying_type UnderlyingValue() noexcept {
    return static_cast<underlying_type>(Value);
  }

  template <std::convertible_to<underlying_type> T>
  [[nodiscard]] static constexpr auto FromUnderlying(T value) noexcept -> std::optional<E> {
    return static_cast<E>(value);
  }

  [[nodiscard]] static constexpr std::string_view Name() noexcept { return NameOfImpl<E>::Apply(); }

  [[nodiscard]] static constexpr bool IsScoped() noexcept { return ScopedEnum<E>; }
};

template <typename E, E... Values>
  requires std::is_enum_v<E>
struct EnumValueList {
  using enum_type = E;
  using values = TypeList<SizeType<static_cast<std::size_t>(Values)>...>;

  static constexpr std::size_t kCount = sizeof...(Values);

  [[nodiscard]] static constexpr auto ValueArray() noexcept -> std::array<E, kCount> { return {Values...}; }

  template <std::size_t I>
    requires(I < kCount)
  [[nodiscard]] static constexpr E At() noexcept {
    return ValueArray()[I];
  }

  template <typename F>
    requires(std::invocable<const F&, SizeType<static_cast<std::size_t>(Values)>, decltype(Values)> && ...)
  static constexpr void ForEach(const F& func) noexcept(
      (std::is_nothrow_invocable_v<const F&, SizeType<static_cast<std::size_t>(Values)>, decltype(Values)> && ...)) {
    auto call_f = [&func](auto index_wrapper, E value) { func(index_wrapper, value); };
    (call_f(SizeType<static_cast<std::size_t>(Values)>{}, Values), ...);
  }

  template <E Value>
  [[nodiscard]] static constexpr bool Contains() noexcept {
    return ((Values == Value) || ...);
  }

  [[nodiscard]] static constexpr auto Names() noexcept -> std::array<std::string_view, kCount> {
    return {ValueNameOfImpl<E, Values>::Apply()...};
  }
};

template <typename E, E Lhs, E Rhs>
  requires std::is_enum_v<E>
[[nodiscard]] constexpr bool EnumEqual() noexcept {
  return Lhs == Rhs;
}

template <typename E, E Lhs, E Rhs>
  requires std::is_enum_v<E>
[[nodiscard]] constexpr bool EnumLess() noexcept {
  return static_cast<std::underlying_type_t<E>>(Lhs) < static_cast<std::underlying_type_t<E>>(Rhs);
}

}  // namespace ctti::detail
