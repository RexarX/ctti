#pragma once

#include <ctti/detail/concepts_impl.hpp>
#include <ctti/detail/meta.hpp>

namespace ctti {

/**
 * @brief Concept that checks if a type is reflectable.
 * @tparam T The type to check.
 */
template <typename T>
concept reflectable_type = detail::Reflectable<T>;

/**
 * @brief Concept that checks if a type has a custom name defined.
 * @tparam T The type to check.
 */
template <typename T>
concept has_custom_name_of = detail::HasCustomNameOf<T>;

/**
 * @brief Concept that checks if a type is a specialization of a given template.
 * @tparam T The type to check.
 * @tparam Template The template to check against.
 */
template <typename Symbol, typename Type>
concept symbol_member_of = detail::SymbolMemberOf<Symbol, Type>;

/**
 * @brief Concept that checks if a type is an integral constant type (e.g., std::integral_constant).
 * @tparam T The type to check.
 */
template <typename T>
concept integral_constant_type = detail::IntegralConstantType<T>;

}  // namespace ctti
