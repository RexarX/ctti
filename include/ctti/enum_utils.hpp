#pragma once

#include <ctti/detail/enum_utils_impl.hpp>

#include <concepts>
#include <cstddef>
#include <string_view>
#include <type_traits>

namespace ctti {

template <typename E>
concept scoped_enum = detail::ScopedEnum<E>;

template <typename E>
concept unscoped_enum = detail::UnscopedEnum<E>;

template <typename E>
  requires std::is_enum_v<E>
class enum_info {
private:
  using internal_info = detail::EnumInfo<E>;

public:
  using enum_type = E;
  using underlying_type = std::underlying_type_t<E>;

  template <E Value>
  static constexpr std::string_view name_of_value() noexcept {
    return internal_info::template NameOfValue<Value>();
  }

  template <E Value>
  static constexpr underlying_type underlying_value() noexcept {
    return internal_info::template UnderlyingValue<Value>();
  }

  template <std::convertible_to<underlying_type> T>
  static constexpr std::optional<E> from_underlying(T value) noexcept {
    return internal_info::FromUnderlying(value);
  }

  static constexpr std::string_view name() noexcept { return internal_info::Name(); }

  static constexpr bool is_scoped() noexcept { return internal_info::IsScoped(); }
};

template <typename E, E... Values>
  requires std::is_enum_v<E>
class enum_value_list {
private:
  using internal_list = detail::EnumValueList<E, Values...>;

public:
  using enum_type = E;
  using values_type = typename internal_list::values;
  static constexpr std::size_t count = internal_list::kCount;

  static constexpr std::array<E, count> value_array() noexcept { return internal_list::ValueArray(); }

  template <std::size_t I>
    requires(I < count)
  static constexpr E at() noexcept {
    return internal_list::template At<I>();
  }

  template <E Value>
  static constexpr bool contains() noexcept {
    return internal_list::template Contains<Value>();
  }

  template <typename F>
    requires(std::invocable<const F&, detail::SizeType<static_cast<std::size_t>(Values)>, E> && ...)
  static constexpr void for_each(const F& func) noexcept(noexcept(internal_list::ForEach(std::declval<const F&>()))) {
    internal_list::ForEach(func);
  }

  static constexpr std::array<std::string_view, count> names() noexcept { return internal_list::Names(); }
};

template <auto... Values>
  requires detail::SameEnumValues<Values...> && (sizeof...(Values) > 0)
constexpr auto make_enum_list() noexcept {
  using E = decltype((Values, ...));
  return enum_value_list<E, Values...>{};
}

template <typename E>
  requires std::is_enum_v<E>
constexpr enum_info<E> get_enum_info() noexcept {
  return {};
}

template <typename E, E Value>
  requires std::is_enum_v<E>
constexpr std::string_view enum_name() noexcept {
  return enum_info<E>::template name_of_value<Value>();
}

template <typename E>
  requires std::is_enum_v<E>
constexpr std::string_view enum_type_name() noexcept {
  return enum_info<E>::name();
}

template <typename E>
  requires std::is_enum_v<std::remove_cvref_t<E>>
constexpr auto get_enum_info([[maybe_unused]] const E& obj) noexcept {
  return enum_info<std::remove_cvref_t<E>>{};
}

template <typename E>
  requires std::is_enum_v<std::remove_cvref_t<E>>
constexpr std::string_view enum_type_name([[maybe_unused]] const E& obj) noexcept {
  return enum_info<std::remove_cvref_t<E>>::name();
}

template <typename E, E Value>
  requires std::is_enum_v<E>
constexpr auto enum_underlying_value() noexcept {
  return enum_info<E>::template underlying_value<Value>();
}

template <typename E, E Lhs, E Rhs>
  requires std::is_enum_v<E>
constexpr bool enum_equal() noexcept {
  return detail::EnumEqual<E, Lhs, Rhs>();
}

template <typename E, E Lhs, E Rhs>
  requires std::is_enum_v<E>
constexpr bool enum_less() noexcept {
  return detail::EnumLess<E, Lhs, Rhs>();
}

}  // namespace ctti
