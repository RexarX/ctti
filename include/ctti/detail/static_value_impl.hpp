#ifndef CTTI_DETAIL_STATIC_VALUE_IMPL_HPP
#define CTTI_DETAIL_STATIC_VALUE_IMPL_HPP

#include <compare>

namespace ctti::detail {

template <typename T, T Value>
struct StaticValue {
  using value_type = T;
  static constexpr value_type kValue = Value;
  static constexpr value_type value = kValue;

  constexpr StaticValue() noexcept = default;

  constexpr operator value_type() const noexcept { return kValue; }
  constexpr value_type Get() const noexcept { return kValue; }
  constexpr value_type get() const noexcept { return Get(); }

  constexpr auto operator<=>(const StaticValue&) const noexcept = default;
  constexpr bool operator==(const StaticValue&) const noexcept = default;

  friend constexpr bool operator==(const StaticValue&, const value_type& other) noexcept { return kValue == other; }
  friend constexpr bool operator==(const value_type& other, const StaticValue&) noexcept { return kValue == other; }

  friend constexpr auto operator<=>(const StaticValue&, const value_type& other) noexcept { return kValue <=> other; }
  friend constexpr auto operator<=>(const value_type& other, const StaticValue&) noexcept { return other <=> kValue; }
};

}  // namespace ctti::detail

#endif  // CTTI_DETAIL_STATIC_VALUE_IMPL_HPP
