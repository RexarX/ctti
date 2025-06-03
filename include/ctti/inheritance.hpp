#ifndef CTTI_INHERITANCE_HPP
#define CTTI_INHERITANCE_HPP

#include <ctti/detail/inheritance_impl.hpp>

namespace ctti {

template <typename Derived, typename Base>
concept derived_from = detail::DerivedFrom<Derived, Base>;

template <typename Derived, typename Base>
concept publicly_derived_from = detail::PubliclyDerivedFrom<Derived, Base>;

template <typename T>
concept polymorphic = detail::Polymorphic<T>;

template <typename T>
concept abstract = detail::Abstract<T>;

template <typename T>
concept final_type = detail::Final<T>;

template <typename T>
concept has_virtual_destructor = detail::HasVirtualDestructor<T>;

template <typename Derived, typename Base>
  requires derived_from<Derived, Base>
struct inheritance_info {
  static constexpr bool is_derived = detail::InheritanceInfo<Derived, Base>::kIsDerived;
  static constexpr bool is_public_derived = detail::InheritanceInfo<Derived, Base>::kIsPublicDerived;
  static constexpr bool is_virtual_base = detail::InheritanceInfo<Derived, Base>::kIsVirtualBase;

  using derived_type = Derived;
  using base_type = Base;

  static constexpr std::string_view derived_name() noexcept {
    return detail::InheritanceInfo<Derived, Base>::DerivedName();
  }

  static constexpr std::string_view base_name() noexcept { return detail::InheritanceInfo<Derived, Base>::BaseName(); }
};

template <typename T, typename... Bases>
  requires(derived_from<T, Bases> && ...)
struct base_list {
  using type = T;
  using bases = typename detail::BaseList<T, Bases...>::bases;
  static constexpr std::size_t count = detail::BaseList<T, Bases...>::kCount;

  template <std::size_t I>
    requires(I < count)
  using base = typename detail::BaseList<T, Bases...>::template Base<I>;

  template <typename Base>
  static constexpr bool has_base() noexcept {
    return detail::BaseList<T, Bases...>::template HasBase<Base>();
  }

  template <typename F>
    requires(std::invocable<F, detail::Identity<Bases>> && ...)
  static constexpr void for_each_base(F&& f) {
    detail::BaseList<T, Bases...>::ForEachBase(std::forward<F>(f));
  }
};

template <typename T>
struct polymorphism_info {
  static constexpr bool is_polymorphic = detail::PolymorphismInfo<T>::kIsPolymorphic;
  static constexpr bool is_abstract = detail::PolymorphismInfo<T>::kIsAbstract;
  static constexpr bool is_final = detail::PolymorphismInfo<T>::kIsFinal;
  static constexpr bool has_virtual_destructor = detail::PolymorphismInfo<T>::kHasVirtualDestructor;

  using type = T;

  static constexpr std::string_view name() noexcept { return detail::PolymorphismInfo<T>::Name(); }
};

template <typename Derived, typename Base>
constexpr bool is_derived_from() noexcept {
  return detail::IsDerivedFrom<Derived, Base>();
}

template <typename Derived, typename Base>
constexpr bool is_publicly_derived_from() noexcept {
  return detail::IsPubliclyDerivedFrom<Derived, Base>();
}

template <typename T>
constexpr bool is_polymorphic() noexcept {
  return detail::IsPolymorphic<T>();
}

template <typename T>
constexpr bool is_abstract() noexcept {
  return detail::IsAbstract<T>();
}

template <typename T>
constexpr bool is_final() noexcept {
  return detail::IsFinal<T>();
}

template <typename T>
constexpr polymorphism_info<T> get_polymorphism_info() noexcept {
  return {};
}

template <typename Derived, typename Base>
  requires derived_from<Derived, Base>
constexpr inheritance_info<Derived, Base> get_inheritance_info() noexcept {
  return {};
}

template <typename To, typename From>
  requires publicly_derived_from<From, To> || publicly_derived_from<To, From>
constexpr To* safe_cast(From* ptr) noexcept {
  return detail::SafeCast<To>(ptr);
}

template <typename To, typename From>
  requires publicly_derived_from<From, To> || publicly_derived_from<To, From>
constexpr const To* safe_cast(const From* ptr) noexcept {
  return detail::SafeCast<To>(ptr);
}

template <typename To, typename From>
  requires polymorphic<From>
To* dynamic_cast_safe(From* ptr) noexcept {
  return detail::DynamicCastSafe<To>(ptr);
}

template <typename To, typename From>
  requires polymorphic<From>
const To* dynamic_cast_safe(const From* ptr) noexcept {
  return detail::DynamicCastSafe<To>(ptr);
}

}  // namespace ctti

#endif  // CTTI_INHERITANCE_HPP
