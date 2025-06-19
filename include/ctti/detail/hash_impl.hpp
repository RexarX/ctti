#pragma once

#include <cstddef>
#include <cstdint>
#include <string_view>

namespace ctti::detail {

using HashType = std::uint64_t;

constexpr HashType kFnvBasis = 14695981039346656037ull;
constexpr HashType kFnvPrime = 1099511628211ull;

constexpr HashType Fnv1aHash(std::string_view str, HashType hash = kFnvBasis) noexcept {
  for (char ch : str) {
    hash = (hash ^ static_cast<HashType>(ch)) * kFnvPrime;
  }
  return hash;
}

template <std::size_t N>
constexpr HashType Fnv1aHash(const char (&array)[N]) noexcept {
  return Fnv1aHash(std::string_view{array, N - 1});
}

}  // namespace ctti::detail
