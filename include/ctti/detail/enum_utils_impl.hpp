#ifndef CTTI_DETAIL_ENUM_UTILS_IMPL_HPP
#define CTTI_DETAIL_ENUM_UTILS_IMPL_HPP

#include <ctti/detail/concepts_impl.hpp>
#include <ctti/detail/meta.hpp>
#include <ctti/detail/name_impl.hpp>
#include <ctti/name.hpp>

#include <array>
#include <concepts>
#include <optional>
#include <string_view>
#include <type_traits>
#include <vector>

namespace ctti::detail {

template <typename E>
concept ScopedEnum = std::is_enum_v<E> && !std::convertible_to<E, std::underlying_type_t<E>>;

template <typename E>
concept UnscopedEnum = std::is_enum_v<E> && std::convertible_to<E, std::underlying_type_t<E>>;

template <EnumType E>
struct EnumInfo {
  using enum_type = E;
  using underlying_type = std::underlying_type_t<E>;

  template <E Value>
  static constexpr std::string_view NameOfValue() noexcept {
    return ctti::name_of<E, Value>();
  }

  template <E Value>
  static constexpr underlying_type UnderlyingValue() noexcept {
    return static_cast<underlying_type>(Value);
  }

  static constexpr std::string_view Name() noexcept { return ctti::name_of<E>(); }

  static constexpr bool IsScoped() noexcept { return ScopedEnum<E>; }

  template <std::convertible_to<underlying_type> T>
  static constexpr std::optional<E> FromUnderlying(T value) noexcept {
    return static_cast<E>(value);
  }

  template <E Value>
  static constexpr auto DetailedName() noexcept {
    return ctti::qualified_name_of<E, Value>();
  }
};

template <EnumType E, E... Values>
struct EnumValueList {
  using enum_type = E;
  using values = TypeList<SizeType<static_cast<std::size_t>(Values)>...>;
  static constexpr std::size_t kCount = sizeof...(Values);

  static constexpr std::array<E, kCount> ValueArray() noexcept { return {Values...}; }

  template <std::size_t I>
    requires(I < kCount)
  static constexpr E At() noexcept {
    return ValueArray()[I];
  }

  template <E Value>
  static constexpr bool Contains() noexcept {
    return ((Values == Value) || ...);
  }

  template <typename F>
  static constexpr void ForEach(F&& f) {
    auto call_f = [&f](auto index_wrapper, E value) { f(index_wrapper, value); };
    (call_f(SizeType<static_cast<std::size_t>(Values)>{}, Values), ...);
  }

  static constexpr std::array<std::string_view, kCount> Names() noexcept { return {ctti::name_of<E, Values>()...}; }
};

template <EnumType E, E Lhs, E Rhs>
constexpr bool EnumEqual() noexcept {
  return Lhs == Rhs;
}

template <EnumType E, E Lhs, E Rhs>
constexpr bool EnumLess() noexcept {
  return static_cast<std::underlying_type_t<E>>(Lhs) < static_cast<std::underlying_type_t<E>>(Rhs);
}

}  // namespace ctti::detail

#endif  // CTTI_DETAIL_ENUM_UTILS_IMPL_HPP
