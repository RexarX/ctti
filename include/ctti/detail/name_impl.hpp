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
#include <array>

namespace ctti::detail {

constexpr std::string_view CleanupMsvcTypeName(std::string_view name) noexcept {
#if defined(_MSC_VER)
  // Remove trailing artifacts like "(void)", "n", ">"
  while (!name.empty()) {
    char last = name.back();
    if (last == '>' || last == ')' || last == ' ' || last == 'n') {
      name = name.substr(0, name.length() - 1);
    } else {
      break;
    }
  }

  // Remove trailing "(void" pattern specifically
  constexpr std::string_view void_suffix = "(void";
  if (name.length() >= void_suffix.length() &&
      name.substr(name.length() - void_suffix.length()) == void_suffix) {
    name = name.substr(0, name.length() - void_suffix.length());
  }

  // Final cleanup of trailing spaces and angle brackets
  while (!name.empty() && (name.back() == '>' || name.back() == ' ')) {
    name = name.substr(0, name.length() - 1);
  }
#endif
  return name;
}

template<std::int64_t N>
struct IntToStringHelper {
private:
  static constexpr std::size_t CalculateDigits() noexcept {
    if constexpr (N == 0) {
      return 1;
    } else {
      std::int64_t temp = N < 0 ? -N : N;
      std::size_t digits = 0;
      while (temp > 0) {
        temp /= 10;
        ++digits;
      }
      return N < 0 ? digits + 1 : digits; // +1 for minus sign
    }
  }

  static constexpr std::size_t kDigits = CalculateDigits();

public:
  static constexpr std::array<char, kDigits + 1> Generate() noexcept {
    std::array<char, kDigits + 1> result{};

    if constexpr (N == 0) {
      result[0] = '0';
      result[1] = '\0';
    } else {
      std::int64_t temp = N < 0 ? -N : N;
      std::size_t pos = kDigits;
      result[pos] = '\0';

      while (temp > 0) {
        result[--pos] = '0' + (temp % 10);
        temp /= 10;
      }

      if constexpr (N < 0) {
        result[0] = '-';
      }
    }

    return result;
  }

  static constexpr auto value = Generate();
};

template<std::int64_t N>
constexpr std::string_view GetIntegerString() noexcept {
  constexpr auto& str_array = IntToStringHelper<N>::value;
  return std::string_view{str_array.data()};
}

constexpr int HexCharToInt(char c) noexcept {
  if (c >= '0' && c <= '9') return c - '0';
  if (c >= 'a' && c <= 'f') return c - 'a' + 10;
  if (c >= 'A' && c <= 'F') return c - 'A' + 10;
  return 0;
}

constexpr std::int64_t ParseHexString(std::string_view hex_str) noexcept {
  if (hex_str.length() < 3 || !hex_str.starts_with("0x")) {
    return 0;
  }

  std::int64_t result = 0;
  for (std::size_t i = 2; i < hex_str.length(); ++i) {
    char c = hex_str[i];
    if ((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F')) {
      result = result * 16 + HexCharToInt(c);
    } else {
      break; // Stop at first non-hex character
    }
  }

  return result;
}

constexpr std::string_view ExtractMsvcValue(std::string_view full_sig) noexcept {
  // MSVC signature looks like: "...Value<int,0x2a>(void)..."
  // We need to find the comma after the type, then extract until the closing >

  const auto value_pos = full_sig.find("Value<");
  if (value_pos == std::string_view::npos) {
    return full_sig;
  }

  const auto type_start = value_pos + 6; // After "Value<"
  const auto comma_pos = full_sig.find(',', type_start);
  if (comma_pos == std::string_view::npos) {
    return full_sig;
  }

  const auto value_start = comma_pos + 1;
  const auto close_pos = full_sig.find('>', value_start);
  if (close_pos == std::string_view::npos) {
    return full_sig;
  }

  auto result = full_sig.substr(value_start, close_pos - value_start);

  // Trim whitespace
  while (!result.empty() && result.front() == ' ') {
    result = result.substr(1);
  }
  while (!result.empty() && result.back() == ' ') {
    result = result.substr(0, result.length() - 1);
  }

  return result;
}

template <typename T>
constexpr std::string_view DefaultNameOf(type_tag<T>) noexcept {
  const auto full_name = pretty_function::Type<T>();
  auto result = full_name.substr(CTTI_TYPE_PRETTY_FUNCTION_LEFT,
                                 full_name.size() - CTTI_TYPE_PRETTY_FUNCTION_LEFT - CTTI_TYPE_PRETTY_FUNCTION_RIGHT);

  result = NameParser::FilterTypenamePrefix(result);

  // Handle semicolon separators (common in some compiler outputs)
  const auto pos = result.find(';');
  if (pos != std::string_view::npos) {
    result = result.substr(0, pos);
  }

  // Clean up compiler-specific artifacts
  result = CleanupMsvcTypeName(result);

  return result;
}

template <typename T, T Value>
constexpr std::string_view DefaultNameOf(StaticValue<T, Value>) noexcept {
  // For integral types (except char and bool), directly convert to string
  if constexpr (std::integral<T> && !std::same_as<T, char> && !std::same_as<T, bool>) {
    return GetIntegerString<static_cast<std::int64_t>(Value)>();
  }

  // For bool values
  if constexpr (std::same_as<T, bool>) {
    if constexpr (Value) {
      static constexpr char true_str[] = "true";
      return std::string_view{true_str};
    } else {
      static constexpr char false_str[] = "false";
      return std::string_view{false_str};
    }
  }

  // For char values - convert to numeric representation
  if constexpr (std::same_as<T, char>) {
    return GetIntegerString<static_cast<std::int64_t>(Value)>();
  }

  // For other types (including enums), fall back to pretty function parsing
  const auto full_name = pretty_function::Value<T, Value>();

#if defined(_MSC_VER)
  // Use custom MSVC extraction
  auto value_part = ExtractMsvcValue(full_name);

  // Handle enum values
  if constexpr (std::is_enum_v<T>) {
    if (value_part.starts_with("0x") || value_part.starts_with("0X")) {
      const auto parsed_value = ParseHexString(value_part);
      if (static_cast<std::underlying_type_t<T>>(Value) == parsed_value) {
        return GetIntegerString<static_cast<std::int64_t>(static_cast<std::underlying_type_t<T>>(Value))>();
      }
    }
  }

  return CleanupMsvcTypeName(value_part);

#else
  // GCC/Clang path - extract from pretty function

#if defined(__clang__)
  // Clang format: "... [T = type, ValueParam = value]"
  const auto param_start = full_name.find("ValueParam = ");
  if (param_start != std::string_view::npos) {
    auto value_start = param_start + 13; // After "ValueParam = "
    auto value_end = full_name.find(']', value_start);
    if (value_end == std::string_view::npos) {
      value_end = full_name.size();
    }

    auto result = full_name.substr(value_start, value_end - value_start);

    // Handle enum values - extract the simple name
    if constexpr (std::is_enum_v<T>) {
      const auto last_colon = result.rfind("::");
      if (last_colon != std::string_view::npos) {
        result = result.substr(last_colon + 2);
      }
    }

    return result;
  }
#elif defined(__GNUC__) && !defined(__clang__)
  // GCC format: "... [with T = type; T ValueParam = value]"
  const auto param_start = full_name.find("; T ValueParam = ");
  if (param_start != std::string_view::npos) {
    auto value_start = param_start + 17; // After "; T ValueParam = "
    auto value_end = full_name.find(']', value_start);
    if (value_end == std::string_view::npos) {
      value_end = full_name.size();
    }

    auto result = full_name.substr(value_start, value_end - value_start);

    // Handle enum values - extract the simple name
    if constexpr (std::is_enum_v<T>) {
      const auto last_colon = result.rfind("::");
      if (last_colon != std::string_view::npos) {
        result = result.substr(last_colon + 2);
      }
    }

    return result;
  }
#endif

  // Fallback - return the original if we can't parse it
  return full_name;
#endif
}

// ADL-based name resolution with fallback
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

// Main name resolution implementation
template <typename T, typename = void>
struct NameOfImpl {
  static constexpr std::string_view Apply() noexcept {
    return AdlNameOfImpl(type_tag<T>{});
  }
};

template <HasCustomNameOf T>
struct NameOfImpl<T> {
  static constexpr std::string_view Apply() noexcept {
    return T::ctti_name_of();
  }
};

template <typename T, T Value>
struct ValueNameOfImpl {
  static constexpr std::string_view Apply() noexcept {
    return AdlNameOfImpl(StaticValue<T, Value>{});
  }
};

// Qualified name handling
class QualifiedName {
public:
  constexpr QualifiedName(std::string_view full_name) noexcept {
    // Remove semicolon-separated parts (some compilers add extra info)
    const auto pos = full_name.find(';');
    full_name_ = pos != std::string_view::npos ? full_name.substr(0, pos) : full_name;
  }

  constexpr std::string_view GetName() const noexcept {
    const auto last_colon_pos = full_name_.rfind("::");
    return last_colon_pos == std::string_view::npos ? full_name_ : full_name_.substr(last_colon_pos + 2);
  }

  constexpr std::string_view GetFullName() const noexcept {
    return full_name_;
  }

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

  constexpr bool operator==(const QualifiedName& other) const noexcept {
    return full_name_ == other.full_name_;
  }

private:
  std::string_view full_name_;
};

}  // namespace ctti::detail

// Standard library type customizations
namespace std {

constexpr std::string_view name_of_impl(ctti::type_tag<std::string>) noexcept {
  return "std::string";
}

constexpr std::string_view name_of_impl(ctti::type_tag<std::string_view>) noexcept {
  return "std::string_view";
}

constexpr std::string_view name_of_impl(ctti::type_tag<std::nullptr_t>) noexcept {
  return "std::nullptr_t";
}

}  // namespace std

#endif  // CTTI_DETAIL_NAME_IMPL_HPP
