#pragma once

#include <ctti/detail/static_value_impl.hpp>

namespace ctti {

/**
 * @brief A class that represents a static value of a given type.
 */
template <typename T, T Value>
class static_value {
private:
  using internal_type = detail::StaticValue<T, Value>;

public:
  using value_type = T;

  static constexpr value_type value = internal_type::kValue;

  constexpr static_value() noexcept = default;

  constexpr bool operator==(const static_value&) const noexcept = default;
  constexpr auto operator<=>(const static_value&) const noexcept = default;

  template <T OtherValue>
  [[nodiscard]] constexpr bool operator==(const static_value<T, OtherValue>& /*other*/) const noexcept {
    return value == OtherValue;
  }

  template <T OtherValue>
  [[nodiscard]] constexpr auto operator<=>(const static_value<T, OtherValue>& /*other*/) const noexcept {
    return value <=> OtherValue;
  }

  friend constexpr bool operator==([[maybe_unused]] const static_value& val, const value_type& other) noexcept {
    return value == other;
  }

  friend constexpr bool operator==(const value_type& other, [[maybe_unused]] const static_value& val) noexcept {
    return value == other;
  }

  friend constexpr auto operator<=>([[maybe_unused]] const static_value& val, const value_type& other) noexcept {
    return value <=> other;
  }

  friend constexpr auto operator<=>(const value_type& other, [[maybe_unused]] const static_value& val) noexcept {
    return other <=> value;
  }

  /**
   * @brief Get the static value.
   * @return The static value.
   */
  [[nodiscard]] static constexpr value_type get() noexcept { return internal_type::Get(); }
  [[nodiscard]] explicit constexpr operator value_type() const noexcept { return get(); }
};

/**
 * @brief Create a static_value instance.
 * @tparam T The type of the value.
 * @tparam Value The value.
 * @return A static_value instance.
 */
template <typename T, T Value>
[[nodiscard]] constexpr auto make_static_value() noexcept -> static_value<T, Value> {
  return {};
}

/**
 * @brief Create a static_value instance with automatic type deduction.
 * @tparam Value The value.
 * @return A static_value instance.
 */
template <auto Value>
[[nodiscard]] constexpr auto make_static_value() noexcept -> static_value<decltype(Value), Value> {
  return static_value<decltype(Value), Value>{};
}

}  // namespace ctti
