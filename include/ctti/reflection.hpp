#pragma once

#include <ctti/detail/reflection_impl.hpp>
#include <ctti/detail/symbol_impl.hpp>
#include <ctti/symbol.hpp>

#include <array>
#include <concepts>
#include <cstddef>
#include <string_view>

namespace ctti {

template <typename T>
struct meta;

/**
 * @brief Represents a compile-time symbol, which can be a member or an overloaded member of a type.
 * @tparam InternalDef The internal definition type that provides the implementation details.
 */
template <typename InternalDef>
struct member_definition {
  using internal_type = InternalDef;
  using attributes_type = typename internal_type::AttributesType;
  using symbol_type = typename internal_type::SymbolType;

  static constexpr std::string_view name = internal_type::kName;
  static constexpr auto hash = internal_type::kHash;

  /**
   * @brief Creates a symbol instance based on the internal definition.
   * @return A symbol instance.
   */
  [[nodiscard]] static constexpr auto make_symbol() noexcept { return internal_type::MakeSymbol(); }
};

/**
 * @brief Creates a member definition for a given name, pointer, and optional attributes.
 * @tparam Name The compile-time string representing the name of the member.
 * @tparam Ptr The pointer to the member.
 * @tparam Attrs The optional attributes associated with the member.
 * @return A member definition instance.
 */
template <detail::CompileTimeString Name, auto Ptr, typename... Attrs>
[[nodiscard]] consteval auto member(Attrs...) noexcept {
  using internal_def = detail::SymbolDefinition<Name, Ptr, Attrs...>;
  return member_definition<internal_def>{};
}

/**
 * @brief Creates an overloaded member definition for a given name, pointers, and optional attributes.
 * @tparam Name The compile-time string representing the name of the overloaded member.
 * @tparam Ptrs The pointers to the overloaded members.
 * @return An overloaded member definition instance.
 */
template <detail::CompileTimeString Name, auto... Ptrs>
[[nodiscard]] consteval auto overloaded_member() noexcept {
  using internal_def = detail::OverloadedSymbolDefinition<Name, detail::TypeList<>, Ptrs...>;
  return member_definition<internal_def>{};
}

/**
 * @brief Creates an overloaded member definition for a given name, pointers, and optional attributes.
 * @tparam Name The compile-time string representing the name of the overloaded member.
 * @tparam Ptrs The pointers to the overloaded members.
 * @tparam Attrs The optional attributes associated with the overloaded members.
 * @return An overloaded member definition instance.
 */
template <detail::CompileTimeString Name, auto... Ptrs, typename... Attrs>
[[nodiscard]] consteval auto overloaded_member(Attrs...) noexcept {
  using internal_def = detail::OverloadedSymbolDefinition<Name, detail::TypeList<Attrs...>, Ptrs...>;
  return member_definition<internal_def>{};
}

/**
 * @brief Represents a compile-time reflection of a type, providing access to its symbols and metadata.
 * @tparam InternalReflection The internal reflection type that provides the implementation details.
 */
template <typename InternalReflection>
class reflection {
private:
  using internal_type = InternalReflection;

public:
  using type = typename internal_type::Type;

  static constexpr std::size_t size = internal_type::kSize;

  /**
   * @brief Retrieves a symbol by its compile-time name.
   * @tparam Name The compile-time string representing the name of the symbol.
   * @return The symbol associated with the given name, or a placeholder symbol if not found
   */
  template <detail::CompileTimeString Name>
  [[nodiscard]] static consteval auto get_symbol() noexcept {
    constexpr auto symbol_opt = internal_type::template GetSymbol<Name>();
    if constexpr (symbol_opt.has_value()) {
      return symbol<decltype(symbol_opt.value())>{};
    } else {
      return symbol<detail::Symbol<Name>>{};
    }
  }

  /**
   * @brief Checks if a symbol with the given compile-time name exists.
   * @tparam Name The compile-time string representing the name of the symbol.
   * @return True if the symbol exists, false otherwise.
   */
  template <detail::CompileTimeString Name>
  [[nodiscard]] static constexpr bool has_symbol() noexcept {
    return internal_type::template HasSymbol<Name>();
  }

  /**
   * @brief Retrieves the names of all symbols in the reflection.
   * @return An array of string views representing the names of all symbols.
   */
  [[nodiscard]] static constexpr auto get_symbol_names() noexcept -> std::array<std::string_view, size> {
    return internal_type::GetSymbolNames();
  }

  /**
   * @brief Applies a given function to each symbol in the reflection.
   * @tparam F The type of the function to apply.
   * @param func The function to apply to each symbol.
   */
  template <typename F>
    requires std::invocable<
        const F&,
        symbol<decltype(std::declval<typename internal_type::DefinitionsType::template At<0>>().MakeSymbol())>>
  static constexpr void for_each_symbol(const F& func) noexcept(noexcept(
      internal_type::ForEachSymbol([&func](auto internal_symbol) { func(symbol<decltype(internal_symbol)>{}); }))) {
    internal_type::ForEachSymbol([&func](auto internal_symbol) { func(symbol<decltype(internal_symbol)>{}); });
  }
};

/**
 * @brief Creates a reflection for a given set of member definitions.
 * @tparam Defs The member definitions to include in the reflection.
 * @return A reflection instance containing the provided member definitions.
 */
template <typename... Defs>
[[nodiscard]] consteval auto make_reflection(Defs...) noexcept {
  if constexpr (sizeof...(Defs) > 0) {
    using deduced_type = typename detail::DeduceTypeFromDefinitions<typename Defs::internal_type...>::Type;
    using internal_reflection = detail::TypeReflection<deduced_type, typename Defs::internal_type...>;
    return reflection<internal_reflection>{};
  } else {
    using internal_reflection = detail::TypeReflection<void>;
    return reflection<internal_reflection>{};
  }
}

/**
 * @brief Concept to check if a type T has an associated reflection with at least one symbol.
 * @tparam T The type to check for reflection.
 */
template <typename T>
concept reflectable = requires {
  typename meta<T>::type;
  requires std::same_as<typename meta<T>::type, T>;
  requires meta<T>::reflection.size > 0;
};

/**
 * @brief Retrieves the reflection associated with a given type T.
 * @tparam T The type for which to retrieve the reflection.
 * @return The reflection associated with the type T.
 */
template <reflectable T>
[[nodiscard]] constexpr auto get_reflection() noexcept {
  return meta<T>::reflection;
}

/**
 * @brief Retrieves a symbol by its compile-time name from the reflection of a given type T.
 * @tparam T The type for which to retrieve the symbol.
 * @tparam Name The compile-time string representing the name of the symbol.
 * @return The symbol associated with the given name from the type T's reflection.
 */
template <reflectable T, detail::CompileTimeString Name>
[[nodiscard]] constexpr auto get_symbol() noexcept(noexcept(get_reflection<T>().template get_symbol<Name>())) {
  return get_reflection<T>().template get_symbol<Name>();
}

/**
 * @brief Retrieves the number of symbols in the reflection of a given type T.
 * @tparam T The type for which to retrieve the symbol count.
 * @return The number of symbols in the type T's reflection.
 */
template <reflectable T>
[[nodiscard]] constexpr std::size_t symbol_count() noexcept {
  return get_reflection<T>().size;
}

/**
 * @brief Retrieves the names of all symbols in the reflection of a given type T.
 * @tparam T The type for which to retrieve the symbol names.
 * @return An array of string views representing the names of all symbols in the type T's reflection.
 */
template <reflectable T>
[[nodiscard]] constexpr auto get_symbol_names() noexcept {
  return get_reflection<T>().get_symbol_names();
}

/**
 * @brief Applies a given function to each symbol in the reflection of a given type T.
 * @tparam T The type for which to apply the function to its symbols.
 * @tparam F The type of the function to apply.
 * @param func The function to apply to each symbol.
 */
template <reflectable T, typename F>
  requires std::invocable<const F&,
                          decltype(get_reflection<T>().template get_symbol<detail::CompileTimeString<1>{""}>())>
constexpr void for_each_symbol(const F& func) noexcept(
    noexcept(get_reflection<T>().for_each_symbol(std::declval<const F&>()))) {
  get_reflection<T>().for_each_symbol(func);
}

/**
 * @brief Checks if a symbol with the given compile-time name exists in the reflection of a given type T.
 * @tparam T The type for which to check for the symbol.
 * @tparam Name The compile-time string representing the name of the symbol.
 * @return True if the symbol exists in the type T's reflection, false otherwise.
 */
template <detail::CompileTimeString Name, typename T>
[[nodiscard]] constexpr bool has_symbol() noexcept {
  if constexpr (reflectable<T>) {
    return get_reflection<T>().template has_symbol<Name>();
  }
  return false;
}

}  // namespace ctti
