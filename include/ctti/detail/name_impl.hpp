#ifndef CTTI_DETAIL_NAME_IMPL_HPP
#define CTTI_DETAIL_NAME_IMPL_HPP

#include <ctti/detail/concepts_impl.hpp>
#include <ctti/detail/language_features.hpp>
#include <ctti/detail/meta.hpp>
#include <ctti/detail/name_parser.hpp>
#include <ctti/detail/pretty_function.hpp>
#include <ctti/detail/static_value_impl.hpp>
#include <ctti/type_tag.hpp>

#include <concepts>
#include <cstdint>
#include <string_view>
#include <type_traits>

namespace ctti::detail {

template <typename T>
constexpr std::string_view DefaultNameOf(type_tag<T>) noexcept {
  const auto full_name = pretty_function::Type<T>();
  auto result = full_name.substr(CTTI_TYPE_PRETTY_FUNCTION_LEFT,
                                 full_name.size() - CTTI_TYPE_PRETTY_FUNCTION_LEFT - CTTI_TYPE_PRETTY_FUNCTION_RIGHT);
  result = NameParser::FilterTypenamePrefix(result);
  const auto pos = result.find(';');
  return pos != std::string_view::npos ? result.substr(0, pos) : result;
}

template <typename T, T Value>
constexpr std::string_view DefaultNameOf(StaticValue<T, Value>) noexcept {
  const auto full_name = pretty_function::Value<T, Value>();
  const auto separator = std::string_view{CTTI_VALUE_PRETTY_FUNCTION_SEPARATOR};
  const auto sep_pos = full_name.find(separator);

  if (sep_pos == std::string_view::npos) return full_name;

  auto value_part = full_name.substr(sep_pos + separator.length());
  constexpr auto suffix_len = CTTI_VALUE_PRETTY_FUNCTION_RIGHT;

  if (value_part.length() > suffix_len) {
    value_part = value_part.substr(0, value_part.length() - suffix_len);
  }

  const auto semicolon_pos = value_part.find(';');
  if (semicolon_pos != std::string_view::npos) {
    value_part = value_part.substr(0, semicolon_pos);
  }

  if constexpr (std::is_enum_v<T>) {
    const auto last_colon = value_part.rfind("::");
    if (last_colon != std::string_view::npos) {
      value_part = value_part.substr(last_colon + 2);
    }
  }

  return value_part;
}

template <typename T>
constexpr auto TryNameOfImpl(type_tag<T> tag, int) noexcept -> decltype(name_of_impl(tag)) {
  return name_of_impl(tag);
}

template <typename T>
constexpr std::string_view TryNameOfImpl(type_tag<T> tag, ...) noexcept {
  return DefaultNameOf(tag);
}

template <typename T, T Value>
constexpr auto TryNameOfImpl(StaticValue<T, Value> val, int) noexcept -> decltype(name_of_impl(val)) {
  return name_of_impl(val);
}

template <typename T, T Value>
constexpr std::string_view TryNameOfImpl(StaticValue<T, Value> val, ...) noexcept {
  return DefaultNameOf(val);
}

template <typename T>
constexpr std::string_view AdlNameOfImpl(type_tag<T> tag) noexcept {
  return TryNameOfImpl(tag, 0);
}

template <typename T, T Value>
constexpr std::string_view AdlNameOfImpl(StaticValue<T, Value> val) noexcept {
  return TryNameOfImpl(val, 0);
}
template <typename T, typename = void>
struct NameOfImpl {
  static constexpr std::string_view Apply() noexcept { return AdlNameOfImpl(type_tag<T>{}); }
};

template <HasCustomNameOf T>
struct NameOfImpl<T> {
  static constexpr std::string_view Apply() noexcept { return T::ctti_name_of(); }
};

template <typename T, T Value>
struct ValueNameOfImpl {
  static constexpr std::string_view Apply() noexcept { return AdlNameOfImpl(StaticValue<T, Value>{}); }
};

class QualifiedName {
public:
  constexpr QualifiedName(std::string_view full_name) noexcept {
    const auto pos = full_name.find(';');
    full_name_ = pos != std::string_view::npos ? full_name.substr(0, pos) : full_name;
  }

  constexpr std::string_view GetName() const noexcept {
    const auto last_colon_pos = full_name_.rfind("::");
    return last_colon_pos == std::string_view::npos ? full_name_ : full_name_.substr(last_colon_pos + 2);
  }

  constexpr std::string_view GetFullName() const noexcept { return full_name_; }

  constexpr std::string_view GetQualifier(std::size_t index) const noexcept {
    if (index == 0) {
      const auto first_separator = full_name_.find("::");
      return first_separator != std::string_view::npos ? full_name_.substr(0, first_separator) : std::string_view{};
    }

    const auto pos_prev = NameParser::FindIth(full_name_, "::", index - 1);
    if (pos_prev == std::string_view::npos) return {};

    const auto pos_curr = full_name_.find("::", pos_prev + 2);
    if (pos_curr == std::string_view::npos) return {};

    return full_name_.substr(pos_prev + 2, pos_curr - pos_prev - 2);
  }

  constexpr bool operator==(const QualifiedName& other) const noexcept { return full_name_ == other.full_name_; }

private:
  std::string_view full_name_;
};

}  // namespace ctti::detail

// Provide customizations for std types
namespace std {

constexpr std::string_view name_of_impl(ctti::type_tag<std::string>) noexcept {
  return "std::string";
}

constexpr std::string_view name_of_impl(ctti::type_tag<std::string_view>) noexcept {
  return "std::string_view";
}

}  // namespace std

#endif  // CTTI_DETAIL_NAME_IMPL_HPP
