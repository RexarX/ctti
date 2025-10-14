#pragma once

#include <ctti/detail/meta.hpp>
#include <ctti/detail/name_impl.hpp>

#include <concepts>
#include <cstddef>
#include <string_view>
#include <type_traits>

namespace ctti::detail {

template <typename Derived, typename Base>
concept DerivedFrom = std::is_base_of_v<Base, Derived>;

template <typename Derived, typename Base>
concept PubliclyDerivedFrom = std::derived_from<Derived, Base>;

template <typename T>
concept Polymorphic = std::is_polymorphic_v<T>;

template <typename T>
concept Abstract = std::is_abstract_v<T>;

template <typename T>
concept Final = std::is_final_v<T>;

template <typename T>
concept HasVirtualDestructor = std::has_virtual_destructor_v<T>;

template <typename Derived, typename Base>
  requires DerivedFrom<Derived, Base>
struct InheritanceInfo {
  using derived_type = Derived;
  using base_type = Base;

  static constexpr bool kIsDerived = true;
  static constexpr bool kIsPublicDerived = PubliclyDerivedFrom<Derived, Base>;
  static constexpr bool kIsVirtualBase = false;

  [[nodiscard]] static constexpr std::string_view DerivedName() noexcept { return NameOfImpl<Derived>::Apply(); }
  [[nodiscard]] static constexpr std::string_view BaseName() noexcept { return NameOfImpl<Base>::Apply(); }
};

template <typename T, typename... Bases>
  requires(DerivedFrom<T, Bases> && ...)
struct BaseList {
  using type = T;
  using bases = TypeList<Bases...>;

  static constexpr std::size_t kCount = sizeof...(Bases);

  template <std::size_t I>
    requires(I < kCount)
  using Base = typename bases::template At<I>;

  template <typename F>
    requires(std::invocable<F, Identity<Bases>> && ...)
  static constexpr void ForEachBase(const F& func) noexcept((std::is_nothrow_invocable_v<const F&, Identity<Bases>> &&
                                                             ...)) {
    (func(Identity<Bases>{}), ...);
  }

  template <typename Base>
  [[nodiscard]] static constexpr bool HasBase() noexcept {
    return Contains<Base, bases>::value;
  }
};

template <typename T>
struct PolymorphismInfo {
  using type = T;

  static constexpr bool kIsPolymorphic = Polymorphic<T>;
  static constexpr bool kIsAbstract = Abstract<T>;
  static constexpr bool kIsFinal = Final<T>;
  static constexpr bool kHasVirtualDestructor = HasVirtualDestructor<T>;

  static constexpr std::string_view Name() noexcept { return NameOfImpl<T>::Apply(); }
};

template <typename Derived, typename Base>
[[nodiscard]] constexpr bool IsDerivedFrom() noexcept {
  return DerivedFrom<Derived, Base>;
}

template <typename Derived, typename Base>
[[nodiscard]] constexpr bool IsPubliclyDerivedFrom() noexcept {
  return PubliclyDerivedFrom<Derived, Base>;
}

template <typename T>
[[nodiscard]] constexpr bool IsPolymorphic() noexcept {
  return Polymorphic<T>;
}

template <typename T>
[[nodiscard]] constexpr bool IsAbstract() noexcept {
  return Abstract<T>;
}

template <typename T>
[[nodiscard]] constexpr bool IsFinal() noexcept {
  return Final<T>;
}

template <typename To, typename From>
  requires PubliclyDerivedFrom<From, To> || PubliclyDerivedFrom<To, From>
[[nodiscard]] constexpr To* SafeCast(From* ptr) noexcept {
  return static_cast<To*>(ptr);
}

template <typename To, typename From>
  requires PubliclyDerivedFrom<From, To> || PubliclyDerivedFrom<To, From>
[[nodiscard]] constexpr const To* SafeCast(const From* ptr) noexcept {
  return static_cast<const To*>(ptr);
}

template <typename To, Polymorphic From>

[[nodiscard]] To* DynamicCastSafe(From* ptr) noexcept {
  return dynamic_cast<To*>(ptr);
}

template <typename To, Polymorphic From>
[[nodiscard]] const To* DynamicCastSafe(const From* ptr) noexcept {
  return dynamic_cast<const To*>(ptr);
}

}  // namespace ctti::detail
