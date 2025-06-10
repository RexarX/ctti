#ifndef CTTI_SYMBOL_HPP
#define CTTI_SYMBOL_HPP

#include <ctti/attributes.hpp>
#include <ctti/detail/attributes_impl.hpp>
#include <ctti/detail/member_traits.hpp>
#include <ctti/detail/symbol_impl.hpp>

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

  template <typename Obj, typename... Args>
  static constexpr bool has_overload() noexcept {
    return internal_type::template HasOverload<Obj, Args...>();
  }

  template <typename Obj, typename... Args>
    requires(has_overloads)
  static constexpr decltype(auto) call(Obj&& obj, Args&&... args)
    requires(internal_type::template HasOverload<Obj, Args...>())
  {
    return internal_type::template Call<Args...>(std::forward<Obj>(obj), std::forward<Args>(args)...);
  }

  template <typename T>
  static constexpr auto get_member() noexcept {
    return internal_type::template GetMember<T>();
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

template <auto Symbol, typename T, typename... Args>
  requires(decltype(Symbol)::template has_overload<T, Args...>())
constexpr decltype(auto) call_symbol(T&& obj, Args&&... args) {
  return decltype(Symbol)::template call<T, Args...>(std::forward<T>(obj), std::forward<Args>(args)...);
}

template <auto Symbol, typename T>
  requires(decltype(Symbol)::template is_owner_of<std::remove_cvref_t<T>>())
constexpr decltype(auto) get_symbol_value(T&& obj) {
  constexpr auto member = decltype(Symbol)::template get_member<std::decay_t<T>>();
  if constexpr (!std::same_as<std::remove_cvref_t<decltype(member)>, std::nullptr_t>) {
    using member_type = std::remove_cvref_t<decltype(member)>;
    return detail::MemberTraits<member_type>::Get(std::forward<T>(obj), member);
  } else {
    static_assert(sizeof(T) == 0, "Symbol has no member for this type");
  }
}

template <auto Symbol, typename T, typename Value>
  requires(decltype(Symbol)::template is_owner_of<std::remove_cvref_t<T>>())
constexpr void set_symbol_value(T&& obj, Value&& value) {
  constexpr auto member = decltype(Symbol)::template get_member<std::decay_t<T>>();
  if constexpr (!std::same_as<std::remove_cvref_t<decltype(member)>, std::nullptr_t>) {
    using member_type = std::remove_cvref_t<decltype(member)>;
    detail::MemberTraits<member_type>::Set(std::forward<T>(obj), member, std::forward<Value>(value));
  } else {
    static_assert(sizeof(T) == 0, "Symbol has no member for this type");
  }
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
constexpr decltype(auto) get_member_value(T&& obj) {
  constexpr auto sym = get_reflected_symbol<Name, std::remove_cvref_t<T>>();
  return get_symbol_value<sym>(std::forward<T>(obj));
}

template <detail::CompileTimeString Name, typename T, typename Value>
constexpr void set_member_value(T&& obj, Value&& value) {
  constexpr auto sym = get_reflected_symbol<Name, std::remove_cvref_t<T>>();
  set_symbol_value<sym>(std::forward<T>(obj), std::forward<Value>(value));
}

template <typename Member>
using member_traits = detail::MemberTraits<Member>;

template <auto Symbol, typename Attribute>
constexpr bool has_attribute() noexcept {
  return decltype(Symbol)::template has_attribute<Attribute>();
}

template <auto Symbol, auto Value>
constexpr bool has_attribute_value() noexcept {
  return decltype(Symbol)::template has_attribute_value<Value>();
}

template <auto Symbol, typename Tag>
constexpr bool has_tag() noexcept {
  return decltype(Symbol)::template has_tag<Tag>();
}

template <auto Symbol, typename Obj, typename... Args>
constexpr bool has_overload() noexcept {
  return decltype(Symbol)::template has_overload<Obj, Args...>();
}

template <auto Symbol>
constexpr std::size_t overload_count() noexcept {
  return decltype(Symbol)::overload_count;
}

template <auto Symbol>
constexpr std::string_view symbol_name() noexcept {
  return decltype(Symbol)::name;
}

template <auto Symbol>
constexpr std::uint64_t symbol_hash() noexcept {
  return decltype(Symbol)::hash;
}

}  // namespace ctti

#endif  // CTTI_SYMBOL_HPP
