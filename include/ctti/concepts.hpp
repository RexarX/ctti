#pragma once

#include <ctti/detail/concepts_impl.hpp>
#include <ctti/detail/meta.hpp>
#include <ctti/model.hpp>

namespace ctti {

template <typename T>
concept reflectable_type = detail::Reflectable<T>;

template <typename T>
concept has_custom_name_of = detail::HasCustomNameOf<T>;

template <typename Symbol, typename Type>
concept symbol_member_of = detail::SymbolMemberOf<Symbol, Type>;

template <typename T>
concept integral_constant_type = detail::IntegralConstantType<T>;

}  // namespace ctti
