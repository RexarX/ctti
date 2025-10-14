#pragma once

#include <ctti/detail/hash_impl.hpp>

#include <cstddef>
#include <string_view>

namespace ctti {

/**
 * @brief Compute the FNV-1a hash of a string at compile time.
 * @param str The input string.
 * @return The FNV-1a hash of the input string.
 */
[[nodiscard]] constexpr std::size_t fnv1a_hash(std::string_view str) noexcept {
  return detail::Fnv1aHash(str);
}

namespace hash_literals {

/**
 * @brief User-defined literal for computing the FNV-1a hash of a string at compile time.
 * @param str The input string.
 * @param length The length of the input string.
 * @return The FNV-1a hash of the input string.
 */
[[nodiscard]] constexpr std::size_t operator""_sh(const char* str, std::size_t length) noexcept {
  return ctti::fnv1a_hash({str, length});
}

}  // namespace hash_literals

}  // namespace ctti
