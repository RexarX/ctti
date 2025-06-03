#ifndef CTTI_TEMPLATE_INFO_HPP
#define CTTI_TEMPLATE_INFO_HPP

#include <ctti/detail/template_info_impl.hpp>
#include <ctti/type_tag.hpp>

namespace ctti {

template <typename T>
concept template_specialization = detail::TemplateSpecialization<T>;

template <typename T>
concept template_instantiation = detail::TemplateInstantiation<T>;

// Base template for non-template types
template <typename T>
struct template_info {
  using type = T;
  static constexpr bool is_template_instantiation = detail::TemplateInfo<T>::kIsTemplateInstantiation;
  static constexpr std::size_t parameter_count = detail::TemplateInfo<T>::kParameterCount;

  static constexpr std::string_view name() noexcept { return detail::TemplateInfo<T>::GetName(); }
};

// Specialization for regular template instantiations (template <typename...>)
template <template <typename...> class Template, typename... Args>
struct template_info<Template<Args...>> {
  using type = Template<Args...>;
  static constexpr bool is_template_instantiation = true;
  static constexpr std::size_t parameter_count = sizeof...(Args);

  static constexpr std::string_view name() noexcept { return detail::TemplateInfo<Template<Args...>>::GetName(); }

  template <std::size_t I>
    requires(I < parameter_count)
  using parameter = typename detail::TemplateInfo<Template<Args...>>::template Parameter<I>;

  template <std::size_t I>
    requires(I < parameter_count)
  static constexpr type_tag<parameter<I>> parameter_tag() noexcept {
    return detail::TemplateInfo<Template<Args...>>::template GetParameterTag<I>();
  }

  template <typename F>
  static constexpr void for_each_parameter(F&& f) {
    detail::TemplateInfo<Template<Args...>>::ForEachParameter(std::forward<F>(f));
  }

  static constexpr std::array<std::string_view, parameter_count> parameter_names() noexcept {
    return detail::TemplateInfo<Template<Args...>>::GetParameterNames();
  }
};

// Specialization for non-type template parameters (template <auto...>)
template <template <auto...> class Template, auto... Args>
struct template_info<Template<Args...>> {
  using type = Template<Args...>;
  static constexpr bool is_template_instantiation = true;
  static constexpr std::size_t parameter_count = sizeof...(Args);

  static constexpr std::string_view name() noexcept { return detail::TemplateInfo<Template<Args...>>::GetName(); }

  template <std::size_t I>
    requires(I < parameter_count)
  static constexpr auto parameter_value() noexcept {
    return detail::TemplateInfo<Template<Args...>>::template GetParameterValue<I>();
  }
};

// Specialization for mixed template parameters (template <typename, auto>)
template <template <typename, auto> class Template, typename T, auto Value>
struct template_info<Template<T, Value>> {
  using type = Template<T, Value>;
  static constexpr bool is_template_instantiation = true;
  static constexpr std::size_t parameter_count = 2;

  static constexpr std::string_view name() noexcept { return detail::TemplateInfo<Template<T, Value>>::GetName(); }

  using type_parameter = T;
  static constexpr auto value_parameter = Value;

  template <std::size_t I>
    requires(I < parameter_count)
  static constexpr auto parameter() noexcept {
    return detail::TemplateInfo<Template<T, Value>>::template GetParameter<I>();
  }
};

template <typename T>
constexpr template_info<T> get_template_info() noexcept {
  return {};
}

template <typename T>
constexpr template_info<T> get_template_info(type_tag<T>) noexcept {
  return {};
}

template <typename T>
constexpr bool is_template_instantiation() noexcept {
  return detail::IsTemplateInstantiation<T>();
}

template <typename T>
constexpr std::size_t template_parameter_count() noexcept {
  return detail::TemplateParameterCount<T>();
}

template <typename T, std::size_t I>
  requires template_instantiation<T> && (I < template_info<T>::parameter_count)
using template_parameter_t = detail::TemplateParameterT<T, I>;

template <template <typename...> class Template, typename T>
constexpr bool is_instantiation_of() noexcept {
  return detail::IsInstantiationOf<Template, T>();
}

template <template <typename...> class Template, typename T>
concept instantiation_of = detail::InstantiationOf<Template, T>;

}  // namespace ctti

#endif  // CTTI_TEMPLATE_INFO_HPP
