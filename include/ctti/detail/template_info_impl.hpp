#ifndef CTTI_DETAIL_TEMPLATE_INFO_IMPL_HPP
#define CTTI_DETAIL_TEMPLATE_INFO_IMPL_HPP

#include <ctti/detail/meta.hpp>
#include <ctti/detail/name_impl.hpp>
#include <ctti/type_tag.hpp>

#include <string>
#include <string_view>
#include <concepts>
#include <type_traits>

namespace ctti::detail {

template <typename T>
concept TemplateSpecialization = requires {
  typename T::template rebind<int>;  // Not foolproof, but helps
};

template <typename T>
struct TemplateInfo {
  using type = T;
  static constexpr bool kIsTemplateInstantiation = false;
  static constexpr std::size_t kParameterCount = 0;

  static constexpr std::string_view GetName() noexcept {
    return NameOfImpl<T>::Apply();
  }
};

template <>
struct TemplateInfo<std::string> {
  using type = std::string;
  static constexpr bool kIsTemplateInstantiation = false;
  static constexpr std::size_t kParameterCount = 0;

  static constexpr std::string_view GetName() noexcept {
    return "std::string";
  }
};

template <>
struct TemplateInfo<std::string_view> {
  using type = std::string_view;
  static constexpr bool kIsTemplateInstantiation = false;
  static constexpr std::size_t kParameterCount = 0;

  static constexpr std::string_view GetName() noexcept {
    return "std::string_view";
  }
};

template <template <typename...> class Template, typename... Args>
struct TemplateInfo<Template<Args...>> {
  using type = Template<Args...>;
  static constexpr bool kIsTemplateInstantiation = true;
  static constexpr std::size_t kParameterCount = sizeof...(Args);

  using parameters = TypeList<Args...>;

  template <std::size_t I>
    requires(I < kParameterCount)
  using Parameter = PackGetType<I, Args...>;

  static constexpr std::string_view GetName() noexcept {
    return NameOfImpl<Template<Args...>>::Apply();
  }

  template <std::size_t I>
    requires(I < kParameterCount)
  static constexpr type_tag<Parameter<I>> GetParameterTag() noexcept {
    return {};
  }

  template <typename F>
  static constexpr void ForEachParameter(F&& f) {
    auto call_f = [&f](auto tag) { f(tag); };
    (call_f(type_tag<Args>{}), ...);
  }

  static constexpr std::array<std::string_view, kParameterCount> GetParameterNames() noexcept {
    return {NameOfImpl<Args>::Apply()...};
  }
};

// Specialization for non-type template parameters
template <template <auto...> class Template, auto... Args>
struct TemplateInfo<Template<Args...>> {
  using type = Template<Args...>;
  static constexpr bool kIsTemplateInstantiation = true;
  static constexpr std::size_t kParameterCount = sizeof...(Args);

  static constexpr std::string_view GetName() noexcept {
    return NameOfImpl<Template<Args...>>::Apply();
  }

  using parameters = TypeList<SizeType<static_cast<std::size_t>(Args)>...>;

  template <std::size_t I>
    requires(I < kParameterCount)
  static constexpr auto GetParameterValue() noexcept {
    return PackGetType<I, decltype(Args)...>{Args...};
  }
};

// Mixed template parameters (types and values)
template <template <typename, auto> class Template, typename T, auto Value>
struct TemplateInfo<Template<T, Value>> {
  using type = Template<T, Value>;
  static constexpr bool kIsTemplateInstantiation = true;
  static constexpr std::size_t kParameterCount = 2;

  static constexpr std::string_view GetName() noexcept {
    return NameOfImpl<Template<T, Value>>::Apply();
  }

  using type_parameter = T;
  static constexpr auto kValueParameter = Value;

  template <std::size_t I>
    requires(I < kParameterCount)
  static constexpr auto GetParameter() noexcept {
    if constexpr (I == 0) {
      return type_tag<T>{};
    } else {
      return Value;
    }
  }
};

template <typename T>
concept TemplateInstantiation = TemplateInfo<T>::kIsTemplateInstantiation;

// Utility functions
template <typename T>
constexpr bool IsTemplateInstantiation() noexcept {
  return TemplateInfo<T>::kIsTemplateInstantiation;
}

template <typename T>
constexpr std::size_t TemplateParameterCount() noexcept {
  return TemplateInfo<T>::kParameterCount;
}

template <typename T, std::size_t I>
  requires TemplateInstantiation<T> && (I < TemplateInfo<T>::kParameterCount)
using TemplateParameterT = typename TemplateInfo<T>::template Parameter<I>;

// Template family detection
template <template <typename...> class Template, typename T>
struct IsInstantiationOfHelper : std::false_type {};

template <template <typename...> class Template, typename... Args>
struct IsInstantiationOfHelper<Template, Template<Args...>> : std::true_type {};

template <template <typename...> class Template, typename T>
constexpr bool IsInstantiationOf() noexcept {
  return IsInstantiationOfHelper<Template, T>::value;
}

template <template <typename...> class Template, typename T>
concept InstantiationOf = IsInstantiationOfHelper<Template, T>::value;

}  // namespace ctti::detail

#endif  // CTTI_DETAIL_TEMPLATE_INFO_IMPL_HPP
