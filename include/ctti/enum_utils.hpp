#pragma once

#include <ctti/detail/enum_utils_impl.hpp>

#include <concepts>
#include <cstddef>
#include <string_view>
#include <type_traits>

namespace ctti {

/**
 * @brief Concept that checks if a type is a scoped enum (enum class).
 * @tparam E The type to check.
 */
template <typename E>
concept scoped_enum = detail::ScopedEnum<E>;

/**
 * @brief Concept that checks if a type is an unscoped enum (enum).
 * @tparam E The type to check.
 */
template <typename E>
concept unscoped_enum = detail::UnscopedEnum<E>;

/**
 * @brief Class that provides information about an enum type.
 * @tparam E The enum type.
 */
template <typename E>
  requires std::is_enum_v<E>
class enum_info {
private:
  using internal_info = detail::EnumInfo<E>;

public:
  using enum_type = E;
  using underlying_type = std::underlying_type_t<E>;

  /**
   * @brief Get the name of a specific enum value.
   * @tparam Value The enum value.
   * @return The name of the enum value as a string view.
   */
  template <E Value>
  [[nodiscard]] static constexpr std::string_view name_of_value() noexcept {
    return internal_info::template NameOfValue<Value>();
  }

  /**
   * @brief Get the underlying integer value of a specific enum value.
   * @tparam Value The enum value.
   * @return The underlying integer value of the enum value.
   */
  template <E Value>
  [[nodiscard]] static constexpr underlying_type underlying_value() noexcept {
    return internal_info::template UnderlyingValue<Value>();
  }

  /**
   * @brief Convert an underlying integer value to the corresponding enum value.
   * @tparam T The type of the underlying value (must be convertible to the enum's underlying type).
   * @param value The underlying integer value.
   * @return An optional containing the corresponding enum value if valid, otherwise std::nullopt.
   */
  template <typename T>
    requires std::convertible_to<T, underlying_type>
  [[nodiscard]] static constexpr std::optional<E> from_underlying(T value) noexcept {
    return internal_info::FromUnderlying(value);
  }

  /**
   * @brief Get the name of the enum type.
   * @return The name of the enum type as a string view.
   */
  [[nodiscard]] static constexpr std::string_view name() noexcept { return internal_info::Name(); }

  /**
   * @brief Check if the enum is scoped (enum class).
   * @return True if the enum is scoped, false otherwise.
   */
  [[nodiscard]] static constexpr bool is_scoped() noexcept { return internal_info::IsScoped(); }
};

/**
 * @brief Class that represents a list of enum values.
 * @tparam E The enum type.
 * @tparam Values The enum values.
 */
template <typename E, E... Values>
  requires std::is_enum_v<E>
class enum_value_list {
private:
  using internal_list = detail::EnumValueList<E, Values...>;

public:
  using enum_type = E;
  using values_type = typename internal_list::values;
  static constexpr std::size_t count = internal_list::kCount;

  /**
   * @brief Get an array of all enum values in the list.
   * @return An array containing all enum values.
   */
  [[nodiscard]] static constexpr auto value_array() noexcept -> std::array<E, count> {
    return internal_list::ValueArray();
  }

  /**
   * @brief Get the enum value at a specific index in the list.
   * @tparam I The index of the enum value to retrieve (must be less than count).
   * @return The enum value at the specified index.
   */
  template <std::size_t I>
    requires(I < count)
  [[nodiscard]] static constexpr E at() noexcept {
    return internal_list::template At<I>();
  }

  /**
   * @brief Check if a specific enum value is contained in the list.
   * @tparam Value The enum value to check.
   * @return True if the value is in the list, false otherwise.
   */
  template <E Value>
  [[nodiscard]] static constexpr bool contains() noexcept {
    return internal_list::template Contains<Value>();
  }

  /**
   * @brief Apply a function to each enum value in the list.
   * @tparam F The type of the function to apply (must be invocable with each enum value and its index).
   * @param func The function to apply to each enum value.
   */
  template <typename F>
    requires(std::invocable<const F&, detail::SizeType<static_cast<std::size_t>(Values)>, E> && ...)
  [[nodiscard]] static constexpr void for_each(const F& func) noexcept(
      noexcept(internal_list::ForEach(std::declval<const F&>()))) {
    internal_list::ForEach(func);
  }

  /**
   * @brief Get an array of the names of all enum values in the list.
   * @return An array containing the names of all enum values.
   */
  [[nodiscard]] static constexpr auto names() noexcept -> std::array<std::string_view, count> {
    return internal_list::Names();
  }
};

/**
 * @brief Create an enum_value_list from a list of enum values.
 * @tparam Values The enum values (must be of the same enum type).
 * @return An enum_value_list containing the provided enum values.
 */
template <auto... Values>
  requires detail::SameEnumValues<Values...> && (sizeof...(Values) > 0)
[[nodiscard]] constexpr auto make_enum_list() noexcept {
  using E = decltype((Values, ...));
  return enum_value_list<E, Values...>{};
}

/**
 * @brief Get the enum_info for a specific enum type.
 * @tparam E The enum type.
 * @return An enum_info object for the specified enum type.
 */
template <typename E>
  requires std::is_enum_v<E>
[[nodiscard]] constexpr enum_info<E> get_enum_info() noexcept {
  return {};
}

/**
 * @brief Get the name of a specific enum value.
 * @tparam E The enum type.
 * @tparam Value The enum value.
 * @return The name of the enum value as a string view.
 */
template <typename E, E Value>
  requires std::is_enum_v<E>
[[nodiscard]] constexpr std::string_view enum_name() noexcept {
  return enum_info<E>::template name_of_value<Value>();
}

/**
 * @brief Get the name of an enum type.
 * @tparam E The enum type.
 * @return The name of the enum type as a string view.
 */
template <typename E>
  requires std::is_enum_v<E>
[[nodiscard]] constexpr std::string_view enum_type_name() noexcept {
  return enum_info<E>::name();
}

/**
 * @brief Get the enum_info for the type of a specific enum object.
 * @tparam E The type of the enum object.
 * @return An enum_info object for the type of the specified enum object.
 */
template <typename E>
  requires std::is_enum_v<std::remove_cvref_t<E>>
[[nodiscard]] constexpr auto get_enum_info(const E& /*obj*/) noexcept -> enum_info<std::remove_cvref_t<E>> {
  return enum_info<std::remove_cvref_t<E>>{};
}

/**
 * @brief Get the name of the type of a specific enum object.
 * @tparam E The type of the enum object.
 * @param obj The enum object.
 * @return The name of the enum type as a string view.
 */
template <typename E>
  requires std::is_enum_v<std::remove_cvref_t<E>>
[[nodiscard]] constexpr std::string_view enum_type_name(const E& /*obj*/) noexcept {
  return enum_info<std::remove_cvref_t<E>>::name();
}

/**
 * @brief Get the underlying integer value of a specific enum value.
 * @tparam E The enum type.
 * @tparam Value The enum value.
 * @return The underlying integer value of the enum value.
 */
template <typename E, E Value>
  requires std::is_enum_v<E>
[[nodiscard]] constexpr auto enum_underlying_value() noexcept {
  return enum_info<E>::template underlying_value<Value>();
}

/**
 * @brief Checks if two enum values are equal.
 * @tparam E The enum type.
 * @tparam Lhs The left-hand side enum value.
 * @tparam Rhs The right-hand side enum value.
 * @return True if the enum values are equal, false otherwise.
 */
template <typename E, E Lhs, E Rhs>
  requires std::is_enum_v<E>
[[nodiscard]] constexpr bool enum_equal() noexcept {
  return detail::EnumEqual<E, Lhs, Rhs>();
}

/**
 * @brief Checks if one enum value is less than another.
 * @tparam E The enum type.
 * @tparam Lhs The left-hand side enum value.
 * @tparam Rhs The right-hand side enum value.
 * @return True if Lhs is less than Rhs, false otherwise.
 */
template <typename E, E Lhs, E Rhs>
  requires std::is_enum_v<E>
[[nodiscard]] constexpr bool enum_less() noexcept {
  return detail::EnumLess<E, Lhs, Rhs>();
}

}  // namespace ctti
