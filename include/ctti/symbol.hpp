#pragma once

#include <ctti/attributes.hpp>
#include <ctti/detail/attributes_impl.hpp>
#include <ctti/detail/member_traits.hpp>
#include <ctti/detail/symbol_impl.hpp>

#include <concepts>
#include <cstddef>
#include <cstdint>
#include <string_view>
#include <utility>

namespace ctti {

template <typename T>
struct meta;

template <typename InternalSymbol>
class symbol {
private:
  using internal_type = std::remove_cvref_t<InternalSymbol>;

public:
  static constexpr std::string_view name = internal_type::kName;
  static constexpr std::uint64_t hash = internal_type::kHash;
  static constexpr bool has_overloads = internal_type::kHasOverloads;
  static constexpr std::size_t overload_count = internal_type::kOverloadCount;

  template <typename T>
  static constexpr bool is_owner_of() noexcept {
    return internal_type::template IsOwnerOf<T>();
  }

  template <typename Attribute>
  static constexpr bool has_attribute() noexcept {
    return internal_type::template HasAttribute<Attribute>();
  }

  template <auto Value>
  static constexpr bool has_attribute_value() noexcept {
    return internal_type::template HasAttributeValue<Value>();
  }

  template <typename Tag>
  static constexpr bool has_tag() noexcept {
    return internal_type::template HasTag<Tag>();
  }

  template <typename Signature>
  static constexpr bool has_overload() noexcept {
    return internal_type::template HasOverloadWithSignature<Signature>();
  }

  template <typename Obj, typename... Args>
    requires(internal_type::template HasOverload<std::remove_cvref_t<Obj>, Args...>())
  static constexpr decltype(auto) call(Obj&& obj, Args&&... args) noexcept(
      noexcept(internal_type::template Call<Obj, Args...>(std::forward<Obj>(obj), std::forward<Args>(args)...))) {
    return internal_type::template Call<Obj, Args...>(std::forward<Obj>(obj), std::forward<Args>(args)...);
  }

  template <typename T>
  static constexpr auto get_member() noexcept {
    return internal_type::template GetMember<T>();
  }

  template <typename T>
    requires(internal_type::template IsOwnerOf<std::remove_cvref_t<T>>())
  static constexpr decltype(auto) get_value(T&& obj) noexcept(noexcept(internal_type::GetValue(std::forward<T>(obj)))) {
    return internal_type::GetValue(std::forward<T>(obj));
  }

  template <typename T, typename Value>
    requires(internal_type::template IsOwnerOf<std::remove_cvref_t<T>>())
  static constexpr void set_value(T&& obj, Value&& value) noexcept(
      noexcept(internal_type::SetValue(std::forward<T>(obj), std::forward<Value>(value)))) {
    internal_type::SetValue(std::forward<T>(obj), std::forward<Value>(value));
  }
};

template <detail::CompileTimeString Name>
using symbol_type = symbol<detail::Symbol<Name>>;

template <detail::CompileTimeString Name, typename AttributeList = no_attributes>
consteval auto make_symbol() noexcept {
  if constexpr (std::same_as<AttributeList, no_attributes>) {
    return symbol<detail::Symbol<Name, detail::TypeList<>>>{};
  } else {
    return symbol<detail::Symbol<Name, typename AttributeList::attributes_type>>{};
  }
}

template <detail::CompileTimeString Name, typename AttributeList, auto... MemberPtrs>
consteval auto make_symbol_with_overloads() noexcept {
  if constexpr (std::same_as<AttributeList, no_attributes>) {
    return symbol<detail::Symbol<Name, detail::TypeList<>, MemberPtrs...>>{};
  } else {
    return symbol<detail::Symbol<Name, typename AttributeList::attributes_type, MemberPtrs...>>{};
  }
}

template <detail::CompileTimeString Name, auto MemberPtr>
consteval auto make_simple_symbol() noexcept {
  return symbol<detail::Symbol<Name, detail::TypeList<>, MemberPtr>>{};
}

template <detail::CompileTimeString Name, auto MemberPtr, typename... Attributes>
consteval auto make_attributed_symbol() noexcept {
  return symbol<detail::Symbol<Name, detail::TypeList<Attributes...>, MemberPtr>>{};
}

template <detail::CompileTimeString Name, typename T>
constexpr auto get_reflected_symbol() noexcept {
  if constexpr (requires {
                  typename meta<T>::type;
                  meta<T>::reflection;
                }) {
    constexpr auto reflection = meta<T>::reflection;
    return reflection.template get_symbol<Name>();
  } else {
    return symbol<detail::Symbol<Name>>{};
  }
}

template <detail::CompileTimeString Name, typename T>
  requires(get_reflected_symbol<Name, std::remove_cvref_t<T>>().template is_owner_of<std::remove_cvref_t<T>>())
constexpr decltype(auto) get_symbol_value(T&& obj) noexcept(
    noexcept(get_reflected_symbol<Name, std::remove_cvref_t<T>>().get_value(std::forward<T>(obj)))) {
  constexpr auto sym = get_reflected_symbol<Name, std::remove_cvref_t<T>>();
  return sym.get_value(std::forward<T>(obj));
}

template <detail::CompileTimeString Name, typename T, typename Value>
  requires(get_reflected_symbol<Name, std::remove_cvref_t<T>>().template is_owner_of<std::remove_cvref_t<T>>())
constexpr void set_symbol_value(T&& obj, Value&& value) noexcept(noexcept(
    get_reflected_symbol<Name, std::remove_cvref_t<T>>().set_value(std::forward<T>(obj), std::forward<Value>(value)))) {
  constexpr auto sym = get_reflected_symbol<Name, std::remove_cvref_t<T>>();
  sym.set_value(std::forward<T>(obj), std::forward<Value>(value));
}

template <detail::CompileTimeString Name, typename T, typename... Args>
  requires(get_reflected_symbol<Name, T>().template has_overload<Args...>())
constexpr decltype(auto) call_symbol(T&& obj, Args&&... args) noexcept(noexcept(
    get_reflected_symbol<Name, std::remove_cvref_t<T>>().call(std::forward<T>(obj), std::forward<Args>(args)...))) {
  constexpr auto symbol = get_reflected_symbol<Name, std::remove_cvref_t<T>>();
  return symbol.call(std::forward<T>(obj), std::forward<Args>(args)...);
}

template <detail::CompileTimeString Name, typename T, typename Attribute>
constexpr bool symbol_has_attribute() noexcept {
  constexpr auto symbol = get_reflected_symbol<Name, T>();
  return symbol.template has_attribute<Attribute>();
}

template <detail::CompileTimeString Name, typename T, auto Value>
constexpr bool symbol_has_attribute_value() noexcept {
  constexpr auto symbol = get_reflected_symbol<Name, T>();
  return symbol.template has_attribute_value<Value>();
}

template <detail::CompileTimeString Name, typename T, typename Tag>
constexpr bool symbol_has_tag() noexcept {
  constexpr auto symbol = get_reflected_symbol<Name, T>();
  return symbol.template has_tag<Tag>();
}

template <detail::CompileTimeString Name, typename T, typename Signature>
constexpr bool symbol_has_overload() noexcept {
  constexpr auto symbol = get_reflected_symbol<Name, T>();
  return symbol.template has_overload<Signature>();
}

template <detail::CompileTimeString Name, typename T>
constexpr std::size_t symbol_overload_count() noexcept {
  constexpr auto symbol = get_reflected_symbol<Name, T>();
  return symbol.overload_count;
}

template <detail::CompileTimeString Name, typename T>
constexpr std::string_view symbol_name() noexcept {
  constexpr auto symbol = get_reflected_symbol<Name, T>();
  return symbol.name;
}

template <detail::CompileTimeString Name, typename T>
constexpr std::uint64_t symbol_hash() noexcept {
  constexpr auto symbol = get_reflected_symbol<Name, T>();
  return symbol.hash;
}

}  // namespace ctti
