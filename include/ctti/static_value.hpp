#ifndef CTTI_DETAIL_STATIC_VALUE_HPP
#define CTTI_DETAIL_STATIC_VALUE_HPP

#include <compare>

namespace ctti {

template <typename T, T Value>
struct static_value {
  using value_type = T;
  static constexpr value_type value = Value;

  constexpr static_value() noexcept = default;

  constexpr operator value_type() const noexcept { return Value; }

  constexpr value_type get() const noexcept { return Value; }

  constexpr auto operator<=>(const static_value&) const noexcept = default;
  constexpr bool operator==(const static_value&) const noexcept = default;

  friend constexpr bool operator==(const static_value&, const value_type& other) noexcept { return Value == other; }
  friend constexpr bool operator==(const value_type& other, const static_value&) noexcept { return Value == other; }

  friend constexpr auto operator<=>(const static_value&, const value_type& other) noexcept { return Value <=> other; }
  friend constexpr auto operator<=>(const value_type& other, const static_value&) noexcept { return other <=> Value; }
};

template <typename T, T Value>
constexpr T static_value<T, Value>::value;

}  // namespace ctti

#define CTTI_STATIC_VALUE(x) ::ctti::static_value<decltype(x), (x)>

#endif  // CTTI_DETAIL_STATIC_VALUE_HPP
