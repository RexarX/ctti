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

/**
 * @brief A reflection symbol that provides metadata and access to members of a type.
 * @tparam InternalSymbol The internal symbol type that provides the actual implementation.
 */
template <typename InternalSymbol>
class symbol {
private:
  using internal_type = std::remove_cvref_t<InternalSymbol>;

public:
  static constexpr std::string_view name = internal_type::kName;
  static constexpr std::size_t hash = internal_type::kHash;
  static constexpr bool has_overloads = internal_type::kHasOverloads;
  static constexpr std::size_t overload_count = internal_type::kOverloadCount;

  /**
   * @brief Checks if the symbol is an owner of the specified type T.
   * @tparam T The type to check ownership for.
   * @return true if the symbol owns T, false otherwise.
   */
  template <typename T>
  [[nodiscard]] static constexpr bool is_owner_of() noexcept {
    return internal_type::template IsOwnerOf<T>();
  }

  /**
   * @brief Checks if the symbol has the specified attribute.
   * @tparam Attribute The attribute type to check for.
   * @return true if the symbol has the attribute, false otherwise.
   */
  template <typename Attribute>
  [[nodiscard]] static constexpr bool has_attribute() noexcept {
    return internal_type::template HasAttribute<Attribute>();
  }

  /**
   * @brief Checks if the symbol has an attribute with the specified value.
   * @tparam Value The attribute value to check for.
   * @return true if the symbol has an attribute with the value, false otherwise.
   */
  template <auto Value>
  [[nodiscard]] static constexpr bool has_attribute_value() noexcept {
    return internal_type::template HasAttributeValue<Value>();
  }

  /**
   * @brief Checks if the symbol has the specified tag.
   * @tparam Tag The tag type to check for.
   * @return true if the symbol has the tag, false otherwise.
   */
  template <typename Tag>
  [[nodiscard]] static constexpr bool has_tag() noexcept {
    return internal_type::template HasTag<Tag>();
  }

  /**
   * @brief Checks if the symbol has an overload that matches the specified argument types.
   * @tparam Args The argument types to check for.
   * @return true if the symbol has a matching overload, false otherwise.
   */
  template <typename Signature>
  [[nodiscard]] static constexpr bool has_overload() noexcept {
    return internal_type::template HasOverloadWithSignature<Signature>();
  }

  /**
   * @brief Checks if the symbol has an overload that matches the specified argument types.
   * @tparam Args The argument types to check for.
   * @return true if the symbol has a matching overload, false otherwise.
   */
  template <typename Obj, typename... Args>
    requires(internal_type::template HasOverload<std::remove_cvref_t<Obj>, Args...>())
  [[nodiscard]] static constexpr decltype(auto) call(Obj&& obj, Args&&... args) noexcept(
      noexcept(internal_type::template Call<Obj, Args...>(std::forward<Obj>(obj), std::forward<Args>(args)...))) {
    return internal_type::template Call<Obj, Args...>(std::forward<Obj>(obj), std::forward<Args>(args)...);
  }

  /**
   * @brief Retrieves the member pointer associated with the specified type T.
   * @tparam T The type of the member to retrieve.
   * @return The member pointer of type T.
   */
  template <typename T>
  [[nodiscard]] static constexpr auto get_member() noexcept {
    return internal_type::template GetMember<T>();
  }

  /**
   * @brief Gets the value of the member from the specified object.
   * @tparam T The type of the object from which to get the member value.
   * @param obj The object instance.
   * @return The value of the member.
   */
  template <typename T>
    requires(internal_type::template IsOwnerOf<std::remove_cvref_t<T>>())
  [[nodiscard]] static constexpr decltype(auto) get_value(T&& obj) noexcept(
      noexcept(internal_type::GetValue(std::forward<T>(obj)))) {
    return internal_type::GetValue(std::forward<T>(obj));
  }

  /**
   * @brief Sets the value of the member in the specified object.
   * @tparam T The type of the object in which to set the member value.
   * @tparam Value The type of the value to set.
   * @param obj The object instance.
   * @param value The value to set.
   */
  template <typename T, typename Value>
    requires(internal_type::template IsOwnerOf<std::remove_cvref_t<T>>())
  static constexpr void set_value(T&& obj, Value&& value) noexcept(
      noexcept(internal_type::SetValue(std::forward<T>(obj), std::forward<Value>(value)))) {
    internal_type::SetValue(std::forward<T>(obj), std::forward<Value>(value));
  }
};

template <detail::CompileTimeString Name>
using symbol_type = symbol<detail::Symbol<Name>>;

/**
 * @brief Creates a symbol with the specified name and optional attributes.
 * @tparam Name The compile-time string representing the name of the symbol.
 * @tparam AttributeList The list of attributes to associate with the symbol (default is no_attributes).
 * @return A symbol instance with the specified name and attributes.
 */
template <detail::CompileTimeString Name, typename AttributeList = no_attributes>
[[nodiscard]] consteval auto make_symbol() noexcept {
  if constexpr (std::same_as<AttributeList, no_attributes>) {
    return symbol<detail::Symbol<Name, detail::TypeList<>>>{};
  } else {
    return symbol<detail::Symbol<Name, typename AttributeList::attributes_type>>{};
  }
}

/**
 * @brief Creates a symbol with the specified name, attributes, and member pointers for overloads.
 * @tparam Name The compile-time string representing the name of the symbol.
 * @tparam AttributeList The list of attributes to associate with the symbol (default is no_attributes).
 * @tparam MemberPtrs The member pointers representing the overloads of the symbol.
 * @return A symbol instance with the specified name, attributes, and overloads.
 */
template <detail::CompileTimeString Name, typename AttributeList, auto... MemberPtrs>
[[nodiscard]] consteval auto make_symbol_with_overloads() noexcept {
  if constexpr (std::same_as<AttributeList, no_attributes>) {
    return symbol<detail::Symbol<Name, detail::TypeList<>, MemberPtrs...>>{};
  } else {
    return symbol<detail::Symbol<Name, typename AttributeList::attributes_type, MemberPtrs...>>{};
  }
}

/**
 * @brief Creates a simple symbol with the specified name and member pointer.
 * @tparam Name The compile-time string representing the name of the symbol.
 * @tparam MemberPtr The member pointer associated with the symbol.
 * @return A symbol instance with the specified name and member pointer.
 */
template <detail::CompileTimeString Name, auto MemberPtr>
[[nodiscard]] consteval auto make_simple_symbol() noexcept
    -> symbol<detail::Symbol<Name, detail::TypeList<>, MemberPtr>> {
  return symbol<detail::Symbol<Name, detail::TypeList<>, MemberPtr>>{};
}

/**
 * @brief Creates an attributed symbol with the specified name, member pointer, and attributes.
 * @tparam Name The compile-time string representing the name of the symbol.
 * @tparam MemberPtr The member pointer associated with the symbol.
 * @tparam Attributes The list of attributes to associate with the symbol.
 * @return A symbol instance with the specified name, member pointer, and attributes.
 */
template <detail::CompileTimeString Name, auto MemberPtr, typename... Attributes>
[[nodiscard]] consteval auto make_attributed_symbol() noexcept
    -> symbol<detail::Symbol<Name, detail::TypeList<Attributes...>, MemberPtr>> {
  return symbol<detail::Symbol<Name, detail::TypeList<Attributes...>, MemberPtr>>{};
}

/**
 * @brief Retrieves the reflected symbol for the specified name and type.
 * @tparam Name The compile-time string representing the name of the symbol.
 * @tparam T The type from which to retrieve the symbol.
 * @return The reflected symbol if available, otherwise a default symbol with the specified name.
 */
template <detail::CompileTimeString Name, typename T>
[[nodiscard]] constexpr auto get_reflected_symbol() noexcept {
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

/**
 * @brief Retrieves the value of the symbol from the specified object.
 * @tparam Name The compile-time string representing the name of the symbol.
 * @tparam T The type of the object from which to get the symbol value.
 * @param obj The object instance.
 * @return The value of the symbol.
 */
template <detail::CompileTimeString Name, typename T>
  requires(get_reflected_symbol<Name, std::remove_cvref_t<T>>().template is_owner_of<std::remove_cvref_t<T>>())
[[nodiscard]] constexpr decltype(auto) get_symbol_value(T&& obj) noexcept(
    noexcept(get_reflected_symbol<Name, std::remove_cvref_t<T>>().get_value(std::forward<T>(obj)))) {
  constexpr auto sym = get_reflected_symbol<Name, std::remove_cvref_t<T>>();
  return sym.get_value(std::forward<T>(obj));
}

/**
 * @brief Sets the value of the symbol in the specified object.
 * @tparam Name The compile-time string representing the name of the symbol.
 * @tparam T The type of the object in which to set the symbol value.
 * @tparam Value The type of the value to set.
 * @param obj The object instance.
 * @param value The value to set.
 */
template <detail::CompileTimeString Name, typename T, typename Value>
  requires(get_reflected_symbol<Name, std::remove_cvref_t<T>>().template is_owner_of<std::remove_cvref_t<T>>())
constexpr void set_symbol_value(T&& obj, Value&& value) noexcept(noexcept(
    get_reflected_symbol<Name, std::remove_cvref_t<T>>().set_value(std::forward<T>(obj), std::forward<Value>(value)))) {
  constexpr auto sym = get_reflected_symbol<Name, std::remove_cvref_t<T>>();
  sym.set_value(std::forward<T>(obj), std::forward<Value>(value));
}

/**
 * @brief Calls the symbol with the specified arguments on the given object.
 * @tparam Name The compile-time string representing the name of the symbol.
 * @tparam T The type of the object on which to call the symbol.
 * @tparam Args The types of the arguments to pass to the symbol.
 * @param obj The object instance.
 * @param args The arguments to pass to the symbol.
 * @return The result of the symbol call.
 */
template <detail::CompileTimeString Name, typename T, typename... Args>
  requires(get_reflected_symbol<Name, T>().template has_overload<Args...>())
[[nodiscard]] constexpr decltype(auto) call_symbol(T&& obj, Args&&... args) noexcept(noexcept(
    get_reflected_symbol<Name, std::remove_cvref_t<T>>().call(std::forward<T>(obj), std::forward<Args>(args)...))) {
  constexpr auto symbol = get_reflected_symbol<Name, std::remove_cvref_t<T>>();
  return symbol.call(std::forward<T>(obj), std::forward<Args>(args)...);
}

/**
 * @brief Checks if the symbol is an owner of the specified type T.
 * @tparam Name The compile-time string representing the name of the symbol.
 * @tparam T The type to check ownership for.
 * @return true if the symbol owns T, false otherwise.
 */
template <detail::CompileTimeString Name, typename T, typename Attribute>
[[nodiscard]] constexpr bool symbol_has_attribute() noexcept {
  constexpr auto symbol = get_reflected_symbol<Name, T>();
  return symbol.template has_attribute<Attribute>();
}

/**
 * @brief Checks if the symbol has an attribute with the specified value.
 * @tparam Name The compile-time string representing the name of the symbol.
 * @tparam T The type from which to retrieve the symbol.
 * @tparam Value The attribute value to check for.
 * @return true if the symbol has an attribute with the value, false otherwise.
 */
template <detail::CompileTimeString Name, typename T, auto Value>
[[nodiscard]] constexpr bool symbol_has_attribute_value() noexcept {
  constexpr auto symbol = get_reflected_symbol<Name, T>();
  return symbol.template has_attribute_value<Value>();
}

/**
 * @brief Checks if the symbol has the specified tag.
 * @tparam Name The compile-time string representing the name of the symbol.
 * @tparam T The type from which to retrieve the symbol.
 * @tparam Tag The tag type to check for.
 * @return true if the symbol has the tag, false otherwise.
 */
template <detail::CompileTimeString Name, typename T, typename Tag>
[[nodiscard]] constexpr bool symbol_has_tag() noexcept {
  constexpr auto symbol = get_reflected_symbol<Name, T>();
  return symbol.template has_tag<Tag>();
}

/**
 * @brief Checks if the symbol has an overload that matches the specified signature.
 * @tparam Name The compile-time string representing the name of the symbol.
 * @tparam T The type from which to retrieve the symbol.
 * @tparam Signature The function signature to check for.
 * @return true if the symbol has a matching overload, false otherwise.
 */
template <detail::CompileTimeString Name, typename T, typename Signature>
[[nodiscard]] constexpr bool symbol_has_overload() noexcept {
  constexpr auto symbol = get_reflected_symbol<Name, T>();
  return symbol.template has_overload<Signature>();
}

/**
 * @brief Retrieves the number of overloads associated with the symbol.
 * @tparam Name The compile-time string representing the name of the symbol.
 * @tparam T The type from which to retrieve the symbol.
 * @return The number of overloads for the symbol.
 */
template <detail::CompileTimeString Name, typename T>
[[nodiscard]] constexpr std::size_t symbol_overload_count() noexcept {
  constexpr auto symbol = get_reflected_symbol<Name, T>();
  return symbol.overload_count;
}

/**
 * @brief Retrieves the name of the symbol.
 * @tparam Name The compile-time string representing the name of the symbol.
 * @tparam T The type from which to retrieve the symbol.
 * @return The name of the symbol as a string view.
 */
template <detail::CompileTimeString Name, typename T>
[[nodiscard]] constexpr std::string_view symbol_name() noexcept {
  constexpr auto symbol = get_reflected_symbol<Name, T>();
  return symbol.name;
}

/**
 * @brief Retrieves the hash of the symbol.
 * @tparam Name The compile-time string representing the name of the symbol.
 * @tparam T The type from which to retrieve the symbol.
 * @return The hash of the symbol as a uint64_t.
 */
template <detail::CompileTimeString Name, typename T>
[[nodiscard]] constexpr std::size_t symbol_hash() noexcept {
  constexpr auto symbol = get_reflected_symbol<Name, T>();
  return symbol.hash;
}

}  // namespace ctti
