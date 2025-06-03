#ifndef CTTI_CONSTRUCTOR_HPP
#define CTTI_CONSTRUCTOR_HPP

#include <ctti/detail/constructor_impl.hpp>
#include <ctti/type_tag.hpp>

namespace ctti {

template <typename T, typename... Args>
concept constructible = detail::Constructible<T, Args...>;

template <typename T, typename... Args>
concept nothrow_constructible = detail::NothrowConstructible<T, Args...>;

template <typename T>
concept default_constructible = detail::DefaultConstructible<T>;

template <typename T>
concept copy_constructible = detail::CopyConstructible<T>;

template <typename T>
concept move_constructible = detail::MoveConstructible<T>;

template <typename T>
concept aggregate_type = detail::AggregateType<T>;

template <typename T>
struct constructor_info {
  using type = T;

  template <typename... Args>
    requires constructible<T, Args...>
  static constexpr T construct(Args&&... args) noexcept(nothrow_constructible<T, Args...>) {
    return detail::ConstructorInfo<T>::Construct(std::forward<Args>(args)...);
  }

  template <typename... Args>
    requires constructible<T, Args...>
  static constexpr std::unique_ptr<T> make_unique(Args&&... args) {
    return detail::ConstructorInfo<T>::MakeUnique(std::forward<Args>(args)...);
  }

  template <typename... Args>
    requires constructible<T, Args...>
  static constexpr std::shared_ptr<T> make_shared(Args&&... args) {
    return detail::ConstructorInfo<T>::MakeShared(std::forward<Args>(args)...);
  }

  template <typename... Args>
  static constexpr bool can_construct() noexcept {
    return detail::ConstructorInfo<T>::template CanConstruct<Args...>();
  }

  template <typename... Args>
  static constexpr bool can_construct_nothrow() noexcept {
    return detail::ConstructorInfo<T>::template CanConstructNothrow<Args...>();
  }

  static constexpr bool is_default_constructible() noexcept {
    return detail::ConstructorInfo<T>::IsDefaultConstructible();
  }

  static constexpr bool is_copy_constructible() noexcept {
    return detail::ConstructorInfo<T>::IsCopyConstructible();
  }

  static constexpr bool is_move_constructible() noexcept {
    return detail::ConstructorInfo<T>::IsMoveConstructible();
  }

  static constexpr bool is_aggregate() noexcept {
    return detail::ConstructorInfo<T>::IsAggregate();
  }
};

template <typename T>
constexpr constructor_info<T> get_constructor_info() noexcept {
  return {};
}

template <typename T>
constexpr constructor_info<T> get_constructor_info(type_tag<T>) noexcept {
  return {};
}

template <typename T, typename... Args>
struct constructor_signature {
  using type = T;
  using args_tuple = std::tuple<Args...>;
  static constexpr std::size_t arity = detail::ConstructorSignature<T, Args...>::kArity;

  template <std::size_t I>
    requires(I < arity)
  using arg_type = typename detail::ConstructorSignature<T, Args...>::template ArgType<I>;

  static constexpr bool is_valid = detail::ConstructorSignature<T, Args...>::kIsValid;
  static constexpr bool is_nothrow = detail::ConstructorSignature<T, Args...>::kIsNothrow;
};

}  // namespace ctti

#endif  // CTTI_CONSTRUCTOR_HPP
