#pragma once

#include <ctti/type_tag.hpp>

#include <concepts>
#include <string_view>

namespace ctti::detail {

template <typename T>
concept HasCustomNameOf = requires {
  { T::ctti_name_of() } -> std::convertible_to<std::string_view>;
};

template <typename T>
[[nodiscard]] consteval bool HasAdlModel() noexcept {
  if constexpr (requires { ctti_model(ctti::type_tag<T>{}); }) {
    using model_type = decltype(ctti_model(ctti::type_tag<T>{}));
    if constexpr (requires { model_type::size; }) {
      return model_type::size > 0;
    }
  }
  return false;
}

template <typename T>
[[nodiscard]] consteval bool HasIntrusiveModel() noexcept {
  if constexpr (requires { typename T::ctti_model; }) {
    using model_type = typename T::ctti_model;
    if constexpr (requires { model_type::size; }) {
      return model_type::size > 0;
    }
  }
  return false;
}

template <typename T>
concept Reflectable = HasIntrusiveModel<T>() || HasAdlModel<T>();

template <typename Symbol, typename Type>
concept SymbolMemberOf = requires {
  { Symbol::template is_owner_of<Type>() } -> std::convertible_to<bool>;
} && Symbol::template is_owner_of<Type>();

}  // namespace ctti::detail
