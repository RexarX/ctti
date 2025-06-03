#ifndef CTTI_ENUM_UTILS_HPP
#define CTTI_ENUM_UTILS_HPP

#include <ctti/concepts.hpp>
#include <ctti/detail/enum_utils_impl.hpp>

namespace ctti {

template <typename E>
concept scoped_enum = detail::ScopedEnum<E>;

template <typename E>
concept unscoped_enum = detail::UnscopedEnum<E>;

template <enum_type E>
struct enum_info {
  using enum_type = E;
  using underlying_type = std::underlying_type_t<E>;

  template <E Value>
  static constexpr std::string_view name_of_value() noexcept {
    return detail::EnumInfo<E>::template NameOfValue<Value>();
  }

  template <E Value>
  static constexpr underlying_type underlying_value() noexcept {
    return detail::EnumInfo<E>::template UnderlyingValue<Value>();
  }

  static constexpr std::string_view name() noexcept { return detail::EnumInfo<E>::Name(); }

  static constexpr bool is_scoped() noexcept { return detail::EnumInfo<E>::IsScoped(); }

  template <std::convertible_to<underlying_type> T>
  static constexpr std::optional<E> from_underlying(T value) noexcept {
    return detail::EnumInfo<E>::FromUnderlying(value);
  }

  template <E Value>
  static constexpr auto detailed_name() noexcept {
    return detail::EnumInfo<E>::template DetailedName<Value>();
  }
};

template <enum_type E, E... Values>
struct enum_value_list {
  using enum_type = E;
  using values = typename detail::EnumValueList<E, Values...>::values;
  static constexpr std::size_t count = detail::EnumValueList<E, Values...>::kCount;

  static constexpr std::array<E, count> value_array() noexcept {
    return detail::EnumValueList<E, Values...>::ValueArray();
  }

  template <std::size_t I>
    requires(I < count)
  static constexpr E at() noexcept {
    return detail::EnumValueList<E, Values...>::template At<I>();
  }

  template <E Value>
  static constexpr bool contains() noexcept {
    return detail::EnumValueList<E, Values...>::template Contains<Value>();
  }

  template <typename F>
  static constexpr void for_each(F&& f) {
    detail::EnumValueList<E, Values...>::ForEach(std::forward<F>(f));
  }

  static constexpr std::array<std::string_view, count> names() noexcept {
    return detail::EnumValueList<E, Values...>::Names();
  }
};

template <enum_type E, E... Values>
constexpr enum_value_list<E, Values...> make_enum_list() noexcept {
  return {};
}

template <enum_type E>
constexpr enum_info<E> get_enum_info() noexcept {
  return {};
}

template <enum_type E, E Value>
constexpr std::string_view enum_name() noexcept {
  return enum_info<E>::template name_of_value<Value>();
}

template <enum_type E>
constexpr std::string_view enum_type_name() noexcept {
  return enum_info<E>::name();
}

template <enum_type E, E Value>
constexpr auto enum_underlying_value() noexcept {
  return enum_info<E>::template underlying_value<Value>();
}

template <enum_type E, E Lhs, E Rhs>
constexpr bool enum_equal() noexcept {
  return detail::EnumEqual<E, Lhs, Rhs>();
}

template <enum_type E, E Lhs, E Rhs>
constexpr bool enum_less() noexcept {
  return detail::EnumLess<E, Lhs, Rhs>();
}

}  // namespace ctti

#endif  // CTTI_ENUM_UTILS_HPP
