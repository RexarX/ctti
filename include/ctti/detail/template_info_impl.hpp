#ifndef CTTI_DETAIL_TEMPLATE_INFO_IMPL_HPP
#define CTTI_DETAIL_TEMPLATE_INFO_IMPL_HPP

#include <ctti/detail/meta.hpp>
#include <ctti/detail/name_impl.hpp>
#include <ctti/detail/static_value_impl.hpp>
#include <ctti/type_tag.hpp>

#include <concepts>
#include <string>
#include <string_view>
#include <type_traits>

namespace ctti::detail {

// Helper to detect if a type should be treated as a fundamental type for template detection
template <typename T>
constexpr bool IsFundamentalForTemplateDetection() {
  return std::is_fundamental_v<T> || std::same_as<T, std::string> || std::same_as<T, std::string_view> ||
         std::same_as<T, std::nullptr_t>;
}

// Primary template - non-template types
template <typename T>
struct TemplateInfo {
  using type = T;
  static constexpr bool kIsTemplateInstantiation = false;
  static constexpr std::size_t kParameterCount = 0;
  static constexpr std::size_t kTypeParameterCount = 0;
  static constexpr std::size_t kValueParameterCount = 0;

  static constexpr std::string_view GetName() noexcept { return NameOfImpl<T>::Apply(); }
};

// Variadic type-only templates (but exclude fundamental-like types)
template <template <typename...> class Template, typename... Args>
  requires(!IsFundamentalForTemplateDetection<Template<Args...>>())
struct TemplateInfo<Template<Args...>> {
  using type = Template<Args...>;
  static constexpr bool kIsTemplateInstantiation = true;
  static constexpr std::size_t kParameterCount = sizeof...(Args);
  static constexpr std::size_t kTypeParameterCount = sizeof...(Args);
  static constexpr std::size_t kValueParameterCount = 0;

  using parameters = TypeList<Args...>;
  using type_parameters = TypeList<Args...>;
  using value_parameters = TypeList<>;

  template <std::size_t I>
    requires(I < kParameterCount)
  using Parameter = PackGetType<I, Args...>;

  template <std::size_t I>
    requires(I < kTypeParameterCount)
  using TypeParameter = PackGetType<I, Args...>;

  static constexpr std::string_view GetName() noexcept { return NameOfImpl<Template<Args...>>::Apply(); }

  template <std::size_t I>
    requires(I < kTypeParameterCount)
  static constexpr type_tag<TypeParameter<I>> GetTypeParameterTag() noexcept {
    return {};
  }

  template <typename F>
  static constexpr void ForEachTypeParameter(F&& f) {
    (f(type_tag<Args>{}), ...);
  }

  template <typename F>
  static constexpr void ForEachParameter(F&& f) {
    ForEachTypeParameter(std::forward<F>(f));
  }

  static constexpr std::array<std::string_view, kTypeParameterCount> GetTypeParameterNames() noexcept {
    return {NameOfImpl<Args>::Apply()...};
  }

  static constexpr std::array<std::string_view, kParameterCount> GetParameterNames() noexcept {
    return GetTypeParameterNames();
  }
};

// Variadic value-only templates
template <template <auto...> class Template, auto... Args>
struct TemplateInfo<Template<Args...>> {
  using type = Template<Args...>;
  static constexpr bool kIsTemplateInstantiation = true;
  static constexpr std::size_t kParameterCount = sizeof...(Args);
  static constexpr std::size_t kTypeParameterCount = 0;
  static constexpr std::size_t kValueParameterCount = sizeof...(Args);

  using parameters = TypeList<StaticValue<decltype(Args), Args>...>;
  using type_parameters = TypeList<>;
  using value_parameters = TypeList<StaticValue<decltype(Args), Args>...>;

  static constexpr std::string_view GetName() noexcept { return NameOfImpl<Template<Args...>>::Apply(); }

  template <std::size_t I>
    requires(I < kValueParameterCount)
  static constexpr auto GetValueParameter() noexcept {
    return std::get<I>(std::make_tuple(Args...));
  }

  template <typename F>
  static constexpr void ForEachValueParameter(F&& f) {
    (f(StaticValue<decltype(Args), Args>{}), ...);
  }

  template <typename F>
  static constexpr void ForEachParameter(F&& f) {
    ForEachValueParameter(std::forward<F>(f));
  }
};

// Single type + single value parameter specialization
template <template <typename, auto> class Template, typename T, auto V>
struct TemplateInfo<Template<T, V>> {
  using type = Template<T, V>;
  static constexpr bool kIsTemplateInstantiation = true;
  static constexpr std::size_t kParameterCount = 2;
  static constexpr std::size_t kTypeParameterCount = 1;
  static constexpr std::size_t kValueParameterCount = 1;

  using type_parameter = T;
  static constexpr auto kValueParameter = V;

  template <std::size_t I>
    requires(I < kParameterCount)
  static constexpr auto GetParameter() noexcept {
    if constexpr (I == 0) {
      return type_tag<T>{};
    } else {
      return StaticValue<decltype(V), V>{};
    }
  }

  static constexpr std::string_view GetName() noexcept { return NameOfImpl<Template<T, V>>::Apply(); }
};

// Rest of the implementation remains the same...
template <template <typename...> class Template, typename T>
struct IsVariadicInstantiationOfHelper : std::false_type {};

template <template <typename...> class Template, typename... Args>
struct IsVariadicInstantiationOfHelper<Template, Template<Args...>> : std::true_type {};

template <template <auto...> class Template, typename T>
struct IsVariadicValueInstantiationOfHelper : std::false_type {};

template <template <auto...> class Template, auto... Args>
struct IsVariadicValueInstantiationOfHelper<Template, Template<Args...>> : std::true_type {};

template <template <typename...> class Template, typename T>
constexpr bool IsVariadicInstantiationOf() noexcept {
  return IsVariadicInstantiationOfHelper<Template, T>::value;
}

template <template <auto...> class Template, typename T>
constexpr bool IsVariadicValueInstantiationOf() noexcept {
  return IsVariadicValueInstantiationOfHelper<Template, T>::value;
}

template <typename T>
concept VariadicTypeTemplate = TemplateInfo<T>::kIsTemplateInstantiation && TemplateInfo<T>::kTypeParameterCount > 0 &&
                               TemplateInfo<T>::kValueParameterCount == 0;

template <typename T>
concept VariadicValueTemplate = TemplateInfo<T>::kIsTemplateInstantiation &&
                                TemplateInfo<T>::kTypeParameterCount == 0 && TemplateInfo<T>::kValueParameterCount > 0;

template <typename T>
concept MixedVariadicTemplate = TemplateInfo<T>::kIsTemplateInstantiation && TemplateInfo<T>::kTypeParameterCount > 0 &&
                                TemplateInfo<T>::kValueParameterCount > 0;

}  // namespace ctti::detail

#endif  // CTTI_DETAIL_TEMPLATE_INFO_IMPL_HPP
