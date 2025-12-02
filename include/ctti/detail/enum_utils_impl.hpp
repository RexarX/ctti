#pragma once

#include <ctti/detail/meta.hpp>
#include <ctti/detail/name_impl.hpp>

#include <array>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <string_view>
#include <type_traits>
#include <utility>

namespace ctti::detail {

template <typename E>
concept ScopedEnum = std::is_enum_v<E> && !std::convertible_to<E, std::underlying_type_t<E>>;

template <typename E>
concept UnscopedEnum = std::is_enum_v<E> && std::convertible_to<E, std::underlying_type_t<E>>;

template <auto... Values>
concept SameEnumValues =
    (std::same_as<decltype(Values), decltype((Values, ...))> && ...) && std::is_enum_v<decltype((Values, ...))>;

// Runtime enum name lookup
template <typename E, std::size_t N>
[[nodiscard]] constexpr std::optional<std::string_view> EnumNameRuntime(
    E value, const std::array<E, N>& values, const std::array<std::string_view, N>& names) noexcept {
  using underlying = std::underlying_type_t<E>;
  auto underlying_value = static_cast<underlying>(value);

  for (std::size_t i = 0; i < N; ++i) {
    if (static_cast<underlying>(values[i]) == underlying_value) {
      return names[i];
    }
  }
  return std::nullopt;
}

// Runtime string to enum conversion
template <typename E, std::size_t N>
[[nodiscard]] constexpr std::optional<E> EnumCastRuntime(std::string_view name, const std::array<E, N>& values,
                                                         const std::array<std::string_view, N>& names) noexcept {
  for (std::size_t i = 0; i < N; ++i) {
    if (names[i] == name) {
      return values[i];
    }
  }
  return std::nullopt;
}

// Check if value is in the valid enum values
template <typename E, std::size_t N>
[[nodiscard]] constexpr bool EnumContainsRuntime(E value, const std::array<E, N>& values) noexcept {
  using underlying = std::underlying_type_t<E>;
  auto underlying_value = static_cast<underlying>(value);

  for (std::size_t i = 0; i < N; ++i) {
    if (static_cast<underlying>(values[i]) == underlying_value) {
      return true;
    }
  }
  return false;
}

// Get index of enum value
template <typename E, std::size_t N>
[[nodiscard]] constexpr std::optional<std::size_t> EnumIndexRuntime(E value, const std::array<E, N>& values) noexcept {
  using underlying = std::underlying_type_t<E>;
  auto underlying_value = static_cast<underlying>(value);

  for (std::size_t i = 0; i < N; ++i) {
    if (static_cast<underlying>(values[i]) == underlying_value) {
      return i;
    }
  }
  return std::nullopt;
}

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
  using underlying_type = std::underlying_type_t<E>;
  using values = TypeList<SizeType<static_cast<std::size_t>(Values)>...>;

  static constexpr std::size_t kCount = sizeof...(Values);
  static constexpr std::array<E, kCount> kValues = {Values...};
  static constexpr std::array<std::string_view, kCount> kNames = {ValueNameOfImpl<E, Values>::Apply()...};

  [[nodiscard]] static constexpr auto ValueArray() noexcept -> const std::array<E, kCount>& { return kValues; }

  [[nodiscard]] static constexpr auto NameArray() noexcept -> const std::array<std::string_view, kCount>& {
    return kNames;
  }

  template <std::size_t I>
    requires(I < kCount)
  [[nodiscard]] static constexpr E At() noexcept {
    return kValues[I];
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

  [[nodiscard]] static constexpr bool ContainsRuntime(E value) noexcept { return EnumContainsRuntime(value, kValues); }

  [[nodiscard]] static constexpr auto Names() noexcept -> std::array<std::string_view, kCount> {
    return {ValueNameOfImpl<E, Values>::Apply()...};
  }

  [[nodiscard]] static constexpr std::optional<std::string_view> NameOf(E value) noexcept {
    return EnumNameRuntime(value, kValues, kNames);
  }

  [[nodiscard]] static constexpr std::optional<E> Cast(std::string_view name) noexcept {
    return EnumCastRuntime(name, kValues, kNames);
  }

  [[nodiscard]] static constexpr std::optional<std::size_t> IndexOf(E value) noexcept {
    return EnumIndexRuntime(value, kValues);
  }

  [[nodiscard]] static constexpr std::optional<E> ValueAt(std::size_t index) noexcept {
    if (index < kCount) {
      return kValues[index];
    }
    return std::nullopt;
  }

  [[nodiscard]] static constexpr std::optional<E> FromUnderlying(underlying_type value) noexcept {
    auto enum_val = static_cast<E>(value);
    if (ContainsRuntime(enum_val)) {
      return enum_val;
    }
    return std::nullopt;
  }

  static consteval auto Entries() noexcept -> std::array<std::pair<E, std::string_view>, kCount> {
    std::array<std::pair<E, std::string_view>, kCount> result{};
    for (std::size_t i = 0; i < kCount; ++i) {
      result[i] = {kValues[i], kNames[i]};
    }
    return result;
  }

  static constexpr auto kEntries = Entries();
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
