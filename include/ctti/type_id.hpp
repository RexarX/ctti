#pragma once

#include <ctti/detail/type_id_impl.hpp>

#include <cstddef>
#include <cstdint>
#include <string_view>
#include <type_traits>

namespace ctti {

class type_id {
public:
  constexpr type_id() noexcept = default;
  explicit constexpr type_id(std::string_view name) noexcept : impl_(name) {}
  explicit constexpr type_id(const detail::TypeId& impl) noexcept : impl_(impl) {}
  constexpr type_id(const type_id&) noexcept = default;
  constexpr type_id(type_id&&) noexcept = default;
  constexpr ~type_id() noexcept = default;

  constexpr type_id& operator=(const type_id&) noexcept = default;
  constexpr type_id& operator=(type_id&&) noexcept = default;

  constexpr std::uint64_t hash() const noexcept { return impl_.GetHash(); }
  constexpr std::string_view name() const noexcept { return impl_.GetName(); }

  constexpr auto operator<=>(const type_id&) const noexcept = default;
  constexpr bool operator==(const type_id&) const noexcept = default;

private:
  detail::TypeId impl_;
};

class type_index {
public:
  explicit constexpr type_index(std::uint64_t hash) noexcept : impl_(hash) {}
  explicit constexpr type_index(const type_id& id) noexcept : impl_(id.hash()) {}
  explicit constexpr type_index(const detail::TypeIndex& impl) noexcept : impl_(impl) {}
  constexpr type_index(const type_index&) noexcept = default;
  constexpr type_index(type_index&&) noexcept = default;
  constexpr ~type_index() noexcept = default;

  constexpr type_index& operator=(const type_index&) noexcept = default;
  constexpr type_index& operator=(type_index&&) noexcept = default;

  constexpr std::uint64_t hash() const noexcept { return impl_.GetHash(); }

  constexpr auto operator<=>(const type_index&) const noexcept = default;
  constexpr bool operator==(const type_index&) const noexcept = default;

private:
  detail::TypeIndex impl_;
};

constexpr type_index id_from_name(std::string_view type_name) noexcept {
  return type_index(detail::IdFromName(type_name));
}

template <typename T>
constexpr type_id type_id_of([[maybe_unused]] const T& obj) noexcept {
  return type_id(detail::TypeIdOf<std::decay_t<T>>());
}

template <typename T>
constexpr type_id type_id_of() noexcept {
  return type_id(detail::TypeIdOf<T>());
}

template <typename T>
constexpr type_index type_index_of([[maybe_unused]] const T& obj) noexcept {
  return type_index(detail::TypeIndexOf<std::decay_t<T>>());
}

template <typename T>
constexpr type_index type_index_of() noexcept {
  return type_index(detail::TypeIndexOf<T>());
}

}  // namespace ctti

namespace std {

template <>
struct hash<ctti::type_id> {
  constexpr std::size_t operator()(const ctti::type_id& id) const noexcept {
    return static_cast<std::size_t>(id.hash());
  }
};

template <>
struct hash<ctti::type_index> {
  constexpr std::size_t operator()(const ctti::type_index& id) const noexcept {
    return static_cast<std::size_t>(id.hash());
  }
};

}  // namespace std
