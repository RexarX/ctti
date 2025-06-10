#ifndef CTTI_DETAIL_REFLECTION_IMPL_HPP
#define CTTI_DETAIL_REFLECTION_IMPL_HPP

#include <ctti/detail/meta.hpp>
#include <ctti/detail/symbol_impl.hpp>

#include <array>
#include <optional>
#include <string_view>

namespace ctti::detail {

template <typename ReflectedType, typename... Definitions>
class TypeReflection {
public:
  using Type = ReflectedType;
  using DefinitionsType = TypeList<Definitions...>;
  static constexpr std::size_t kSize = sizeof...(Definitions);

  template <CompileTimeString Name>
  static consteval auto GetSymbol() noexcept {
    return GetSymbolByName<Name, Definitions...>();
  }

  template <std::size_t I>
    requires(I < kSize)
  using DefinitionAt = std::tuple_element_t<I, std::tuple<Definitions...>>;

  template <typename F>
  static constexpr void ForEachSymbol(F&& f) {
    (f(Definitions::MakeSymbol()), ...);
  }

  template <CompileTimeString Name>
  static constexpr bool HasSymbol() noexcept {
    return HasSymbolByName<Name, Definitions...>();
  }

  static constexpr std::array<std::string_view, kSize> GetSymbolNames() noexcept {
    if constexpr (kSize > 0) {
      return {Definitions::kName...};
    } else {
      return {};
    }
  }

private:
  template <CompileTimeString Name, typename First, typename... Rest>
  static consteval auto GetSymbolByName() noexcept {
    if constexpr (Name.View() == First::kName) {
      return std::optional{First::MakeSymbol()};
    } else if constexpr (sizeof...(Rest) > 0) {
      return GetSymbolByName<Name, Rest...>();
    } else {
      return std::optional<typename First::SymbolType>{};
    }
  }

  template <CompileTimeString Name>
  static consteval auto GetSymbolByName() noexcept {
    return std::optional<std::nullptr_t>{};
  }

  template <CompileTimeString Name, typename First, typename... Rest>
  static constexpr bool HasSymbolByName() noexcept {
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
  static constexpr auto value = Ptr;
  using ClassType = typename ExtractClassType<decltype(Ptr)>::Type;
};

// For OverloadedSymbolDefinition - get the first member pointer
template <CompileTimeString Name, typename AttributeList, auto FirstPtr, auto... RestPtrs>
struct GetFirstMemberPtr<OverloadedSymbolDefinition<Name, AttributeList, FirstPtr, RestPtrs...>> {
  static constexpr auto value = FirstPtr;
  using ClassType = typename ExtractClassType<decltype(FirstPtr)>::Type;
};

// Validate all definitions belong to the same class
template <typename ExpectedClass, typename Definition>
constexpr bool ValidateDefinitionClass() {
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

#endif  // CTTI_DETAIL_REFLECTION_IMPL_HPP
