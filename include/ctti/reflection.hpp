#ifndef CTTI_REFLECTION_HPP
#define CTTI_REFLECTION_HPP

#include <ctti/detail/reflection_impl.hpp>
#include <ctti/detail/symbol_impl.hpp>
#include <ctti/symbol.hpp>

#include <array>
#include <string_view>

namespace ctti {

template <typename T>
struct meta;

template <typename InternalDef>
struct member_definition {
  using internal_type = InternalDef;

  static constexpr std::string_view name = internal_type::kName;
  static constexpr auto hash = internal_type::kHash;

  using attributes_type = typename internal_type::AttributesType;
  using symbol_type = typename internal_type::SymbolType;

  static constexpr auto make_symbol() noexcept { return internal_type::MakeSymbol(); }
};

template <detail::CompileTimeString Name, auto Ptr, typename... Attrs>
consteval auto member(Attrs...) {
  using internal_def = detail::SymbolDefinition<Name, Ptr, Attrs...>;
  return member_definition<internal_def>{};
}

template <detail::CompileTimeString Name, auto... Ptrs>
consteval auto overloaded_member() {
  using internal_def = detail::OverloadedSymbolDefinition<Name, detail::TypeList<>, Ptrs...>;
  return member_definition<internal_def>{};
}

template <detail::CompileTimeString Name, auto... Ptrs, typename... Attrs>
consteval auto overloaded_member(Attrs...) {
  using internal_def = detail::OverloadedSymbolDefinition<Name, detail::TypeList<Attrs...>, Ptrs...>;
  return member_definition<internal_def>{};
}

template <typename InternalReflection>
class reflection {
private:
  using internal_type = InternalReflection;

public:
  using type = typename internal_type::Type;
  static constexpr std::size_t size = internal_type::kSize;

  template <detail::CompileTimeString Name>
  static consteval auto get_symbol() noexcept {
    constexpr auto symbol_opt = internal_type::template GetSymbol<Name>();
    if constexpr (symbol_opt.has_value()) {
      return symbol<decltype(symbol_opt.value())>{};
    } else {
      return symbol<detail::Symbol<Name>>{};
    }
  }

  template <detail::CompileTimeString Name>
  static constexpr bool has_symbol() noexcept {
    return internal_type::template HasSymbol<Name>();
  }

  static constexpr std::array<std::string_view, size> get_symbol_names() noexcept {
    return internal_type::GetSymbolNames();
  }

  template <typename F>
  static constexpr void for_each_symbol(F&& f) {
    internal_type::ForEachSymbol([&f](auto internal_symbol) { f(symbol<decltype(internal_symbol)>{}); });
  }
};

template <typename... Defs>
consteval auto make_reflection(Defs...) {
  if constexpr (sizeof...(Defs) > 0) {
    using deduced_type = typename detail::DeduceTypeFromDefinitions<typename Defs::internal_type...>::Type;
    using internal_reflection = detail::TypeReflection<deduced_type, typename Defs::internal_type...>;
    return reflection<internal_reflection>{};
  } else {
    using internal_reflection = detail::TypeReflection<void>;
    return reflection<internal_reflection>{};
  }
}

template <typename T>
concept reflectable = requires {
  typename meta<T>::type;
  requires std::same_as<typename meta<T>::type, T>;
  requires meta<T>::reflection.size > 0;
};

template <reflectable T>
constexpr auto get_reflection() noexcept {
  return meta<T>::reflection;
}

template <reflectable T, detail::CompileTimeString Name>
constexpr auto get_symbol() noexcept {
  return get_reflection<T>().template get_symbol<Name>();
}

template <reflectable T>
constexpr std::size_t symbol_count() noexcept {
  return get_reflection<T>().size;
}

template <reflectable T>
constexpr auto get_symbol_names() noexcept {
  return get_reflection<T>().get_symbol_names();
}

template <reflectable T, typename F>
constexpr void for_each_symbol(F&& f) {
  get_reflection<T>().for_each_symbol(std::forward<F>(f));
}

template <detail::CompileTimeString Name, typename T>
constexpr bool has_symbol() noexcept {
  if constexpr (reflectable<T>) {
    return get_reflection<T>().template has_symbol<Name>();
  }
  return false;
}

}  // namespace ctti

#endif  // CTTI_REFLECTION_HPP
