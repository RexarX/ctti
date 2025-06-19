#pragma once

#include <ctti/detail/template_info_impl.hpp>
#include <ctti/type_tag.hpp>

#include <concepts>
#include <cstddef>
#include <string_view>
#include <type_traits>

namespace ctti {

template <typename T>
concept template_instantiation = detail::TemplateInfo<T>::kIsTemplateInstantiation;

template <typename T>
concept variadic_type_template = detail::VariadicTypeTemplate<T>;

template <typename T>
concept variadic_value_template = detail::VariadicValueTemplate<T>;

template <typename T>
concept mixed_variadic_template = detail::MixedVariadicTemplate<T>;

template <typename T>
class template_info {
private:
  using internal_info = detail::TemplateInfo<T>;

public:
  using type = T;

  static constexpr bool is_template_instantiation = internal_info::kIsTemplateInstantiation;
  static constexpr std::size_t parameter_count = internal_info::kParameterCount;
  static constexpr std::size_t type_parameter_count = internal_info::kTypeParameterCount;
  static constexpr std::size_t value_parameter_count = internal_info::kValueParameterCount;

  static constexpr std::string_view name() noexcept { return internal_info::GetName(); }
};

template <variadic_type_template T>
class template_info<T> {
private:
  using internal_info = detail::TemplateInfo<T>;

public:
  using type = T;

  static constexpr bool is_template_instantiation = true;
  static constexpr std::size_t parameter_count = internal_info::kParameterCount;
  static constexpr std::size_t type_parameter_count = internal_info::kTypeParameterCount;
  static constexpr std::size_t value_parameter_count = 0;

  template <std::size_t I>
    requires(I < type_parameter_count)
  using type_parameter = std::conditional_t<requires { typename internal_info::template TypeParameter<I>; },
                                            typename internal_info::template TypeParameter<I>, void>;

  template <std::size_t I>
    requires(I < parameter_count)
  using parameter = type_parameter<I>;

  static constexpr std::string_view name() noexcept { return internal_info::GetName(); }

  template <std::size_t I>
    requires(I < type_parameter_count && !std::same_as<type_parameter<I>, void>)
  static constexpr ctti::type_tag<type_parameter<I>> type_parameter_tag() noexcept {
    if constexpr (requires { internal_info::template GetTypeParameterTag<I>(); }) {
      return internal_info::template GetTypeParameterTag<I>();
    } else {
      return {};
    }
  }

  template <typename F>
    requires requires { internal_info::ForEachTypeParameter(std::declval<const F&>()); }
  static constexpr void for_each_type_parameter(const F& func) noexcept(
      noexcept(internal_info::ForEachTypeParameter(func))) {
    internal_info::ForEachTypeParameter(func);
  }

  template <typename F>
    requires requires { internal_info::ForEachTypeParameter(std::declval<const F&>()); }
  static constexpr void for_each_parameter(const F& func) noexcept(noexcept(for_each_type_parameter(func))) {
    for_each_type_parameter(func);
  }

  static constexpr auto type_parameter_names() noexcept {
    if constexpr (requires { internal_info::GetTypeParameterNames(); }) {
      return internal_info::GetTypeParameterNames();
    } else {
      return std::array<std::string_view, 0>{};
    }
  }

  static constexpr auto parameter_names() noexcept { return type_parameter_names(); }
};

template <variadic_value_template T>
class template_info<T> {
private:
  using internal_info = detail::TemplateInfo<T>;

public:
  using type = T;

  static constexpr bool is_template_instantiation = true;
  static constexpr std::size_t parameter_count = internal_info::kParameterCount;
  static constexpr std::size_t type_parameter_count = 0;
  static constexpr std::size_t value_parameter_count = internal_info::kValueParameterCount;

  static constexpr std::string_view name() noexcept { return internal_info::GetName(); }

  template <std::size_t I>
    requires(I < value_parameter_count)
  static constexpr auto value_parameter() noexcept {
    return internal_info::template GetValueParameter<I>();
  }

  template <typename F>
    requires requires { internal_info::ForEachValueParameter(std::declval<const F&>()); }
  static constexpr void for_each_value_parameter(const F& func) noexcept(
      noexcept(internal_info::ForEachValueParameter(func))) {
    internal_info::ForEachValueParameter(func);
  }

  template <typename F>
    requires requires { internal_info::ForEachValueParameter(std::declval<const F&>()); }
  static constexpr void for_each_parameter(const F& func) noexcept(noexcept(for_each_value_parameter(func))) {
    for_each_value_parameter(func);
  }
};

template <mixed_variadic_template T>
class template_info<T> {
private:
  using internal_info = detail::TemplateInfo<T>;

public:
  using type = T;
  using type_parameter = typename internal_info::type_parameter;

  static constexpr bool is_template_instantiation = true;
  static constexpr std::size_t parameter_count = internal_info::kParameterCount;
  static constexpr std::size_t type_parameter_count = internal_info::kTypeParameterCount;
  static constexpr std::size_t value_parameter_count = internal_info::kValueParameterCount;
  static constexpr auto value_parameter = internal_info::kValueParameter;

  template <std::size_t I>
    requires(I < parameter_count)
  static constexpr auto parameter() noexcept {
    return internal_info::template GetParameter<I>();
  }

  static constexpr std::string_view name() noexcept { return internal_info::GetName(); }
};

template <typename T>
constexpr auto get_template_info() noexcept {
  return template_info<T>{};
}

template <typename T>
constexpr bool is_template_instantiation() noexcept {
  return template_instantiation<T>;
}

template <typename T>
constexpr std::size_t template_parameter_count() noexcept {
  return template_info<T>::parameter_count;
}

template <typename T>
constexpr auto get_template_info([[maybe_unused]] const T& obj) noexcept {
  return template_info<std::remove_cvref_t<T>>{};
}

template <typename T>
constexpr bool is_template_instantiation([[maybe_unused]] const T& obj) noexcept {
  return template_instantiation<std::remove_cvref_t<T>>;
}

template <typename T>
constexpr std::size_t template_parameter_count([[maybe_unused]] const T& obj) noexcept {
  return template_info<std::remove_cvref_t<T>>::parameter_count;
}

template <typename T, std::size_t I>
  requires(I < template_info<T>::parameter_count && variadic_type_template<T>)
using template_parameter_t = typename template_info<T>::template parameter<I>;

template <typename T>
constexpr std::size_t template_param_count_v = template_info<T>::parameter_count;

template <typename T>
concept template_specialization = template_instantiation<T>;

}  // namespace ctti

