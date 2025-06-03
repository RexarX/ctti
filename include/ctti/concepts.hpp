#ifndef CTTI_CONCEPTS_HPP
#define CTTI_CONCEPTS_HPP

#include <ctti/detail/concepts_impl.hpp>

namespace ctti {

template <typename T>
concept reflectable = detail::Reflectable<T>;

template <typename T>
concept has_custom_name_of = detail::HasCustomNameOf<T>;

template <typename T>
concept enum_type = detail::EnumType<T>;

template <typename Symbol, typename Type>
concept symbol_member_of = detail::SymbolMemberOf<Symbol, Type>;

template <typename T>
concept integral_constant_type = detail::IntegralConstantType<T>;

}  // namespace ctti

#endif  // CTTI_CONCEPTS_HPP
