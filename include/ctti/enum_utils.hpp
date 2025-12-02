#pragma once

#include <ctti/detail/enum_utils_impl.hpp>

#include <concepts>
#include <cstddef>
#include <optional>
#include <string_view>
#include <type_traits>
#include <utility>

namespace ctti {

/**
 * @brief Primary template for enum value registration.
 * @details Specialize this template to register enum values for runtime operations.
 * This enables centralized enum value registration similar to the reflection system.
 * @tparam E The enum type.
 *
 * @example
 * @code
 * enum class Color { Red, Green, Blue };
 *
 * template<>
 * struct ctti::enum_values<Color> {
 *   static constexpr auto values = ctti::make_enum_list<Color::Red, Color::Green, Color::Blue>();
 * };
 *
 * auto name = ctti::enum_name(Color::Red);  // "Red"
 * auto val = ctti::enum_cast<Color>("Green");  // Color::Green
 * @endcode
 */
template <typename E>
  requires std::is_enum_v<E>
struct enum_values {
  // Default: no values registered
  static constexpr bool registered = false;
};

namespace detail {

template <typename E>
concept HasRegisteredEnumValues = requires {
  { enum_values<E>::values } -> std::convertible_to<decltype(enum_values<E>::values)>;
  requires enum_values<E>::values.count > 0;
};

}  // namespace detail

/**
 * @brief Concept that checks if an enum type has registered values via enum_values<E> specialization.
 * @tparam E The enum type to check.
 */
template <typename E>
concept registered_enum = std::is_enum_v<E> && detail::HasRegisteredEnumValues<E>;

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
class enum_info final {
private:
  using internal_info = detail::EnumInfo<E>;

public:
  using enum_type = E;
  using underlying_type = std::underlying_type_t<E>;

  /**
   * @brief Get the name of a specific enum value (compile-time).
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
 * @brief Class that represents a list of enum values with runtime capabilities.
 *
 * This class provides magic_enum-like functionality when you explicitly specify
 * the enum values. Use make_enum_list to create instances.
 *
 * @tparam E The enum type.
 * @tparam Values The enum values.
 */
template <typename E, E... Values>
  requires std::is_enum_v<E>
class enum_value_list final {
private:
  using internal_list = detail::EnumValueList<E, Values...>;

public:
  using enum_type = E;
  using underlying_type = std::underlying_type_t<E>;
  using values_type = typename internal_list::values;

  /// Number of enum values in this list.
  static constexpr std::size_t count = internal_list::kCount;

  /**
   * @brief Get an array of all enum values in the list.
   * @return Const reference to array containing all enum values.
   */
  [[nodiscard]] static constexpr const auto& values() noexcept { return internal_list::ValueArray(); }

  /**
   * @brief Get an array of all enum value names in the list.
   * @return Const reference to array containing all names.
   */
  [[nodiscard]] static constexpr const auto& names() noexcept { return internal_list::NameArray(); }

  /**
   * @brief Get all entries (value, name pairs).
   * @return Const reference to array of entries.
   */
  [[nodiscard]] static constexpr const auto& entries() noexcept { return internal_list::kEntries; }

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
   * @brief Get enum value at runtime index.
   * @param index The index.
   * @return Optional containing value if index valid, nullopt otherwise.
   */
  [[nodiscard]] static constexpr std::optional<E> value_at(std::size_t index) noexcept {
    return internal_list::ValueAt(index);
  }

  /**
   * @brief Check if a specific enum value is contained in the list (compile-time).
   * @tparam Value The enum value to check.
   * @return True if the value is in the list, false otherwise.
   */
  template <E Value>
  [[nodiscard]] static constexpr bool contains() noexcept {
    return internal_list::template Contains<Value>();
  }

  /**
   * @brief Check if a specific enum value is contained in the list (runtime).
   * @param value The enum value to check.
   * @return True if the value is in the list, false otherwise.
   */
  [[nodiscard]] static constexpr bool contains(E value) noexcept { return internal_list::ContainsRuntime(value); }

  /**
   * @brief Get the name of an enum value (runtime).
   * @param value The enum value.
   * @return Optional containing the name if found, nullopt otherwise.
   */
  [[nodiscard]] static constexpr std::optional<std::string_view> name_of(E value) noexcept {
    return internal_list::NameOf(value);
  }

  /**
   * @brief Convert a string to enum value.
   * @param name The name to look up.
   * @return Optional containing the value if found, nullopt otherwise.
   */
  [[nodiscard]] static constexpr std::optional<E> cast(std::string_view name) noexcept {
    return internal_list::Cast(name);
  }

  /**
   * @brief Get the index of an enum value.
   * @param value The enum value.
   * @return Optional containing the index if found, nullopt otherwise.
   */
  [[nodiscard]] static constexpr std::optional<std::size_t> index_of(E value) noexcept {
    return internal_list::IndexOf(value);
  }

  /**
   * @brief Convert underlying value to enum, validating against list.
   * @param value The underlying value.
   * @return Optional containing enum if valid, nullopt otherwise.
   */
  [[nodiscard]] static constexpr std::optional<E> from_underlying(underlying_type value) noexcept {
    return internal_list::FromUnderlying(value);
  }

  /**
   * @brief Apply a function to each enum value in the list.
   * @tparam F The type of the function to apply (must be invocable with each enum value and its index).
   * @param func The function to apply to each enum value.
   */
  template <typename F>
    requires(std::invocable<const F&, detail::SizeType<static_cast<std::size_t>(Values)>, E> && ...)
  static constexpr void for_each(const F& func) noexcept(noexcept(internal_list::ForEach(std::declval<const F&>()))) {
    internal_list::ForEach(func);
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

// ============================================================================
// Free Functions
// ============================================================================

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
 * @brief Get the name of a specific enum value (compile-time).
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
 * @brief Get the underlying integer value of a specific enum value (compile-time).
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
 * @brief Get the underlying integer value of an enum value (runtime).
 * @tparam E The enum type.
 * @param value The enum value.
 * @return The underlying integer value.
 */
template <typename E>
  requires std::is_enum_v<E>
[[nodiscard]] constexpr auto enum_underlying(E value) noexcept {
  return static_cast<std::underlying_type_t<E>>(value);
}

/**
 * @brief Checks if two enum values are equal (compile-time).
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
 * @brief Checks if one enum value is less than another (compile-time).
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

// ============================================================================
// Registered Enum Functions (use enum_values<E> specialization)
// ============================================================================

/**
 * @brief Get the name of an enum value at runtime using registered values.
 * @tparam E The enum type (must have enum_values<E> specialization).
 * @param value The enum value.
 * @return Optional containing the name if found, nullopt otherwise.
 */
template <registered_enum E>
[[nodiscard]] constexpr std::optional<std::string_view> enum_name(E value) noexcept {
  return enum_values<E>::values.name_of(value);
}

/**
 * @brief Convert a string to an enum value using registered values.
 * @tparam E The enum type (must have enum_values<E> specialization).
 * @param name The string name to convert.
 * @return Optional containing the enum value if found, nullopt otherwise.
 */
template <registered_enum E>
[[nodiscard]] constexpr std::optional<E> enum_cast(std::string_view name) noexcept {
  return enum_values<E>::values.cast(name);
}

/**
 * @brief Check if an enum value is valid (exists in registered values).
 * @tparam E The enum type (must have enum_values<E> specialization).
 * @param value The enum value to check.
 * @return True if the value exists in the registered values, false otherwise.
 */
template <registered_enum E>
[[nodiscard]] constexpr bool enum_contains(E value) noexcept {
  return enum_values<E>::values.contains(value);
}

/**
 * @brief Get the index of an enum value in the registered values list.
 * @tparam E The enum type (must have enum_values<E> specialization).
 * @param value The enum value.
 * @return Optional containing the index if found, nullopt otherwise.
 */
template <registered_enum E>
[[nodiscard]] constexpr std::optional<std::size_t> enum_index(E value) noexcept {
  return enum_values<E>::values.index_of(value);
}

/**
 * @brief Get the number of registered enum values.
 * @tparam E The enum type (must have enum_values<E> specialization).
 * @return The number of registered values.
 */
template <registered_enum E>
[[nodiscard]] constexpr std::size_t enum_count() noexcept {
  return enum_values<E>::values.count;
}

/**
 * @brief Get all registered enum values as an array.
 * @tparam E The enum type (must have enum_values<E> specialization).
 * @return Const reference to the array of enum values.
 */
template <registered_enum E>
[[nodiscard]] constexpr const auto& enum_entries() noexcept {
  return enum_values<E>::values.values();
}

/**
 * @brief Get all registered enum names as an array.
 * @tparam E The enum type (must have enum_values<E> specialization).
 * @return Const reference to the array of enum names.
 */
template <registered_enum E>
[[nodiscard]] constexpr const auto& enum_names() noexcept {
  return enum_values<E>::values.names();
}

/**
 * @brief Convert an underlying integer value to enum using registered values.
 * @tparam E The enum type (must have enum_values<E> specialization).
 * @param value The underlying integer value.
 * @return Optional containing the enum if value is valid, nullopt otherwise.
 */
template <registered_enum E>
[[nodiscard]] constexpr std::optional<E> enum_from_underlying(std::underlying_type_t<E> value) noexcept {
  return enum_values<E>::values.from_underlying(value);
}

/**
 * @brief Apply a function to each registered enum value.
 * @tparam E The enum type (must have enum_values<E> specialization).
 * @tparam F The function type.
 * @param func The function to apply (receives index wrapper and enum value).
 */
template <registered_enum E, typename F>
constexpr void enum_for_each(const F& func) noexcept(noexcept(enum_values<E>::values.for_each(func))) {
  enum_values<E>::values.for_each(func);
}

}  // namespace ctti
