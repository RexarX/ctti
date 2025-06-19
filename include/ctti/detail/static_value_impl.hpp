#pragma once

#include <compare>

namespace ctti::detail {

template <typename T, T Value>
struct StaticValue {
  using value_type = T;

  static constexpr value_type kValue = Value;

  constexpr StaticValue() noexcept = default;

  constexpr auto operator<=>(const StaticValue&) const noexcept = default;
  constexpr bool operator==(const StaticValue&) const noexcept = default;

  friend constexpr bool operator==([[maybe_unused]] const StaticValue& value, const value_type& other) noexcept {
    return kValue == other;
  }

  friend constexpr bool operator==(const value_type& other, [[maybe_unused]] const StaticValue& value) noexcept {
    return kValue == other;
  }

  friend constexpr auto operator<=>([[maybe_unused]] const StaticValue& value, const value_type& other) noexcept {
    return kValue <=> other;
  }

  friend constexpr auto operator<=>(const value_type& other, [[maybe_unused]] const StaticValue& value) noexcept {
    return other <=> kValue;
  }

  static constexpr value_type Get() noexcept { return kValue; }
  constexpr operator value_type() const noexcept { return Get(); }
};

}  // namespace ctti::detail
