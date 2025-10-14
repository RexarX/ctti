#pragma once

#include <cstddef>
#include <cstdint>
#include <string_view>

namespace ctti::detail {

using HashType = std::size_t;

#if SIZE_MAX == UINT64_MAX
constexpr HashType kFnvBasis = 14695981039346656037ULL;
constexpr HashType kFnvPrime = 1099511628211ULL;
#elif SIZE_MAX == UINT32_MAX
constexpr HashType kFnvBasis = 2166136261U;
constexpr HashType kFnvPrime = 16777619U;
#else
#error "Unsupported platform: size_t must be 32 or 64 bits"
#endif

[[nodiscard]] constexpr HashType Fnv1aHash(std::string_view str, HashType hash = kFnvBasis) noexcept {
  for (const char ch : str) {
    hash = (hash ^ static_cast<HashType>(ch)) * kFnvPrime;
  }
  return hash;
}

template <std::size_t N>
[[nodiscard]] constexpr HashType Fnv1aHash(const char (&array)[N]) noexcept {
  return Fnv1aHash(std::string_view{array, N - 1});
}

}  // namespace ctti::detail
