#ifndef CTTI_DETAIL_CONCEPTS_IMPL_HPP
#define CTTI_DETAIL_CONCEPTS_IMPL_HPP

#include <ctti/type_tag.hpp>

#include <concepts>
#include <string>
#include <string_view>
#include <type_traits>

namespace ctti::detail {

template <typename T>
constexpr bool HasAdlModel() {
  if constexpr (requires { ctti_model(type_tag<T>{}); }) {
    using model_type = decltype(ctti_model(type_tag<T>{}));
    if constexpr (requires { typename model_type::symbols; }) {
      return model_type::symbols::kSize > 0;
    }
    return false;
  }
  return false;
}

template <typename T>
constexpr bool HasIntrusiveModel() {
  if constexpr (requires { typename T::ctti_model; }) {
    using model_type = typename T::ctti_model;
    if constexpr (requires { typename model_type::symbols; }) {
      return model_type::symbols::kSize > 0;
    }
    return false;
  }
  return false;
}

template <typename T>
concept Reflectable = HasIntrusiveModel<T>() || (HasAdlModel<T>() && !std::is_fundamental_v<T> &&
                                                 !std::same_as<T, std::string> && !std::same_as<T, std::string_view>);

template <typename T>
concept HasCustomNameOf = requires {
  { T::ctti_name_of() } -> std::convertible_to<std::string_view>;
};

template <typename T>
concept EnumType = std::is_enum_v<T>;

template <typename Symbol, typename Type>
concept SymbolMemberOf = Symbol::template is_owner_of<Type>();

template <typename T>
concept IntegralConstantType = requires {
  typename T::value_type;
  { T::value } -> std::convertible_to<typename T::value_type>;
};

}  // namespace ctti::detail

#endif  // CTTI_DETAIL_CONCEPTS_IMPL_HPP
