#pragma once

#include <ctti/detail/meta.hpp>
#include <ctti/detail/symbol_impl.hpp>

#include <array>
#include <concepts>
#include <cstddef>
#include <optional>
#include <string_view>
#include <tuple>
#include <type_traits>

namespace ctti::detail {

template <typename ReflectedType, typename... Definitions>
class TypeReflection {
public:
  using Type = ReflectedType;
  using DefinitionsType = TypeList<Definitions...>;

  static constexpr std::size_t kSize = sizeof...(Definitions);

  template <std::size_t I>
    requires(I < kSize)
  using DefinitionAt = std::tuple_element_t<I, std::tuple<Definitions...>>;

  template <CompileTimeString Name>
  [[nodiscard]] static consteval auto GetSymbol() noexcept {
    return GetSymbolByName<Name, Definitions...>();
  }

  template <typename F>
    requires(std::invocable<const F&, decltype(Definitions::MakeSymbol())> && ...)
  [[nodiscard]] static constexpr void ForEachSymbol(const F& func) noexcept(
      (std::is_nothrow_invocable_v<const F&, decltype(Definitions::MakeSymbol())> && ...)) {
    (func(Definitions::MakeSymbol()), ...);
  }

  template <CompileTimeString Name>
  [[nodiscard]] static constexpr bool HasSymbol() noexcept {
    return HasSymbolByName<Name, Definitions...>();
  }

  [[nodiscard]] static constexpr auto GetSymbolNames() noexcept -> std::array<std::string_view, kSize> {
    if constexpr (kSize > 0) {
      return {Definitions::kName...};
    } else {
      return {};
    }
  }

private:
  template <CompileTimeString Name, typename First, typename... Rest>
  [[nodiscard]] static consteval auto GetSymbolByName() noexcept {
    if constexpr (Name.View() == First::kName) {
      return std::optional{First::MakeSymbol()};
    } else if constexpr (sizeof...(Rest) > 0) {
      return GetSymbolByName<Name, Rest...>();
    } else {
      return std::optional<typename First::SymbolType>{};
    }
  }

  template <CompileTimeString Name>
  [[nodiscard]] static consteval auto GetSymbolByName() noexcept -> std::optional<std::nullptr_t> {
    return std::optional<std::nullptr_t>{};
  }

  template <CompileTimeString Name, typename First, typename... Rest>
  [[nodiscard]] static constexpr bool HasSymbolByName() noexcept {
    if constexpr (Name.View() == First::kName) {
      return true;
    } else if constexpr (sizeof...(Rest) > 0) {
      return HasSymbolByName<Name, Rest...>();
    } else {
      return false;
    }
  }
};

template <typename T>
struct ExtractClassType;

template <typename R, typename C>
struct ExtractClassType<R C::*> {
  using Type = C;
};

template <typename R, typename C, typename... Args>
struct ExtractClassType<R (C::*)(Args...)> {
  using Type = C;
};

template <typename R, typename C, typename... Args>
struct ExtractClassType<R (C::*)(Args...) const> {
  using Type = C;
};

template <typename R, typename C, typename... Args>
struct ExtractClassType<R (C::*)(Args...) noexcept> {
  using Type = C;
};

template <typename R, typename C, typename... Args>
struct ExtractClassType<R (C::*)(Args...) const noexcept> {
  using Type = C;
};

// Helper to get first member pointer from definitions
template <typename Definition>
struct GetFirstMemberPtr;

// For SymbolDefinition - get the single member pointer
template <CompileTimeString Name, auto Ptr, typename... Attributes>
struct GetFirstMemberPtr<SymbolDefinition<Name, Ptr, Attributes...>> {
  using ClassType = typename ExtractClassType<decltype(Ptr)>::Type;
  static constexpr auto kValue = Ptr;
};

// For OverloadedSymbolDefinition - get the first member pointer
template <CompileTimeString Name, typename AttributeList, auto FirstPtr, auto... RestPtrs>
struct GetFirstMemberPtr<OverloadedSymbolDefinition<Name, AttributeList, FirstPtr, RestPtrs...>> {
  using ClassType = typename ExtractClassType<decltype(FirstPtr)>::Type;
  static constexpr auto kValue = FirstPtr;
};

// Validate all definitions belong to the same class
template <typename ExpectedClass, typename Definition>
[[nodiscard]] constexpr bool ValidateDefinitionClass() {
  using ActualClass = typename GetFirstMemberPtr<Definition>::ClassType;
  return std::same_as<ExpectedClass, ActualClass>;
}

template <typename... Definitions>
class DeduceTypeFromDefinitions {
private:
  // Get the class type from the first definition
  using FirstDef = std::tuple_element_t<0, std::tuple<Definitions...>>;
  using ExpectedClass = typename GetFirstMemberPtr<FirstDef>::ClassType;

  // Validate all definitions belong to the same class
  static_assert((ValidateDefinitionClass<ExpectedClass, Definitions>() && ...),
                "All member definitions must belong to the same class");

public:
  using Type = ExpectedClass;
};

template <>
struct DeduceTypeFromDefinitions<> {
  using Type = void;
};

}  // namespace ctti::detail
