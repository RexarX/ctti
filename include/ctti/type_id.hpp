#ifndef CTTI_TYPE_ID_HPP
#define CTTI_TYPE_ID_HPP

#include <ctti/detail/type_id_impl.hpp>

namespace ctti {

struct type_id {
  constexpr type_id() noexcept = default;
  constexpr explicit type_id(std::string_view name) noexcept : impl_(name) {}
  constexpr type_id(const detail::TypeId& impl) noexcept : impl_(impl) {}

  constexpr std::uint64_t hash() const noexcept { return impl_.GetHash(); }
  constexpr std::string_view name() const noexcept { return impl_.GetName(); }

  constexpr auto operator<=>(const type_id&) const noexcept = default;
  constexpr bool operator==(const type_id&) const noexcept = default;

private:
  detail::TypeId impl_;
};

struct type_index {
  constexpr type_index(std::uint64_t hash) noexcept : impl_(hash) {}
  constexpr type_index(const type_id& id) noexcept : impl_(id.hash()) {}
  constexpr type_index(const detail::TypeIndex& impl) noexcept : impl_(impl) {}

  constexpr std::uint64_t hash() const noexcept { return impl_.GetHash(); }

  constexpr auto operator<=>(const type_index&) const noexcept = default;
  constexpr bool operator==(const type_index&) const noexcept = default;

private:
  detail::TypeIndex impl_;
};

constexpr type_index id_from_name(std::string_view type_name) noexcept {
  return detail::IdFromName(type_name);
}

template <typename T>
constexpr type_id type_id_of(T&&) noexcept {
  return detail::TypeIdOf<std::decay_t<T>>();
}

template <typename T>
constexpr type_id type_id_of() noexcept {
  return detail::TypeIdOf<T>();
}

template <typename T>
constexpr type_index type_index_of(T&&) noexcept {
  return detail::TypeIndexOf<std::decay_t<T>>();
}

template <typename T>
constexpr type_index type_index_of() noexcept {
  return detail::TypeIndexOf<T>();
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

#endif  // CTTI_TYPE_ID_HPP
