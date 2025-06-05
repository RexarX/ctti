#ifndef CTTI_DETAIL_TYPE_ID_IMPL_HPP
#define CTTI_DETAIL_TYPE_ID_IMPL_HPP

#include <ctti/detail/hash_impl.hpp>
#include <ctti/detail/name_impl.hpp>

#include <compare>
#include <functional>
#include <string_view>

namespace ctti::detail {

class TypeId {
public:
  constexpr TypeId() noexcept = default;
  constexpr explicit TypeId(std::string_view name) noexcept : name_(name), hash_(Fnv1aHash(name_)) {}

  constexpr HashType GetHash() const noexcept { return hash_; }
  constexpr std::string_view GetName() const noexcept { return name_; }

  constexpr auto operator<=>(const TypeId&) const noexcept = default;
  constexpr bool operator==(const TypeId&) const noexcept = default;

private:
  std::string_view name_ = "void";
  HashType hash_ = Fnv1aHash(name_);
};

class TypeIndex {
public:
  constexpr TypeIndex(HashType hash) noexcept : hash_(hash) {}
  constexpr TypeIndex(const TypeId& id) noexcept : hash_(id.GetHash()) {}

  constexpr HashType GetHash() const noexcept { return hash_; }

  constexpr auto operator<=>(const TypeIndex&) const noexcept = default;
  constexpr bool operator==(const TypeIndex&) const noexcept = default;

private:
  HashType hash_;
};

constexpr TypeIndex IdFromName(std::string_view type_name) noexcept {
  return Fnv1aHash(type_name);
}

template <typename T>
constexpr TypeId TypeIdOf() noexcept {
  return TypeId{NameOfImpl<T>::Apply()};
}

template <typename T>
constexpr TypeIndex TypeIndexOf() noexcept {
  return {IdFromName(NameOfImpl<T>::Apply())};
}

}  // namespace ctti::detail

namespace std {

template <>
struct hash<ctti::detail::TypeId> {
  constexpr std::size_t operator()(const ctti::detail::TypeId& id) const noexcept {
    return static_cast<std::size_t>(id.GetHash());
  }
};

template <>
struct hash<ctti::detail::TypeIndex> {
  constexpr std::size_t operator()(const ctti::detail::TypeIndex& id) const noexcept {
    return static_cast<std::size_t>(id.GetHash());
  }
};

}  // namespace std

#endif  // CTTI_DETAIL_TYPE_ID_IMPL_HPP
