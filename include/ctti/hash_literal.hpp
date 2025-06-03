#ifndef CTTI_HASH_LITERAL_HPP
#define CTTI_HASH_LITERAL_HPP

#include <ctti/detail/hash.hpp>

#include <array>
#include <cstdint>
#include <string_view>

#ifdef CTTI_HASH_LITERAL_NAMESPACE
namespace CTTI_HASH_LITERAL_NAMESPACE {
#endif

constexpr std::uint64_t operator""_sh(const char* str, std::size_t length) noexcept {
  return ctti::detail::Fnv1aHash(std::string_view{str, length});
}

#ifdef CTTI_HASH_LITERAL_NAMESPACE
}
#endif

#endif  // CTTI_HASH_LITERAL_HPP
