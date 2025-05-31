#ifndef CTTI_TYPE_ID_HPP
#define CTTI_TYPE_ID_HPP

#include <ctti/detail/hash.hpp>
#include <ctti/nameof.hpp>

#include <compare>
#include <functional>
#include <string_view>

#ifdef CTTI_DEBUG_ID_FUNCTIONS
#include <iostream>
#include <string>

#ifndef CTTI_CONSTEXPR_ID
#define CTTI_CONSTEXPR_ID
#endif
#else
#ifndef CTTI_CONSTEXPR_ID
#define CTTI_CONSTEXPR_ID constexpr
#endif
#endif

namespace ctti {

struct type_id_t {
  constexpr type_id_t() noexcept = default;
  constexpr type_id_t(std::string_view name) noexcept : name_{name} {}

  constexpr detail::hash_t hash() const noexcept { return detail::fnv1a_hash(name_); }

  constexpr std::string_view name() const noexcept { return name_; }

  friend constexpr bool operator==(const type_id_t& lhs, const type_id_t& rhs) noexcept {
    return lhs.hash() == rhs.hash();
  }

  friend constexpr std::strong_ordering operator<=>(const type_id_t& lhs, const type_id_t& rhs) noexcept {
    return lhs.name() <=> rhs.name();
  }

private:
  std::string_view name_ = "void";
};

struct unnamed_type_id_t {
  constexpr unnamed_type_id_t(const detail::hash_t hash) noexcept : hash_{hash} {}
  constexpr unnamed_type_id_t(const type_id_t& id) noexcept : hash_{id.hash()} {}

  constexpr detail::hash_t hash() const noexcept { return hash_; }

  friend constexpr bool operator==(const unnamed_type_id_t& lhs, const unnamed_type_id_t& rhs) noexcept {
    return lhs.hash() == rhs.hash();
  }

  friend constexpr std::strong_ordering operator<=>(const unnamed_type_id_t& lhs,
                                                    const unnamed_type_id_t& rhs) noexcept {
    return lhs.hash() <=> rhs.hash();
  }

private:
  detail::hash_t hash_;
};

using type_index = unnamed_type_id_t;  // To mimic std::type_index when using maps

constexpr ctti::unnamed_type_id_t id_from_name(std::string_view type_name) noexcept {
  return detail::fnv1a_hash(type_name);
}

namespace detail {
template <typename T>
CTTI_CONSTEXPR_ID ctti::type_id_t type_id() noexcept {
  return {ctti::nameof<T>()};
}

template <typename T>
CTTI_CONSTEXPR_ID ctti::unnamed_type_id_t unnamed_type_id() noexcept {
  return {id_from_name(ctti::nameof<T>())};
}

}  // namespace detail

/**
 * @brief Returns type information at compile-time for a value
 * of type T. Decay is applied to argument type first, use
 * ctti::type_id<decltype(arg)>() to preserve references and cv qualifiers.
 */
template <typename T>
constexpr type_id_t type_id(T&&) noexcept {
  return detail::type_id<std::decay_t<T>>();
}

/** @brief Returns type information at compile-time for type T. */
template <typename T>
constexpr type_id_t type_id() noexcept {
  return detail::type_id<T>();
}

/**
 * @brief Returns unnamed type information at compile-time for a value
 * of type T. Decay is applied to argument type first, use
 * ctti::type_id<decltype(arg)>() to preserve references and cv qualifiers.
 */
template <typename T>
constexpr unnamed_type_id_t unnamed_type_id(T&&) noexcept {
  return detail::unnamed_type_id<std::decay_t<T>>();
}

/** @brief Returns unnamed type information at compile-time for type T. */
template <typename T>
constexpr unnamed_type_id_t unnamed_type_id() noexcept {
  return detail::unnamed_type_id<T>();
}

}  // namespace ctti

namespace std {

template <>
struct hash<ctti::type_id_t> {
  constexpr std::size_t operator()(const ctti::type_id_t& id) const noexcept {
    return static_cast<std::size_t>(id.hash());
  }
};

template <>
struct hash<ctti::unnamed_type_id_t> {
  constexpr std::size_t operator()(const ctti::unnamed_type_id_t& id) const noexcept {
    return static_cast<std::size_t>(id.hash());
  }
};

}  // namespace std

#endif  // CTTI_TYPE_ID_HPP
