#pragma once

#include <ctti/detail/hash_impl.hpp>
#include <ctti/detail/name_impl.hpp>

#include <cstddef>
#include <functional>
#include <string_view>

namespace ctti::detail {

class TypeId {
public:
  constexpr TypeId() noexcept = default;
  constexpr explicit TypeId(std::string_view name) noexcept : name_(name), hash_(Fnv1aHash(name_)) {}
  constexpr TypeId(const TypeId&) noexcept = default;
  constexpr TypeId(TypeId&&) noexcept = default;
  constexpr ~TypeId() noexcept = default;

  constexpr TypeId& operator=(const TypeId&) noexcept = default;
  constexpr TypeId& operator=(TypeId&&) noexcept = default;

  constexpr auto operator<=>(const TypeId&) const noexcept = default;
  constexpr bool operator==(const TypeId&) const noexcept = default;

  constexpr HashType GetHash() const noexcept { return hash_; }
  constexpr std::string_view GetName() const noexcept { return name_; }

private:
  std::string_view name_ = "void";
  HashType hash_ = Fnv1aHash(name_);
};

class TypeIndex {
public:
  explicit constexpr TypeIndex(HashType hash) noexcept : hash_(hash) {}
  explicit constexpr TypeIndex(const TypeId& id) noexcept : hash_(id.GetHash()) {}
  constexpr TypeIndex(const TypeIndex&) noexcept = default;
  constexpr TypeIndex(TypeIndex&&) noexcept = default;
  constexpr ~TypeIndex() noexcept = default;

  constexpr TypeIndex& operator=(const TypeIndex&) noexcept = default;
  constexpr TypeIndex& operator=(TypeIndex&&) noexcept = default;

  constexpr auto operator<=>(const TypeIndex&) const noexcept = default;
  constexpr bool operator==(const TypeIndex&) const noexcept = default;

  constexpr HashType GetHash() const noexcept { return hash_; }

private:
  HashType hash_;
};

constexpr TypeIndex IdFromName(std::string_view type_name) noexcept {
  return TypeIndex{Fnv1aHash(type_name)};
}

template <typename T>
constexpr TypeId TypeIdOf() noexcept {
  constexpr auto type_name = NameOfImpl<T>::Apply();

  static_assert(type_name != "invalid_signature", "Failed to extract type name");
  static_assert(type_name != "unsupported_compiler", "Compiler not supported for type name extraction");
  static_assert(!type_name.empty(), "Empty type name extracted");

  return TypeId{type_name};
}

template <typename T>
constexpr TypeIndex TypeIndexOf() noexcept {
  constexpr auto type_name = NameOfImpl<T>::Apply();
  return TypeIndex{Fnv1aHash(type_name)};
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
