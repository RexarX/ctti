#ifndef CTTI_DETAILED_NAMEOF_HPP
#define CTTI_DETAILED_NAMEOF_HPP

#include <ctti/name.hpp>
#include <ctti/nameof.hpp>

namespace ctti {

template <typename T>
constexpr ctti::name_t detailed_nameof() noexcept {
  return {ctti::nameof<T>()};
}

template <typename T, T Value>
constexpr ctti::name_t detailed_nameof() noexcept {
  return {ctti::nameof<T, Value>()};
}

// Variable templates are standard in C++23
template <typename T>
inline constexpr ctti::name_t detailed_nameof_v = ctti::detailed_nameof<T>();

template <typename T, T Value>
inline constexpr ctti::name_t detailed_nameof_value_v = ctti::detailed_nameof<T, Value>();

}  // namespace ctti

#endif  // CTTI_DETAILED_NAMEOF_HPP
