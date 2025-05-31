#ifndef CTTI_HASH_LITERAL_HPP
#define CTTI_HASH_LITERAL_HPP

#include <ctti/detail/hash.hpp>

#include <string_view>

#ifdef CTI_HASH_LITERAL_NAMESPACE
namespace CTTI_HASH_LITERAL_NAMESPACE {
#endif  // CTTI_HASH_LITERAL_NAMESPACE

constexpr std::uint64_t operator""_sh(const char* str, std::size_t length) {
  return ctti::detail::fnv1a_hash(std::string_view{str, length});
}

#ifdef CTTI_HASH_LITERAL_NAMESPACE
}
#endif  // CTTI_HASH_LITERAL_NAMESPACE

#endif  // CTTI_HASH_LITERAL_HPP
