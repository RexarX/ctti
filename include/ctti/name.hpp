#pragma once

#include <ctti/detail/name_impl.hpp>

#include <cstddef>
#include <string_view>
#include <type_traits>

namespace ctti {

/**
 * @brief Represents a qualified name, which includes the base name and its qualifiers (namespaces or enclosing types).
 */
class qualified_name {
public:
  explicit constexpr qualified_name(detail::QualifiedName qualified_name) noexcept : qualified_name_(qualified_name) {}
  constexpr qualified_name(const qualified_name&) noexcept = default;
  constexpr qualified_name(qualified_name&&) noexcept = default;
  constexpr ~qualified_name() noexcept = default;

  constexpr qualified_name& operator=(const qualified_name&) noexcept = default;
  constexpr qualified_name& operator=(qualified_name&&) noexcept = default;

  /**
   * @brief Retrieves the base name (the unqualified name) of the type.
   * @return The base name as a string view.
   */
  [[nodiscard]] constexpr std::string_view get_name() const noexcept { return qualified_name_.GetName(); }

  /**
   * @brief Retrieves the full qualified name, including all qualifiers.
   * @return The full qualified name as a string view.
   */
  [[nodiscard]] constexpr std::string_view get_full_name() const noexcept { return qualified_name_.GetFullName(); }

  /**
   * @brief Retrieves the number of qualifiers in the qualified name.
   * @return The number of qualifiers.
   */
  [[nodiscard]] constexpr std::string_view get_qualifier(std::size_t index) const noexcept {
    return qualified_name_.GetQualifier(index);
  }

  constexpr bool operator==(const qualified_name& other) const noexcept {
    return qualified_name_ == other.qualified_name_;
  }

private:
  detail::QualifiedName qualified_name_;
};

/**
 * @brief Retrieves the name of a type T as a string view.
 * @tparam T The type for which to retrieve the name.
 * @return The name of the type T.
 */
template <typename T>
[[nodiscard]] constexpr std::string_view name_of() noexcept {
  return detail::NameOfImpl<T>::Apply();
}

/**
 * @brief Retrieves the name of the type of the given object as a string view.
 * @tparam T The type of the object.
 * @param obj The object whose type name is to be retrieved.
 * @return The name of the type of the object.
 */
template <typename T>
[[nodiscard]] constexpr std::string_view name_of(const T& /*obj*/) noexcept {
  return detail::NameOfImpl<std::remove_cvref_t<T>>::Apply();
}

/**
 * @brief Retrieves the name of a specific value of type T as a string view.
 * @tparam T The type of the value.
 * @tparam Value The specific value whose name is to be retrieved.
 * @return The name of the specific value.
 */
template <typename T, T Value>
[[nodiscard]] constexpr std::string_view name_of() noexcept {
  return detail::ValueNameOfImpl<T, Value>::Apply();
}

/**
 * @brief Retrieves the qualified name of a type T.
 * @tparam T The type for which to retrieve the qualified name.
 * @return The qualified name of the type T.
 */
template <typename T>
[[nodiscard]] constexpr qualified_name qualified_name_of() noexcept {
  return qualified_name(detail::QualifiedName(name_of<T>()));
}

/**
 * @brief Retrieves the qualified name of the type of the given object.
 * @tparam T The type of the object.
 * @param obj The object whose type qualified name is to be retrieved.
 * @return The qualified name of the type of the object.
 */
template <typename T>
[[nodiscard]] constexpr qualified_name qualified_name_of(const T& obj) noexcept {
  return qualified_name(detail::QualifiedName(name_of(obj)));
}

/**
 * @brief Retrieves the qualified name of a specific value of type T.
 * @tparam T The type of the value.
 * @tparam Value The specific value whose qualified name is to be retrieved.
 * @return The qualified name of the specific value.
 */
template <typename T, T Value>
[[nodiscard]] constexpr qualified_name qualified_name_of() noexcept {
  return qualified_name(detail::QualifiedName(name_of<T, Value>()));
}

}  // namespace ctti
