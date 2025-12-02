#pragma once

#include <string_view>

#if defined(__clang__)
#define CTTI_PRETTY_FUNCTION __PRETTY_FUNCTION__
#elif defined(__GNUC__) && !defined(__clang__)
#define CTTI_PRETTY_FUNCTION __PRETTY_FUNCTION__
#elif defined(_MSC_VER)
#define CTTI_PRETTY_FUNCTION __FUNCSIG__
#else
#error "Unsupported compiler"
#endif

namespace ctti::detail {

/**
 * @brief Provides access to raw compiler-specific pretty function signatures.
 *
 * This struct exposes the raw `__PRETTY_FUNCTION__` (GCC/Clang) or `__FUNCSIG__` (MSVC)
 * output for a given type or value. The format is compiler-dependent and should only
 * be used for debugging or low-level introspection.
 *
 * @note For extracting clean type/value names, use `ctti::name_of<T>()` instead.
 */
struct PrettyFunction {
  /**
   * @brief Gets the raw pretty function signature for a type.
   * @tparam T The type to inspect.
   * @return The raw compiler-specific signature string.
   */
  template <typename T>
  [[nodiscard]] static constexpr std::string_view Type() noexcept {
    return CTTI_PRETTY_FUNCTION;
  }

  /**
   * @brief Gets the raw pretty function signature for a value.
   * @tparam T The type of the value.
   * @tparam ValueParam The compile-time value to inspect.
   * @return The raw compiler-specific signature string.
   */
  template <typename T, T ValueParam>
  [[nodiscard]] static constexpr std::string_view Value() noexcept {
    return CTTI_PRETTY_FUNCTION;
  }
};

inline namespace pretty_function {

/**
 * @brief Gets the raw pretty function signature for a type.
 * @tparam T The type to inspect.
 * @return The raw compiler-specific signature string.
 */
template <typename T>
[[nodiscard]] constexpr std::string_view Type() noexcept {
  return PrettyFunction::Type<T>();
}

/**
 * @brief Gets the raw pretty function signature for a value.
 * @tparam T The type of the value.
 * @tparam ValueParam The compile-time value to inspect.
 * @return The raw compiler-specific signature string.
 */
template <typename T, T ValueParam>
[[nodiscard]] constexpr std::string_view Value() noexcept {
  return PrettyFunction::Value<T, ValueParam>();
}

}  // namespace pretty_function

}  // namespace ctti::detail
