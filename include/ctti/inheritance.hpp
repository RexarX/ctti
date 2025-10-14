#pragma once

#include <ctti/detail/inheritance_impl.hpp>

#include <cstddef>
#include <string_view>
#include <type_traits>

namespace ctti {

/**
 * @brief Concept that checks if `Derived` is derived from `Base`.
 * @tparam Derived The derived class type.
 * @tparam Base The base class type.
 */
template <typename Derived, typename Base>
concept derived_from = detail::DerivedFrom<Derived, Base>;

/**
 * @brief Concept that checks if `Derived` is publicly derived from `Base`.
 * @tparam Derived The derived class type.
 * @tparam Base The base class type.
 */
template <typename Derived, typename Base>
concept publicly_derived_from = detail::PubliclyDerivedFrom<Derived, Base>;

/**
 * @brief Concept that checks if a type is polymorphic (has at least one virtual function).
 * @tparam T The type to check.
 */
template <typename T>
concept polymorphic = detail::Polymorphic<T>;

/**
 * @brief Concept that checks if a type is abstract (cannot be instantiated).
 * @tparam T The type to check.
 */
template <typename T>
concept abstract = detail::Abstract<T>;

/**
 * @brief Concept that checks if a type is final (cannot be inherited from).
 * @tparam T The type to check.
 */
template <typename T>
concept final_type = detail::Final<T>;

/**
 * @brief Concept that checks if a type has a virtual destructor.
 * @tparam T The type to check.
 */
template <typename T>
concept has_virtual_destructor = detail::HasVirtualDestructor<T>;

/**
 * @brief Provides information about the inheritance relationship between two types.
 * @tparam Derived The derived class type.
 * @tparam Base The base class type.
 */
template <typename Derived, typename Base>
  requires derived_from<Derived, Base>
class inheritance_info {
private:
  using internal_info = detail::InheritanceInfo<Derived, Base>;

public:
  using derived_type = Derived;
  using base_type = Base;

  static constexpr bool is_derived = internal_info::kIsDerived;
  static constexpr bool is_public_derived = internal_info::kIsPublicDerived;
  static constexpr bool is_virtual_base = internal_info::kIsVirtualBase;

  /**
   * @brief Returns the name of the derived class.
   * @return A string view representing the name of the derived class.
   */
  [[nodiscard]] static constexpr std::string_view derived_name() noexcept { return internal_info::DerivedName(); }

  /**
   * @brief Returns the name of the base class.
   * @return A string view representing the name of the base class.
   */
  [[nodiscard]] static constexpr std::string_view base_name() noexcept { return internal_info::BaseName(); }
};

/**
 * @brief Provides a list of base classes for a given type.
 * @tparam T The derived class type.
 * @tparam Bases The base class types.
 */
template <typename T, typename... Bases>
  requires(derived_from<T, Bases> && ...)
class base_list {
private:
  using internal_list = detail::BaseList<T, Bases...>;

public:
  using type = T;
  using bases_type = typename internal_list::bases;

  static constexpr std::size_t count = internal_list::kCount;
  template <std::size_t I>
    requires(I < count)
  using base = typename internal_list::template Base<I>;

  /**
   * @brief Checks if the specified base class is in the list of base classes.
   * @tparam Base The base class type to check.
   * @return true if the base class is in the list, otherwise false.
   */
  template <typename Base>
  [[nodiscard]] static constexpr bool has_base() noexcept {
    return internal_list::template HasBase<Base>();
  }

  /**
   * @brief Executes a provided function for each base class in the list.
   * @tparam F The type of the function to execute.
   * @param func The function to execute for each base class.
   */
  template <typename F>
    requires(std::invocable<const F&, detail::Identity<Bases>> && ...)
  static constexpr void for_each_base(const F& func) noexcept(
      noexcept(internal_list::ForEachBase(std::declval<const F&>()))) {
    internal_list::ForEachBase(func);
  }
};

/**
 * @brief Provides information about the polymorphic properties of a type.
 * @tparam T The type to check.
 */
template <typename T>
class polymorphism_info {
private:
  using internal_info = detail::PolymorphismInfo<T>;

public:
  using type = T;

  static constexpr bool is_polymorphic = internal_info::kIsPolymorphic;
  static constexpr bool is_abstract = internal_info::kIsAbstract;
  static constexpr bool is_final = internal_info::kIsFinal;
  static constexpr bool has_virtual_destructor = internal_info::kHasVirtualDestructor;

  /**
   * @brief Returns the name of the type.
   * @return A string view representing the name of the type.
   */
  static constexpr std::string_view name() noexcept { return internal_info::Name(); }
};

/**
 * @brief Checks if `Derived` is derived from `Base`.
 * @tparam Derived The derived class type.
 * @tparam Base The base class type.
 * @return true if `Derived` is derived from `Base`, otherwise false.
 */
template <typename Derived, typename Base>
[[nodiscard]] constexpr bool is_derived_from() noexcept {
  return detail::IsDerivedFrom<Derived, Base>();
}

/**
 * @brief Checks if `Derived` is publicly derived from `Base`.
 * @tparam Derived The derived class type.
 * @tparam Base The base class type.
 * @return true if `Derived` is publicly derived from `Base`, otherwise false.
 */
template <typename Derived, typename Base>
[[nodiscard]] constexpr bool is_publicly_derived_from() noexcept {
  return detail::IsPubliclyDerivedFrom<Derived, Base>();
}

/**
 * @brief Checks if a type is polymorphic (has at least one virtual function).
 * @tparam T The type to check.
 * @return true if the type is polymorphic, otherwise false.
 */
template <typename T>
[[nodiscard]] constexpr bool is_polymorphic() noexcept {
  return detail::IsPolymorphic<T>();
}

/**
 * @brief Checks if a type is abstract (cannot be instantiated).
 * @tparam T The type to check.
 * @return true if the type is abstract, otherwise false.
 */
template <typename T>
[[nodiscard]] constexpr bool is_abstract() noexcept {
  return detail::IsAbstract<T>();
}

/**
 * @brief Checks if a type is final (cannot be inherited from).
 * @tparam T The type to check.
 * @return true if the type is final, otherwise false.
 */
template <typename T>
[[nodiscard]] constexpr bool is_final() noexcept {
  return detail::IsFinal<T>();
}

/**
 * @brief Checks if a type has a virtual destructor.
 * @tparam T The type to check.
 * @return true if the type has a virtual destructor, otherwise false.
 */
template <typename T>
[[nodiscard]] constexpr auto get_polymorphism_info() noexcept -> polymorphism_info<T> {
  return {};
}

/**
 * @brief Retrieves inheritance information between `Derived` and `Base`.
 * @tparam Derived The derived class type.
 * @tparam Base The base class type.
 * @return An instance of `inheritance_info<Derived, Base>`.
 */
template <typename Derived, typename Base>
  requires derived_from<Derived, Base>
[[nodiscard]] constexpr auto get_inheritance_info() noexcept -> inheritance_info<Derived, Base> {
  return {};
}

/**
 * @brief Retrieves polymorphism information for the type of the given object.
 * @tparam T The type of the object.
 * @param obj The object to analyze.
 * @return An instance of `polymorphism_info<T>`.
 */
template <typename T>
constexpr auto get_polymorphism_info(const T& /*obj*/) noexcept -> polymorphism_info<std::remove_cvref_t<T>> {
  return {};
}

/**
 * @brief Checks if the type of the given object is polymorphic (has at least one virtual function).
 * @tparam T The type of the object.
 * @param obj The object to analyze.
 * @return true if the type is polymorphic, otherwise false.
 */
template <typename T>
[[nodiscard]] constexpr bool is_polymorphic(const T& /*obj*/) noexcept {
  return detail::IsPolymorphic<std::remove_cvref_t<T>>();
}

/**
 * @brief Checks if the type of the given object is abstract (cannot be instantiated).
 * @tparam T The type of the object.
 * @param obj The object to analyze.
 * @return true if the type is abstract, otherwise false.
 */
template <typename T>
[[nodiscard]] constexpr bool is_abstract(const T& /*obj*/) noexcept {
  return detail::IsAbstract<std::remove_cvref_t<T>>();
}

/**
 * @brief Checks if the type of the given object is final (cannot be inherited from).
 * @tparam T The type of the object.
 * @param obj The object to analyze.
 * @return true if the type is final, otherwise false.
 */
template <typename T>
[[nodiscard]] constexpr bool is_final(const T& /*obj*/) noexcept {
  return detail::IsFinal<std::remove_cvref_t<T>>();
}

/**
 * @brief Checks if the type of the given object has a virtual destructor.
 * @tparam T The type of the object.
 * @param obj The object to analyze.
 * @return true if the type has a virtual destructor, otherwise false.
 */
template <typename To, typename From>
  requires publicly_derived_from<From, To> || publicly_derived_from<To, From>
[[nodiscard]] constexpr To* safe_cast(From* ptr) noexcept {
  return detail::SafeCast<To>(ptr);
}

/**
 * @brief Safely casts a pointer from `From` type to `To` type if they are publicly related.
 * @tparam To The target type to cast to.
 * @tparam From The source type to cast from.
 * @param ptr The pointer of type `From` to be cast.
 * @return A const pointer of type `To` if the cast is valid, otherwise `nullptr`.
 */
template <typename To, typename From>
  requires publicly_derived_from<From, To> || publicly_derived_from<To, From>
[[nodiscard]] constexpr const To* safe_cast(const From* ptr) noexcept {
  return detail::SafeCast<To>(ptr);
}

/**
 * @brief Safely performs a dynamic cast on a polymorphic type.
 * @tparam To The target type to cast to.
 * @tparam From The source polymorphic type to cast from.
 * @param ptr The pointer of type `From` to be cast.
 * @return A pointer of type `To` if the cast is valid, otherwise `nullptr`.
 */
template <typename To, polymorphic From>
[[nodiscard]] To* dynamic_cast_safe(From* ptr) noexcept {
  return detail::DynamicCastSafe<To>(ptr);
}

/**
 * @brief Safely performs a dynamic cast on a polymorphic type.
 * @tparam To The target type to cast to.
 * @tparam From The source polymorphic type to cast from.
 * @param ptr The pointer of type `From` to be cast.
 * @return A const pointer of type `To` if the cast is valid, otherwise `nullptr`.
 */
template <typename To, polymorphic From>
[[nodiscard]] const To* dynamic_cast_safe(const From* ptr) noexcept {
  return detail::DynamicCastSafe<To>(ptr);
}

}  // namespace ctti
