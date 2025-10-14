#pragma once

#include <ctti/detail/type_id_impl.hpp>

#include <cstddef>
#include <functional>
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

  constexpr auto operator<=>(const type_id&) const noexcept = default;
  constexpr bool operator==(const type_id&) const noexcept = default;

  /**
   * @brief Returns the hash of the type.
   * @return The hash of the type.
   */
  [[nodiscard]] constexpr std::size_t hash() const noexcept { return impl_.GetHash(); }

  /**
   * @brief Returns the name of the type.
   * @return The name of the type.
   */
  [[nodiscard]] constexpr std::string_view name() const noexcept { return impl_.GetName(); }

private:
  detail::TypeId impl_;
};

class type_index {
public:
  explicit constexpr type_index(std::size_t hash) noexcept : impl_(hash) {}
  explicit constexpr type_index(const type_id& id) noexcept : impl_(id.hash()) {}
  explicit constexpr type_index(const detail::TypeIndex& impl) noexcept : impl_(impl) {}
  constexpr type_index(const type_index&) noexcept = default;
  constexpr type_index(type_index&&) noexcept = default;
  constexpr ~type_index() noexcept = default;

  constexpr type_index& operator=(const type_index&) noexcept = default;
  constexpr type_index& operator=(type_index&&) noexcept = default;

  constexpr auto operator<=>(const type_index&) const noexcept = default;
  constexpr bool operator==(const type_index&) const noexcept = default;

  /**
   * @brief Returns the hash of the type.
   * @return The hash of the type.
   */
  [[nodiscard]] constexpr std::size_t hash() const noexcept { return impl_.GetHash(); }

private:
  detail::TypeIndex impl_;
};

/**
 * @brief Gets a type_index from a type name.
 * @param type_name The name of the type.
 * @return The type_index of the type.
 */
[[nodiscard]] constexpr type_index id_from_name(std::string_view type_name) noexcept {
  return type_index(detail::IdFromName(type_name));
}

/**
 * @brief Gets a type_id from a type name.
 * @param type_name The name of the type.
 * @return The type_id of the type.
 */
template <typename T>
[[nodiscard]] constexpr type_id type_id_of(const T& /*obj*/) noexcept {
  return type_id(detail::TypeIdOf<std::decay_t<T>>());
}

/**
 * @brief Gets a type_id from a type.
 * @tparam T The type to get the type_id of.
 * @return The type_id of the type.
 */
template <typename T>
[[nodiscard]] constexpr type_id type_id_of() noexcept {
  return type_id(detail::TypeIdOf<T>());
}

/**
 * @brief Gets a type_index from a type.
 * @tparam T The type to get the type_index of.
 * @return The type_index of the type.
 */
template <typename T>
[[nodiscard]] constexpr type_index type_index_of(const T& /*obj*/) noexcept {
  return type_index(detail::TypeIndexOf<std::decay_t<T>>());
}

/**
 * @brief Gets a type_index from a type.
 * @tparam T The type to get the type_index of.
 * @return The type_index of the type.
 */
template <typename T>
[[nodiscard]] constexpr type_index type_index_of() noexcept {
  return type_index(detail::TypeIndexOf<T>());
}

}  // namespace ctti

template <>
struct std::hash<ctti::type_id> {
  constexpr std::size_t operator()(const ctti::type_id& id) const noexcept { return id.hash(); }
};

template <>
struct std::hash<ctti::type_index> {
  constexpr std::size_t operator()(const ctti::type_index& id) const noexcept { return id.hash(); }
};
