#ifndef CTTI_TEMPLATE_INFO_HPP
#define CTTI_TEMPLATE_INFO_HPP

#include <ctti/detail/template_info_impl.hpp>
#include <ctti/type_tag.hpp>

namespace ctti {

template <typename T>
concept template_instantiation = detail::TemplateInfo<T>::kIsTemplateInstantiation;

template <typename T>
concept variadic_type_template = detail::VariadicTypeTemplate<T>;

template <typename T>
concept variadic_value_template = detail::VariadicValueTemplate<T>;

template <typename T>
concept mixed_variadic_template = detail::MixedVariadicTemplate<T>;

// Primary template_info
template <typename T>
struct template_info {
  using type = T;
  static constexpr bool is_template_instantiation = detail::TemplateInfo<T>::kIsTemplateInstantiation;
  static constexpr std::size_t parameter_count = detail::TemplateInfo<T>::kParameterCount;
  static constexpr std::size_t type_parameter_count = detail::TemplateInfo<T>::kTypeParameterCount;
  static constexpr std::size_t value_parameter_count = detail::TemplateInfo<T>::kValueParameterCount;

  static constexpr std::string_view name() noexcept { return detail::TemplateInfo<T>::GetName(); }
};

// Specialization for variadic type templates
template <variadic_type_template T>
struct template_info<T> {
  using type = T;
  static constexpr bool is_template_instantiation = true;
  static constexpr std::size_t parameter_count = detail::TemplateInfo<T>::kParameterCount;
  static constexpr std::size_t type_parameter_count = detail::TemplateInfo<T>::kTypeParameterCount;
  static constexpr std::size_t value_parameter_count = 0;

  template <std::size_t I>
    requires(I < type_parameter_count)
  using type_parameter = typename detail::TemplateInfo<T>::template TypeParameter<I>;

  template <std::size_t I>
    requires(I < parameter_count)
  using parameter = type_parameter<I>;

  static constexpr std::string_view name() noexcept { return detail::TemplateInfo<T>::GetName(); }

  template <std::size_t I>
    requires(I < type_parameter_count)
  static constexpr type_tag<type_parameter<I>> type_parameter_tag() noexcept {
    return detail::TemplateInfo<T>::template GetTypeParameterTag<I>();
  }

  template <typename F>
  static constexpr void for_each_type_parameter(F&& f) {
    detail::TemplateInfo<T>::ForEachTypeParameter(std::forward<F>(f));
  }

  template <typename F>
  static constexpr void for_each_parameter(F&& f) {
    for_each_type_parameter(std::forward<F>(f));
  }

  static constexpr std::array<std::string_view, type_parameter_count> type_parameter_names() noexcept {
    return detail::TemplateInfo<T>::GetTypeParameterNames();
  }

  static constexpr std::array<std::string_view, parameter_count> parameter_names() noexcept {
    return type_parameter_names();
  }
};

// Specialization for variadic value templates
template <variadic_value_template T>
struct template_info<T> {
  using type = T;
  static constexpr bool is_template_instantiation = true;
  static constexpr std::size_t parameter_count = detail::TemplateInfo<T>::kParameterCount;
  static constexpr std::size_t type_parameter_count = 0;
  static constexpr std::size_t value_parameter_count = detail::TemplateInfo<T>::kValueParameterCount;

  static constexpr std::string_view name() noexcept { return detail::TemplateInfo<T>::GetName(); }

  template <std::size_t I>
    requires(I < value_parameter_count)
  static constexpr auto value_parameter() noexcept {
    return detail::TemplateInfo<T>::template GetValueParameter<I>();
  }

  template <typename F>
  static constexpr void for_each_value_parameter(F&& f) {
    detail::TemplateInfo<T>::ForEachValueParameter(std::forward<F>(f));
  }

  template <typename F>
  static constexpr void for_each_parameter(F&& f) {
    for_each_value_parameter(std::forward<F>(f));
  }
};

// Specialization for mixed templates
template <mixed_variadic_template T>
struct template_info<T> {
  using type = T;
  static constexpr bool is_template_instantiation = true;
  static constexpr std::size_t parameter_count = detail::TemplateInfo<T>::kParameterCount;
  static constexpr std::size_t type_parameter_count = detail::TemplateInfo<T>::kTypeParameterCount;
  static constexpr std::size_t value_parameter_count = detail::TemplateInfo<T>::kValueParameterCount;

  using type_parameter = typename detail::TemplateInfo<T>::type_parameter;
  static constexpr auto value_parameter = detail::TemplateInfo<T>::kValueParameter;

  template <std::size_t I>
    requires(I < parameter_count)
  static constexpr auto parameter() noexcept {
    return detail::TemplateInfo<T>::template GetParameter<I>();
  }

  static constexpr std::string_view name() noexcept { return detail::TemplateInfo<T>::GetName(); }
};

// Utility functions
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

template <typename T, std::size_t I>
  requires(I < template_info<T>::parameter_count && variadic_type_template<T>)
using template_parameter_t = typename template_info<T>::template parameter<I>;

template <template <typename...> class Template, typename T>
constexpr bool is_variadic_instantiation_of() noexcept {
  return detail::IsVariadicInstantiationOf<Template, T>();
}

template <template <auto...> class Template, typename T>
constexpr bool is_variadic_value_instantiation_of() noexcept {
  return detail::IsVariadicValueInstantiationOf<Template, T>();
}

template <template <typename...> class Template, typename T>
concept variadic_instantiation_of = detail::IsVariadicInstantiationOfHelper<Template, T>::value;

template <template <auto...> class Template, typename T>
concept variadic_value_instantiation_of = detail::IsVariadicValueInstantiationOfHelper<Template, T>::value;

// Simple instantiation checking (for any template)
template <typename T>
concept template_specialization = template_instantiation<T>;

}  // namespace ctti

#endif  // CTTI_TEMPLATE_INFO_HPP
