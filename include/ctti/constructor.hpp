#pragma once

#include <ctti/detail/constructor_impl.hpp>
#include <ctti/type_tag.hpp>

#include <cstddef>
#include <memory>
#include <tuple>
#include <type_traits>
#include <utility>

namespace ctti {

/**
 * @brief Checks if a type T is constructible with the given argument types Args.
 * @tparam T The type to check for constructibility.
 * @tparam Args The argument types to check against.
 */
template <typename T, typename... Args>
concept constructible = detail::Constructible<T, Args...>;

/**
 * @brief Checks if a type T is nothrow constructible with the given argument types Args.
 * @tparam T The type to check for nothrow constructibility.
 * @tparam Args The argument types to check against.
 */
template <typename T, typename... Args>
concept nothrow_constructible = detail::NothrowConstructible<T, Args...>;

/**
 * @brief Checks if a type T is default constructible.
 * @tparam T The type to check for default constructibility.
 */
template <typename T>
concept default_constructible = detail::DefaultConstructible<T>;

/**
 * @brief Checks if a type T is copy constructible.
 * @tparam T The type to check for copy constructibility.
 */
template <typename T>
concept copy_constructible = detail::CopyConstructible<T>;

/**
 * @brief Checks if a type T is move constructible.
 * @tparam T The type to check for move constructibility.
 */
template <typename T>
concept move_constructible = detail::MoveConstructible<T>;

/**
 * @brief Checks if a type T is an aggregate type.
 * @tparam T The type to check for being an aggregate.
 */
template <typename T>
concept aggregate_type = detail::AggregateType<T>;

/**
 * @brief Provides information and utilities related to the constructors of a type T.
 * @tparam T The type for which to provide constructor information.
 */
template <typename T>
class constructor_info {
private:
  using internal_info = detail::ConstructorInfo<T>;

public:
  using type = T;

  /**
   * @brief Constructs an instance of type T using the provided arguments.
   * @tparam Args The argument types for the constructor.
   * @param args The arguments to pass to the constructor.
   * @return An instance of type T constructed with the provided arguments.
   */
  template <typename... Args>
    requires constructible<T, Args...>
  [[nodiscard]] static constexpr T construct(Args&&... args) noexcept(nothrow_constructible<T, Args...>) {
    return internal_info::Construct(std::forward<Args>(args)...);
  }

  /**
   * @brief Creates a std::unique_ptr to an instance of type T using the provided arguments.
   * @tparam Args The argument types for the constructor.
   * @param args The arguments to pass to the constructor.
   * @return A std::unique_ptr to an instance of type T constructed with the provided
   */
  template <typename... Args>
    requires constructible<T, Args...>
  [[nodiscard]] static constexpr std::unique_ptr<T> make_unique(Args&&... args) {
    return internal_info::MakeUnique(std::forward<Args>(args)...);
  }

  /**
   * @brief Creates a std::shared_ptr to an instance of type T using the provided arguments.
   * @tparam Args The argument types for the constructor.
   * @param args The arguments to pass to the constructor.
   * @return A std::shared_ptr to an instance of type T constructed with the provided
   */
  template <typename... Args>
    requires constructible<T, Args...>
  [[nodiscard]] static constexpr std::shared_ptr<T> make_shared(Args&&... args) {
    return internal_info::MakeShared(std::forward<Args>(args)...);
  }

  /**
   * @brief Checks if type T can be constructed with the given argument types Args.
   * @tparam Args The argument types to check against.
   * @return true if T can be constructed with Args, false otherwise.
   */
  template <typename... Args>
  [[nodiscard]] static constexpr bool can_construct() noexcept {
    return internal_info::template CanConstruct<Args...>();
  }

  /**
   * @brief Checks if type T can be constructed with the given argument types Args without throwing exceptions.
   * @tparam Args The argument types to check against.
   * @return true if T can be constructed with Args without throwing, false otherwise.
   */
  template <typename... Args>
  [[nodiscard]] static constexpr bool can_construct_nothrow() noexcept {
    return internal_info::template CanConstructNothrow<Args...>();
  }

  /**
   * @brief Checks if type T is default constructible.
   * @return true if T is default constructible, false otherwise.
   */
  [[nodiscard]] static constexpr bool is_default_constructible() noexcept {
    return internal_info::IsDefaultConstructible();
  }

  /**
   * @brief Checks if type T is copy constructible.
   * @return true if T is copy constructible, false otherwise.
   */
  [[nodiscard]] static constexpr bool is_copy_constructible() noexcept { return internal_info::IsCopyConstructible(); }

  /**
   * @brief Checks if type T is move constructible.
   * @return true if T is move constructible, false otherwise.
   */
  [[nodiscard]] static constexpr bool is_move_constructible() noexcept { return internal_info::IsMoveConstructible(); }

  /**
   * @brief Checks if type T is an aggregate type.
   * @return true if T is an aggregate type, false otherwise.
   */
  [[nodiscard]] static constexpr bool is_aggregate() noexcept { return internal_info::IsAggregate(); }
};

/**
 * @brief Retrieves constructor information for the specified type T.
 * @tparam T The type for which to retrieve constructor information.
 * @return An instance of constructor_info<T> containing information about T's constructors.
 */
template <typename T>
[[nodiscard]] constexpr auto get_constructor_info() noexcept -> constructor_info<T> {
  return {};
}

/**
 * @brief Retrieves constructor information for the type T deduced from the provided type tag.
 * @tparam T The type for which to retrieve constructor information.
 * @param tag A type tag representing the type T.
 * @return An instance of constructor_info<T> containing information about T's constructors.
 */
template <typename T>
[[nodiscard]] constexpr auto get_constructor_info(type_tag<T> /*tag*/) noexcept -> constructor_info<T> {
  return {};
}

/**
 * @brief Retrieves constructor information for the type T deduced from the provided object.
 * @tparam T The type for which to retrieve constructor information.
 * @param obj An object of type T.
 * @return An instance of constructor_info<std::remove_cvref_t<T>> containing information about
 */
template <typename T>
[[nodiscard]] constexpr auto get_constructor_info(const T& /*obj*/) noexcept
    -> constructor_info<std::remove_cvref_t<T>> {
  return {};
}

/**
 * @brief Provides a compile-time signature of a constructor for type T with argument types Args.
 * @tparam T The type for which to provide the constructor signature.
 * @tparam Args The argument types of the constructor.
 */
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
