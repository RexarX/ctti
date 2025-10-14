#pragma once

#include <ctti/detail/template_info_impl.hpp>
#include <ctti/type_tag.hpp>

#include <concepts>
#include <cstddef>
#include <string_view>
#include <type_traits>

namespace ctti {

/**
 * @brief Concept to check if a type is a template instantiation.
 * @tparam T The type to check.
 */
template <typename T>
concept template_instantiation = detail::TemplateInfo<T>::kIsTemplateInstantiation;

/**
 * @brief Concept to check if a type is a variadic type template.
 * @tparam T The type to check.
 */
template <typename T>
concept variadic_type_template = detail::VariadicTypeTemplate<T>;

/**
 * @brief Concept to check if a type is a variadic value template.
 * @tparam T The type to check.
 */
template <typename T>
concept variadic_value_template = detail::VariadicValueTemplate<T>;

/**
 * @brief Concept to check if a type is a mixed variadic template (both type and value parameters).
 * @tparam T The type to check.
 */
template <typename T>
concept mixed_variadic_template = detail::MixedVariadicTemplate<T>;

/**
 * @brief Provides compile-time information about a template type.
 * @tparam T The template type to analyze.
 */
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

  /**
   * @brief Retrieves the name of the template type.
   * @return The name of the template type as a string view.
   */
  [[nodiscard]] static constexpr std::string_view name() noexcept { return internal_info::GetName(); }
};

/**
 * @brief Specialization of template_info for variadic type templates.
 * @tparam T The variadic type template to analyze.
 */
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

  /**
   * @brief Retrieves the name of the template type.
   * @return The name of the template type as a string view.
   */
  [[nodiscard]] static constexpr std::string_view name() noexcept { return internal_info::GetName(); }

  /**
   * @brief Retrieves the type tag for the I-th type parameter.
   * @tparam I The index of the type parameter.
   * @return The type tag of the I-th type parameter.
   */
  template <std::size_t I>
    requires(I < type_parameter_count && !std::same_as<type_parameter<I>, void>)
  [[nodiscard]] static constexpr auto type_parameter_tag() noexcept -> ctti::type_tag<type_parameter<I>> {
    if constexpr (requires { internal_info::template GetTypeParameterTag<I>(); }) {
      return internal_info::template GetTypeParameterTag<I>();
    } else {
      return {};
    }
  }

  /**
   * @brief Applies a function to each type parameter of the template.
   * @tparam F The function type to apply.
   * @param func The function to apply to each type parameter.
   */
  template <typename F>
    requires requires { internal_info::ForEachTypeParameter(std::declval<const F&>()); }
  static constexpr void for_each_type_parameter(const F& func) noexcept(
      noexcept(internal_info::ForEachTypeParameter(func))) {
    internal_info::ForEachTypeParameter(func);
  }

  /**
   * @brief Applies a function to each parameter (type parameters) of the template.
   * @tparam F The function type to apply.
   * @param func The function to apply to each parameter.
   */
  template <typename F>
    requires requires { internal_info::ForEachTypeParameter(std::declval<const F&>()); }
  static constexpr void for_each_parameter(const F& func) noexcept(noexcept(for_each_type_parameter(func))) {
    for_each_type_parameter(func);
  }

  /**
   * @brief Retrieves the names of the type parameters of the template.
   * @return An array of string views representing the names of the type parameters.
   */
  [[nodiscard]] static constexpr auto type_parameter_names() noexcept {
    if constexpr (requires { internal_info::GetTypeParameterNames(); }) {
      return internal_info::GetTypeParameterNames();
    } else {
      return std::array<std::string_view, 0>{};
    }
  }

  /**
   * @brief Retrieves the names of the parameters (type parameters) of the template.
   * @return An array of string views representing the names of the parameters.
   */
  [[nodiscard]] static constexpr auto parameter_names() noexcept { return type_parameter_names(); }
};

/**
 * @brief Specialization of template_info for variadic value templates.
 * @tparam T The variadic value template to analyze.
 */
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

  /**
   * @brief Retrieves the name of the template type.
   * @return The name of the template type as a string view.
   */
  [[nodiscard]] static constexpr std::string_view name() noexcept { return internal_info::GetName(); }

  /**
   * @brief Retrieves the value of the I-th value parameter.
   * @tparam I The index of the value parameter.
   * @return The value of the I-th value parameter.
   */
  template <std::size_t I>
    requires(I < value_parameter_count)
  [[nodiscard]] static constexpr auto value_parameter() noexcept {
    return internal_info::template GetValueParameter<I>();
  }

  /**
   * @brief Applies a function to each value parameter of the template.
   * @tparam F The function type to apply.
   * @param func The function to apply to each value parameter.
   */
  template <typename F>
    requires requires { internal_info::ForEachValueParameter(std::declval<const F&>()); }
  static constexpr void for_each_value_parameter(const F& func) noexcept(
      noexcept(internal_info::ForEachValueParameter(func))) {
    internal_info::ForEachValueParameter(func);
  }

  /**
   * @brief Applies a function to each parameter (value parameters) of the template.
   * @tparam F The function type to apply.
   * @param func The function to apply to each parameter.
   */
  template <typename F>
    requires requires { internal_info::ForEachValueParameter(std::declval<const F&>()); }
  static constexpr void for_each_parameter(const F& func) noexcept(noexcept(for_each_value_parameter(func))) {
    for_each_value_parameter(func);
  }
};

/**
 * @brief Specialization of template_info for mixed variadic templates (both type and value parameters).
 * @tparam T The mixed variadic template to analyze.
 */
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

  /**
   * @brief Retrieves the type tag for the I-th type parameter.
   * @tparam I The index of the type parameter.
   * @return The type tag of the I-th type parameter.
   */
  template <std::size_t I>
    requires(I < parameter_count)
  [[nodiscard]] static constexpr auto parameter() noexcept {
    return internal_info::template GetParameter<I>();
  }

  /**
   * @brief Retrieves the name of the template type.
   * @return The name of the template type as a string view.
   */
  static constexpr std::string_view name() noexcept { return internal_info::GetName(); }
};

/**
 * @brief Retrieves the template_info for the specified type T.
 * @tparam T The type to analyze.
 * @return The template_info instance for type T.
 */
template <typename T>
[[nodiscard]] constexpr auto get_template_info() noexcept -> template_info<T> {
  return template_info<T>{};
}

/**
 * @brief Checks if the specified type T is a template instantiation.
 * @tparam T The type to check.
 * @return true if T is a template instantiation, false otherwise.
 */
template <typename T>
[[nodiscard]] constexpr bool is_template_instantiation() noexcept {
  return template_instantiation<T>;
}

/**
 * @brief Retrieves the number of template parameters for the specified type T.
 * @tparam T The type to analyze.
 * @return The number of template parameters for type T.
 */
template <typename T>
[[nodiscard]] constexpr std::size_t template_parameter_count() noexcept {
  return template_info<T>::parameter_count;
}

/**
 * @brief Retrieves the template_info for the specified object instance.
 * @tparam T The type of the object to analyze.
 * @param obj The object instance (not used, only for type deduction).
 * @return The template_info instance for the type of obj.
 */
template <typename T>
[[nodiscard]] constexpr auto get_template_info(const T& /*obj*/) noexcept -> template_info<std::remove_cvref_t<T>> {
  return template_info<std::remove_cvref_t<T>>{};
}

/**
 * @brief Checks if the type of the specified object instance is a template instantiation.
 * @tparam T The type of the object to check.
 * @param obj The object instance (not used, only for type deduction).
 * @return true if the type of obj is a template instantiation, false otherwise.
 */
template <typename T>
[[nodiscard]] constexpr bool is_template_instantiation(const T& /*obj*/) noexcept {
  return template_instantiation<std::remove_cvref_t<T>>;
}

/**
 * @brief Retrieves the number of template parameters for the type of the specified object instance.
 * @tparam T The type of the object to analyze.
 * @param obj The object instance (not used, only for type deduction).
 * @return The number of template parameters for the type of obj.
 */
template <typename T>
[[nodiscard]] constexpr std::size_t template_parameter_count(const T& /*obj*/) noexcept {
  return template_info<std::remove_cvref_t<T>>::parameter_count;
}

template <typename T, std::size_t I>
  requires(I < template_info<T>::parameter_count && variadic_type_template<T>)
using template_parameter_t = typename template_info<T>::template parameter<I>;

/**
 * @brief Alias for retrieving the I-th type parameter of a variadic type template.
 * @tparam T The variadic type template.
 * @tparam I The index of the type parameter.
 */
template <typename T>
constexpr std::size_t template_param_count_v = template_info<T>::parameter_count;

/**
 * @brief Concept to check if a type is a specialization of a template.
 * @tparam T The type to check.
 */
template <typename T>
concept template_specialization = template_instantiation<T>;

}  // namespace ctti
