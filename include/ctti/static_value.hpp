#ifndef CTTI_STATIC_VALUE_HPP
#define CTTI_STATIC_VALUE_HPP

#include <ctti/detail/static_value_impl.hpp>

namespace ctti {

template <typename T, T Value>
class static_value {
private:
  using internal_type = detail::StaticValue<T, Value>;

public:
  using value_type = T;
  static constexpr value_type value = internal_type::kValue;

  constexpr static_value() noexcept = default;

  static constexpr value_type get() noexcept { return internal_type::Get(); }
  constexpr operator value_type() const noexcept { return get(); }

  constexpr auto operator<=>(const static_value&) const noexcept = default;
  constexpr bool operator==(const static_value&) const noexcept = default;

  friend constexpr bool operator==(const static_value&, const value_type& other) noexcept { return value == other; }
  friend constexpr bool operator==(const value_type& other, const static_value&) noexcept { return value == other; }

  friend constexpr auto operator<=>(const static_value&, const value_type& other) noexcept { return value <=> other; }
  friend constexpr auto operator<=>(const value_type& other, const static_value&) noexcept { return other <=> value; }
};

template <typename T, T Value>
constexpr static_value<T, Value> make_static_value() noexcept {
  return {};
}

template <auto Value>
constexpr auto make_static_value() noexcept {
  return static_value<decltype(Value), Value>{};
}

}  // namespace ctti

#endif  // CTTI_STATIC_VALUE_HPP
