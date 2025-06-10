#ifndef CTTI_HASH_HPP
#define CTTI_HASH_HPP

#include <ctti/detail/hash_impl.hpp>

namespace ctti {

constexpr std::uint64_t fnv1a_hash(std::string_view str) noexcept {
  return detail::Fnv1aHash(str);
}

namespace hash_literals {

constexpr std::uint64_t operator""_sh(const char* str, std::size_t length) noexcept {
  return ctti::fnv1a_hash(std::string_view{str, length});
}

}  // namespace hash_literals

}  // namespace ctti

#endif  // CTTI_HASH_HPP
