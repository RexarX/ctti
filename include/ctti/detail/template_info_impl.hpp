#pragma once

#include <ctti/detail/meta.hpp>
#include <ctti/detail/name_impl.hpp>
#include <ctti/detail/static_value_impl.hpp>
#include <ctti/type_tag.hpp>

#include <array>
#include <concepts>
#include <cstddef>
#include <memory>
#include <string>
#include <string_view>
#include <type_traits>
#include <vector>

namespace ctti::detail {

[[nodiscard]] constexpr bool LooksLikeTemplate(std::string_view name) noexcept {
  return name.find('<') != std::string_view::npos && name.find('>') != std::string_view::npos;
}

template <typename T>
[[nodiscard]] constexpr bool IsBasicTemplateInstantiation() noexcept {
  constexpr auto name = NameOfImpl<T>::Apply();
  return LooksLikeTemplate(name) && !std::is_fundamental_v<T>;
}

template <typename T>
struct TemplateInfo {
  using type = T;

  static constexpr bool kIsTemplateInstantiation = IsBasicTemplateInstantiation<T>();
  static constexpr std::size_t kParameterCount = 0;
  static constexpr std::size_t kTypeParameterCount = 0;
  static constexpr std::size_t kValueParameterCount = 0;

  [[nodiscard]] static constexpr std::string_view GetName() noexcept { return NameOfImpl<T>::Apply(); }
};

template <>
struct TemplateInfo<std::string> {
  using type = std::string;

  static constexpr bool kIsTemplateInstantiation = false;
  static constexpr std::size_t kParameterCount = 0;
  static constexpr std::size_t kTypeParameterCount = 0;
  static constexpr std::size_t kValueParameterCount = 0;

  [[nodiscard]] static constexpr std::string_view GetName() noexcept { return "std::string"; }
};

template <>
struct TemplateInfo<std::string_view> {
  using type = std::string_view;

  static constexpr bool kIsTemplateInstantiation = false;
  static constexpr std::size_t kParameterCount = 0;
  static constexpr std::size_t kTypeParameterCount = 0;
  static constexpr std::size_t kValueParameterCount = 0;

  [[nodiscard]] static constexpr std::string_view GetName() noexcept { return "std::string_view"; }
};

template <typename T, typename Alloc>
struct TemplateInfo<std::vector<T, Alloc>> {
  using type = std::vector<T, Alloc>;
  using parameters = TypeList<T>;
  using type_parameters = TypeList<T>;
  using value_parameters = TypeList<>;

  static constexpr bool kIsTemplateInstantiation = true;
  static constexpr std::size_t kParameterCount = 1;
  static constexpr std::size_t kTypeParameterCount = 1;
  static constexpr std::size_t kValueParameterCount = 0;

  template <std::size_t I>
    requires(I < kParameterCount)
  using Parameter = T;

  template <std::size_t I>
    requires(I < kTypeParameterCount)
  using TypeParameter = T;

  [[nodiscard]] static constexpr std::string_view GetName() noexcept {
    return NameOfImpl<std::vector<T, Alloc>>::Apply();
  }

  template <std::size_t I>
    requires(I < kTypeParameterCount)
  [[nodiscard]] static constexpr auto GetTypeParameterTag() noexcept -> ctti::type_tag<T> {
    return ctti::type_tag<T>{};
  }

  template <typename F>
    requires std::invocable<const F&, ctti::type_tag<T>>
  static constexpr void ForEachTypeParameter(const F& func) noexcept(
      std::is_nothrow_invocable_v<const F&, ctti::type_tag<T>>) {
    func(ctti::type_tag<T>{});
  }

  template <typename F>
    requires std::invocable<const F&, ctti::type_tag<T>>
  static constexpr void ForEachParameter(const F& func) noexcept(
      std::is_nothrow_invocable_v<const F&, ctti::type_tag<T>>) {
    ForEachTypeParameter(func);
  }

  [[nodiscard]] static constexpr auto GetTypeParameterNames() noexcept
      -> std::array<std::string_view, kTypeParameterCount> {
    return {NameOfImpl<T>::Apply()};
  }

  [[nodiscard]] static constexpr auto GetParameterNames() noexcept -> std::array<std::string_view, kParameterCount> {
    return GetTypeParameterNames();
  }
};

template <typename T, typename Deleter>
struct TemplateInfo<std::unique_ptr<T, Deleter>> {
  using type = std::unique_ptr<T, Deleter>;
  using parameters = TypeList<T>;
  using type_parameters = TypeList<T>;
  using value_parameters = TypeList<>;

  static constexpr bool kIsTemplateInstantiation = true;
  static constexpr std::size_t kParameterCount = 1;
  static constexpr std::size_t kTypeParameterCount = 1;
  static constexpr std::size_t kValueParameterCount = 0;

  template <std::size_t I>
    requires(I < kParameterCount)
  using Parameter = T;

  template <std::size_t I>
    requires(I < kTypeParameterCount)
  using TypeParameter = T;

  [[nodiscard]] static constexpr std::string_view GetName() noexcept {
    return NameOfImpl<std::unique_ptr<T, Deleter>>::Apply();
  }

  template <std::size_t I>
    requires(I < kTypeParameterCount)
  [[nodiscard]] static constexpr auto GetTypeParameterTag() noexcept -> ctti::type_tag<T> {
    return ctti::type_tag<T>{};
  }

  template <typename F>
    requires std::invocable<const F&, ctti::type_tag<T>>
  static constexpr void ForEachTypeParameter(const F& func) noexcept(
      std::is_nothrow_invocable_v<const F&, ctti::type_tag<T>>) {
    func(ctti::type_tag<T>{});
  }

  template <typename F>
    requires std::invocable<const F&, ctti::type_tag<T>>
  static constexpr void ForEachParameter(const F& func) noexcept(
      std::is_nothrow_invocable_v<const F&, ctti::type_tag<T>>) {
    ForEachTypeParameter(func);
  }

  [[nodiscard]] static constexpr auto GetTypeParameterNames() noexcept
      -> std::array<std::string_view, kTypeParameterCount> {
    return {NameOfImpl<T>::Apply()};
  }

  [[nodiscard]] static constexpr auto GetParameterNames() noexcept -> std::array<std::string_view, kParameterCount> {
    return GetTypeParameterNames();
  }
};

template <typename T>
struct TemplateInfo<std::shared_ptr<T>> {
  using type = std::shared_ptr<T>;
  using parameters = TypeList<T>;
  using type_parameters = TypeList<T>;
  using value_parameters = TypeList<>;

  static constexpr bool kIsTemplateInstantiation = true;
  static constexpr std::size_t kParameterCount = 1;
  static constexpr std::size_t kTypeParameterCount = 1;
  static constexpr std::size_t kValueParameterCount = 0;

  template <std::size_t I>
    requires(I < kParameterCount)
  using Parameter = T;

  template <std::size_t I>
    requires(I < kTypeParameterCount)
  using TypeParameter = T;

  [[nodiscard]] static constexpr std::string_view GetName() noexcept { return NameOfImpl<std::shared_ptr<T>>::Apply(); }

  template <std::size_t I>
    requires(I < kTypeParameterCount)
  [[nodiscard]] static constexpr auto GetTypeParameterTag() noexcept -> ctti::type_tag<T> {
    return ctti::type_tag<T>{};
  }

  template <typename F>
    requires std::invocable<const F&, ctti::type_tag<T>>
  static constexpr void ForEachTypeParameter(const F& func) noexcept(
      std::is_nothrow_invocable_v<const F&, ctti::type_tag<T>>) {
    func(ctti::type_tag<T>{});
  }

  template <typename F>
    requires std::invocable<const F&, ctti::type_tag<T>>
  static constexpr void ForEachParameter(const F& func) noexcept(
      std::is_nothrow_invocable_v<const F&, ctti::type_tag<T>>) {
    ForEachTypeParameter(func);
  }

  [[nodiscard]] static constexpr auto GetTypeParameterNames() noexcept
      -> std::array<std::string_view, kTypeParameterCount> {
    return {NameOfImpl<T>::Apply()};
  }

  [[nodiscard]] static constexpr auto GetParameterNames() noexcept -> std::array<std::string_view, kParameterCount> {
    return GetTypeParameterNames();
  }
};

template <template <typename> class Template, typename Arg>
  requires(!std::same_as<Template<Arg>, std::vector<Arg, std::allocator<Arg>>> &&
           !std::same_as<Template<Arg>, std::string> && !std::same_as<Template<Arg>, std::shared_ptr<Arg>>)
struct TemplateInfo<Template<Arg>> {
  using type = Template<Arg>;
  using parameters = TypeList<Arg>;
  using type_parameters = TypeList<Arg>;
  using value_parameters = TypeList<>;

  static constexpr bool kIsTemplateInstantiation = true;
  static constexpr std::size_t kParameterCount = 1;
  static constexpr std::size_t kTypeParameterCount = 1;
  static constexpr std::size_t kValueParameterCount = 0;

  template <std::size_t I>
    requires(I < kParameterCount)
  using Parameter = Arg;

  template <std::size_t I>
    requires(I < kTypeParameterCount)
  using TypeParameter = Arg;

  [[nodiscard]] static constexpr std::string_view GetName() noexcept { return NameOfImpl<Template<Arg>>::Apply(); }

  template <std::size_t I>
    requires(I < kTypeParameterCount)
  [[nodiscard]] static constexpr auto GetTypeParameterTag() noexcept -> ctti::type_tag<TypeParameter<I>> {
    return ctti::type_tag<TypeParameter<I>>{};
  }

  template <typename F>
    requires std::invocable<const F&, ctti::type_tag<Arg>>
  static constexpr void ForEachTypeParameter(const F& func) noexcept(
      std::is_nothrow_invocable_v<const F&, ctti::type_tag<Arg>>) {
    func(ctti::type_tag<Arg>{});
  }

  template <typename F>
    requires std::invocable<const F&, ctti::type_tag<Arg>>
  static constexpr void ForEachParameter(const F& func) noexcept(
      std::is_nothrow_invocable_v<const F&, ctti::type_tag<Arg>>) {
    ForEachTypeParameter(func);
  }

  [[nodiscard]] static constexpr auto GetTypeParameterNames() noexcept
      -> std::array<std::string_view, kTypeParameterCount> {
    return {NameOfImpl<Arg>::Apply()};
  }

  [[nodiscard]] static constexpr auto GetParameterNames() noexcept -> std::array<std::string_view, kParameterCount> {
    return GetTypeParameterNames();
  }
};

template <template <typename, typename> class Template, typename T, typename U>
  requires(!std::same_as<Template<T, U>, std::vector<T, U>> && !std::same_as<Template<T, U>, std::unique_ptr<T, U>>)
struct TemplateInfo<Template<T, U>> {
  using type = Template<T, U>;
  using parameters = TypeList<T, U>;
  using type_parameters = TypeList<T, U>;
  using value_parameters = TypeList<>;

  static constexpr bool kIsTemplateInstantiation = true;
  static constexpr std::size_t kParameterCount = 2;
  static constexpr std::size_t kTypeParameterCount = 2;
  static constexpr std::size_t kValueParameterCount = 0;

  template <std::size_t I>
    requires(I < kParameterCount)
  using Parameter = std::conditional_t<I == 0, T, U>;

  template <std::size_t I>
    requires(I < kTypeParameterCount)
  using TypeParameter = std::conditional_t<I == 0, T, U>;

  [[nodiscard]] static constexpr std::string_view GetName() noexcept { return NameOfImpl<Template<T, U>>::Apply(); }

  template <std::size_t I>
    requires(I < kTypeParameterCount)
  [[nodiscard]] static constexpr auto GetTypeParameterTag() noexcept {
    if constexpr (I == 0) {
      return ctti::type_tag<T>{};
    } else {
      return ctti::type_tag<U>{};
    }
  }

  template <typename F>
    requires std::invocable<const F&, ctti::type_tag<T>> && std::invocable<const F&, ctti::type_tag<U>>
  static constexpr void ForEachTypeParameter(const F& func) noexcept(
      std::is_nothrow_invocable_v<const F&, ctti::type_tag<T>> &&
      std::is_nothrow_invocable_v<const F&, ctti::type_tag<U>>) {
    func(ctti::type_tag<T>{});
    func(ctti::type_tag<U>{});
  }

  template <typename F>
    requires std::invocable<const F&, ctti::type_tag<T>> && std::invocable<const F&, ctti::type_tag<U>>
  static constexpr void ForEachParameter(const F& func) noexcept(
      std::is_nothrow_invocable_v<const F&, ctti::type_tag<T>> &&
      std::is_nothrow_invocable_v<const F&, ctti::type_tag<U>>) {
    ForEachTypeParameter(func);
  }

  [[nodiscard]] static constexpr auto GetTypeParameterNames() noexcept
      -> std::array<std::string_view, kTypeParameterCount> {
    return {NameOfImpl<T>::Apply(), NameOfImpl<U>::Apply()};
  }

  [[nodiscard]] static constexpr auto GetParameterNames() noexcept -> std::array<std::string_view, kParameterCount> {
    return GetTypeParameterNames();
  }
};

template <template <typename, auto> class Template, typename T, auto Value>
struct TemplateInfo<Template<T, Value>> {
  using type = Template<T, Value>;
  using type_parameter = T;

  static constexpr bool kIsTemplateInstantiation = true;
  static constexpr std::size_t kParameterCount = 2;
  static constexpr std::size_t kTypeParameterCount = 1;
  static constexpr std::size_t kValueParameterCount = 1;
  static constexpr auto kValueParameter = Value;

  template <std::size_t I>
    requires(I < kParameterCount)
  [[nodiscard]] static constexpr auto GetParameter() noexcept {
    if constexpr (I == 0) {
      return ctti::type_tag<T>{};
    } else {
      return StaticValue<decltype(Value), Value>{};
    }
  }

  [[nodiscard]] static constexpr std::string_view GetName() noexcept { return NameOfImpl<Template<T, Value>>::Apply(); }
};

template <template <auto> class Template, auto Value>
struct TemplateInfo<Template<Value>> {
  using type = Template<Value>;
  using parameters = TypeList<StaticValue<decltype(Value), Value>>;
  using type_parameters = TypeList<>;
  using value_parameters = TypeList<StaticValue<decltype(Value), Value>>;

  static constexpr bool kIsTemplateInstantiation = true;
  static constexpr std::size_t kParameterCount = 1;
  static constexpr std::size_t kTypeParameterCount = 0;
  static constexpr std::size_t kValueParameterCount = 1;

  [[nodiscard]] static constexpr std::string_view GetName() noexcept { return NameOfImpl<Template<Value>>::Apply(); }

  template <std::size_t I>
    requires(I < kValueParameterCount)
  [[nodiscard]] static constexpr auto GetValueParameter() noexcept {
    static_assert(I == 0);
    return Value;
  }

  template <typename F>
    requires std::invocable<const F&, StaticValue<decltype(Value), Value>>
  static constexpr void ForEachValueParameter(const F& func) noexcept(
      std::is_nothrow_invocable_v<const F&, StaticValue<decltype(Value), Value>>) {
    func(StaticValue<decltype(Value), Value>{});
  }

  template <typename F>
    requires std::invocable<const F&, StaticValue<decltype(Value), Value>>
  static constexpr void ForEachParameter(const F& func) noexcept(
      std::is_nothrow_invocable_v<const F&, StaticValue<decltype(Value), Value>>) {
    ForEachValueParameter(func);
  }
};

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
