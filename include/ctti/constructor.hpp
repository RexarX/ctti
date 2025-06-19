#pragma once

#include <ctti/detail/constructor_impl.hpp>
#include <ctti/type_tag.hpp>

#include <memory>
#include <type_traits>
#include <utility>

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
class constructor_info {
private:
  using internal_info = detail::ConstructorInfo<T>;

public:
  using type = T;

  template <typename... Args>
    requires constructible<T, Args...>
  static constexpr T construct(Args&&... args) noexcept(nothrow_constructible<T, Args...>) {
    return internal_info::Construct(std::forward<Args>(args)...);
  }

  template <typename... Args>
    requires constructible<T, Args...>
  static constexpr std::unique_ptr<T> make_unique(Args&&... args) {
    return internal_info::MakeUnique(std::forward<Args>(args)...);
  }

  template <typename... Args>
    requires constructible<T, Args...>
  static constexpr std::shared_ptr<T> make_shared(Args&&... args) {
    return internal_info::MakeShared(std::forward<Args>(args)...);
  }

  template <typename... Args>
  static constexpr bool can_construct() noexcept {
    return internal_info::template CanConstruct<Args...>();
  }

  template <typename... Args>
  static constexpr bool can_construct_nothrow() noexcept {
    return internal_info::template CanConstructNothrow<Args...>();
  }

  static constexpr bool is_default_constructible() noexcept { return internal_info::IsDefaultConstructible(); }
  static constexpr bool is_copy_constructible() noexcept { return internal_info::IsCopyConstructible(); }
  static constexpr bool is_move_constructible() noexcept { return internal_info::IsMoveConstructible(); }
  static constexpr bool is_aggregate() noexcept { return internal_info::IsAggregate(); }
};

template <typename T>
constexpr constructor_info<T> get_constructor_info() noexcept {
  return {};
}

template <typename T>
constexpr constructor_info<T> get_constructor_info([[maybe_unused]] type_tag<T> tag) noexcept {
  return {};
}

template <typename T>
constexpr constructor_info<std::remove_cvref_t<T>> get_constructor_info([[maybe_unused]] const T& obj) noexcept {
  return {};
}

template <typename T, typename... Args>
class constructor_signature {
private:
  using internal_signature = detail::ConstructorSignature<T, Args...>;

public:
  using type = T;
  using args_tuple = std::tuple<Args...>;

  static constexpr std::size_t arity = internal_signature::kArity;
  static constexpr bool is_valid = internal_signature::kIsValid;
  static constexpr bool is_nothrow = internal_signature::kIsNothrow;

  template <std::size_t I>
    requires(I < arity)
  using arg_type = typename internal_signature::template ArgType<I>;
};

}  // namespace ctti
