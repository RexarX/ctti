#ifndef CTTI_DETAIL_HASH_HPP
#define CTTI_DETAIL_HASH_HPP

#include <cstdint>
#include <string_view>

namespace ctti::detail {

using hash_t = std::uint64_t;

// FNV-1a hash parameters
constexpr hash_t fnv_basis = 14695981039346656037ull;
constexpr hash_t fnv_prime = 1099511628211ull;

// FNV-1a 64-bit hash implementation for various inputs
constexpr hash_t fnv1a_hash(std::string_view str, hash_t hash = fnv_basis) noexcept {
  for (char c : str) {
    hash = (hash ^ static_cast<hash_t>(c)) * fnv_prime;
  }
  return hash;
}

constexpr hash_t fnv1a_hash(std::size_t n, const char* str, hash_t hash = fnv_basis) noexcept {
  return n > 0 ? fnv1a_hash(n - 1, str + 1, (hash ^ static_cast<hash_t>(*str)) * fnv_prime) : hash;
}

template <std::size_t N>
constexpr hash_t fnv1a_hash(const char (&array)[N]) noexcept {
  return fnv1a_hash(std::string_view{array, N - 1});
}

}  // namespace ctti::detail

#endif  // CTTI_DETAIL_HASH_HPP
