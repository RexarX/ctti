#pragma once

#include <ctti/detail/hash_impl.hpp>

#include <cstddef>
#include <cstdint>

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
