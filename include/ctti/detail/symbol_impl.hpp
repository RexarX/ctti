#ifndef CTTI_DETAIL_SYMBOL_IMPL_HPP
#define CTTI_DETAIL_SYMBOL_IMPL_HPP

#include <ctti/detail/attributes_impl.hpp>
#include <ctti/detail/compile_time_string.hpp>
#include <ctti/detail/hash_impl.hpp>
#include <ctti/detail/member_traits.hpp>
#include <ctti/detail/meta.hpp>

#include <concepts>
#include <string_view>
#include <type_traits>

namespace ctti::detail {

template <typename AttributeList, auto Value>
constexpr bool SymbolHasAttributeValue() noexcept {
  if constexpr (AttributeList::kSize == 0) {
    return false;
  } else {
    return [&]<std::size_t... Is>(std::index_sequence<Is...>) {
      return (... || AttributeHasValue<typename AttributeList::template At<Is>, Value>());
    }(std::make_index_sequence<AttributeList::kSize>{});
  }
}

template <typename AttributeList, typename Tag>
constexpr bool SymbolHasAttributeTag() noexcept {
  if constexpr (AttributeList::kSize == 0) {
    return false;
  } else {
    return [&]<std::size_t... Is>(std::index_sequence<Is...>) {
      return (... || AttributeHasTag<typename AttributeList::template At<Is>, Tag>());
    }(std::make_index_sequence<AttributeList::kSize>{});
  }
}

template <CompileTimeString Name, typename AttributeList = TypeList<>, auto... MemberPtrs>
struct Symbol {
  static constexpr std::string_view kName = Name.View();
  static constexpr HashType kHash = Fnv1aHash(kName);
  static constexpr bool kHasOverloads = sizeof...(MemberPtrs) > 0;
  static constexpr std::size_t kOverloadCount = sizeof...(MemberPtrs);

  using AttributesType = AttributeList;
  using OverloadSetType = MemberOverloadSet<MemberPtrs...>;

  template <typename T>
  static constexpr bool IsOwnerOf() noexcept {
    if constexpr (kHasOverloads) {
      return ((std::same_as<std::remove_cvref_t<T>, typename MemberTraits<decltype(MemberPtrs)>::class_type>) || ...);
    }
    return false;
  }

  template <typename Attribute>
  static constexpr bool HasAttribute() noexcept {
    return Contains<Attribute, AttributesType>::value;
  }

  template <auto Value>
  static constexpr bool HasAttributeValue() noexcept {
    return SymbolHasAttributeValue<AttributesType, Value>();
  }

  template <typename Tag>
  static constexpr bool HasTag() noexcept {
    return SymbolHasAttributeTag<AttributesType, Tag>();
  }

  template <typename Signature>
  static constexpr bool HasOverloadWithSignature() noexcept {
    if constexpr (kHasOverloads) {
      return OverloadSetType::template HasOverloadWithSignature<Signature>();
    }
    return false;
  }

  template <typename Obj, typename... Args>
  static constexpr bool HasOverload() noexcept {
    if constexpr (kHasOverloads) {
      return OverloadSetType::template HasOverload<std::remove_cvref_t<Obj>, Args...>();
    }
    return false;
  }

  template <typename T, typename... Args>
    requires(kHasOverloads)
  static constexpr decltype(auto) Call(T&& obj, Args&&... args) {
    return OverloadSetType::template Call<T, Args...>(std::forward<T>(obj), std::forward<Args>(args)...);
  }

  template <typename T>
  static constexpr auto GetMember() noexcept {
    if constexpr (sizeof...(MemberPtrs) == 1) {
      return std::get<0>(std::make_tuple(MemberPtrs...));
    } else {
      return nullptr;
    }
  }

  template <typename T>
    requires(IsOwnerOf<std::remove_cvref_t<T>>())
  static constexpr decltype(auto) GetValue(T&& obj) noexcept {
    constexpr auto member = GetMember<std::decay_t<T>>();
    if constexpr (!std::same_as<std::remove_cvref_t<decltype(member)>, std::nullptr_t>) {
      using member_type = std::remove_cvref_t<decltype(member)>;
      return detail::MemberTraits<member_type>::Get(std::forward<T>(obj), member);
    } else {
      static_assert(sizeof(T) == 0, "Symbol has no member for this type");
    }
  }

  template <typename T, typename Value>
    requires(IsOwnerOf<std::remove_cvref_t<T>>())
  static constexpr void SetValue(T&& obj, Value&& value) {
    constexpr auto member = GetMember<std::decay_t<T>>();
    if constexpr (!std::same_as<std::remove_cvref_t<decltype(member)>, std::nullptr_t>) {
      using member_type = std::remove_cvref_t<decltype(member)>;
      detail::MemberTraits<member_type>::Set(std::forward<T>(obj), member, std::forward<Value>(value));
    } else {
      static_assert(sizeof(T) == 0, "Symbol has no member for this type");
    }
  }

  template <typename T>
  using MemberType =
      std::conditional_t<sizeof...(MemberPtrs) == 1, std::remove_reference_t<decltype(GetMember<T>())>, std::nullptr_t>;

  template <typename T>
  using ValueType = std::conditional_t<sizeof...(MemberPtrs) == 1 && !std::same_as<MemberType<T>, std::nullptr_t>,
                                       typename MemberTraits<MemberType<T>>::value_type, void>;
};

template <CompileTimeString Name, auto Ptr, typename... Attributes>
struct SymbolDefinition {
  static constexpr std::string_view kName = Name.View();
  static constexpr auto kPointer = Ptr;
  static constexpr HashType kHash = Fnv1aHash(kName);

  using AttributesType = TypeList<Attributes...>;
  using SymbolType = Symbol<Name, TypeList<Attributes...>, Ptr>;

  static constexpr auto MakeSymbol() noexcept { return SymbolType{}; }
};

template <CompileTimeString Name, typename AttributeList, auto... Pointers>
struct OverloadedSymbolDefinition {
  static constexpr std::string_view kName = Name.View();
  static constexpr HashType kHash = Fnv1aHash(kName);

  using AttributesType = AttributeList;
  using SymbolType = Symbol<Name, AttributeList, Pointers...>;

  static constexpr auto MakeSymbol() noexcept { return SymbolType{}; }
};

template <CompileTimeString Name, auto Ptr>
consteval auto MakeSymbolDefinition() noexcept {
  return SymbolDefinition<Name, Ptr>{};
}

template <CompileTimeString Name, auto Ptr, typename... Attributes>
consteval auto MakeAttributedSymbolDefinition() noexcept {
  return SymbolDefinition<Name, Ptr, Attributes...>{};
}

template <CompileTimeString Name, typename AttributeList, auto... Ptrs>
consteval auto MakeOverloadedSymbolDefinition() noexcept {
  return OverloadedSymbolDefinition<Name, AttributeList, Ptrs...>{};
}

}  // namespace ctti::detail

#endif  // CTTI_DETAIL_SYMBOL_IMPL_HPP
