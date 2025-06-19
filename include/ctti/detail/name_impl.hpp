#pragma once

#include <ctti/detail/compile_time_string.hpp>
#include <ctti/detail/meta.hpp>
#include <ctti/detail/pretty_function.hpp>

#include <array>
#include <concepts>
#include <cstddef>
#include <string>
#include <string_view>
#include <type_traits>

namespace ctti::detail {

constexpr bool IsWhitespace(char ch) noexcept {
  return ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r';
}

constexpr std::string_view TrimWhitespace(std::string_view str) noexcept {
  while (!str.empty() && IsWhitespace(str.front())) {
    str = str.substr(1);
  }
  while (!str.empty() && IsWhitespace(str.back())) {
    str = str.substr(0, str.length() - 1);
  }
  return str;
}

constexpr std::string_view FilterPrefix(std::string_view str, std::string_view prefix) noexcept {
  str = TrimWhitespace(str);
  if (str.size() >= prefix.size() && str.starts_with(prefix)) {
    return TrimWhitespace(str.substr(prefix.size()));
  }
  return str;
}

constexpr std::string_view FilterTypenamePrefix(std::string_view type_name) noexcept {
  type_name = FilterPrefix(type_name, "class ");
  type_name = FilterPrefix(type_name, "struct ");
  type_name = FilterPrefix(type_name, "enum ");
  type_name = FilterPrefix(type_name, "union ");
  type_name = FilterPrefix(type_name, "const ");
  return type_name;
}

constexpr std::string_view CleanupTypeName(std::string_view type_name) noexcept {
  type_name = TrimWhitespace(type_name);

  // Only remove class/struct prefixes
  type_name = FilterPrefix(type_name, "class ");
  type_name = FilterPrefix(type_name, "struct ");
  type_name = FilterPrefix(type_name, "enum ");
  type_name = FilterPrefix(type_name, "union ");
  type_name = TrimWhitespace(type_name);

  // Remove any trailing artifacts like ';' or ')'
  while (!type_name.empty() && (type_name.back() == ';' || type_name.back() == ')')) {
    type_name = type_name.substr(0, type_name.size() - 1);
  }

  return TrimWhitespace(type_name);
}

template <std::int64_t N>
class IntToStringHelper {
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
      return N < 0 ? digits + 1 : digits;
    }
  }

public:
  static constexpr std::size_t kDigits = CalculateDigits();

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

  static constexpr auto kValue = Generate();
};

template <std::int64_t N>
constexpr std::string_view GetIntegerString() noexcept {
  constexpr auto& str_array = IntToStringHelper<N>::kValue;
  return std::string_view{str_array.data()};
}

constexpr std::string_view ExtractEnumValueName(std::string_view full_name) noexcept {
#if defined(__clang__)
  // Clang format: "...[T = EnumType, ValueParam = Value]"
  const auto value_param_pos = full_name.find("ValueParam = ");
  if (value_param_pos != std::string_view::npos) {
    const auto value_start = value_param_pos + 13;  // length of "ValueParam = "
    const auto value_end = full_name.find(']', value_start);
    if (value_end != std::string_view::npos) {
      const auto result = TrimWhitespace(full_name.substr(value_start, value_end - value_start));

      // For scoped enums: "EnumType::Value" -> extract "Value"
      const auto last_colon = result.rfind("::");
      if (last_colon != std::string_view::npos) {
        return result.substr(last_colon + 2);
      }

      // For unscoped enums: just "Value"
      return result;
    }
  }

  // Fallback: try the old " = " pattern
  const auto equal_pos = full_name.find(" = ");
  if (equal_pos != std::string_view::npos) {
    const auto value_start = equal_pos + 3;
    const auto value_end = full_name.find(']', value_start);
    if (value_end != std::string_view::npos) {
      const auto result = TrimWhitespace(full_name.substr(value_start, value_end - value_start));

      const auto last_colon = result.rfind("::");
      if (last_colon != std::string_view::npos) {
        return result.substr(last_colon + 2);
      }
      return result;
    }
  }

#elif defined(__GNUC__) && !defined(__clang__)
  // GCC format: "...[with T = EnumType, ValueParam = Value]" or similar

  // First try to find ValueParam pattern
  const auto param_start = full_name.find("ValueParam = ");
  if (param_start != std::string_view::npos) {
    const auto value_start = param_start + 13;  // length of "ValueParam = "
    const auto value_end = full_name.find_first_of("];", value_start);
    if (value_end != std::string_view::npos) {
      const auto result = TrimWhitespace(full_name.substr(value_start, value_end - value_start));

      // For unscoped enums, GCC might format as "EnumName::Value" or just "Value"
      const auto last_colon = result.rfind("::");
      if (last_colon != std::string_view::npos) {
        return result.substr(last_colon + 2);
      }

      // For unscoped enums that show as "(EnumType)Value", extract just the value
      const auto paren_pos = result.find(')');
      if (paren_pos != std::string_view::npos && paren_pos + 1 < result.size()) {
        return result.substr(paren_pos + 1);
      }

      return result;
    }
  }

  // Fallback: try general " = " pattern
  const auto equal_pos = full_name.find(" = ");
  if (equal_pos != std::string_view::npos) {
    const auto value_start = equal_pos + 3;
    const auto value_end = full_name.find_first_of("];", value_start);
    if (value_end != std::string_view::npos) {
      const auto result = TrimWhitespace(full_name.substr(value_start, value_end - value_start));

      const auto last_colon = result.rfind("::");
      if (last_colon != std::string_view::npos) {
        return result.substr(last_colon + 2);
      }
      return result;
    }
  }

#elif defined(_MSC_VER) && !defined(__clang__)
  // MSVC format: typically "function_name<enum_type, value>" or similar
  const auto last_comma = full_name.rfind(',');
  if (last_comma != std::string_view::npos) {
    const auto value_start = last_comma + 1;
    const auto angle_end = full_name.find('>', value_start);
    if (angle_end != std::string_view::npos) {
      const auto result = TrimWhitespace(full_name.substr(value_start, angle_end - value_start));

      // For scoped enums: "EnumType::Value" -> extract "Value"
      const auto last_colon = result.rfind("::");
      if (last_colon != std::string_view::npos) {
        return result.substr(last_colon + 2);
      }

      // For unscoped enums or simple values
      return result;
    }
  }

  // Fallback: try to find the enum value in angle brackets
  const auto last_angle_start = full_name.rfind('<');
  if (last_angle_start != std::string_view::npos) {
    const auto angle_end = full_name.find('>', last_angle_start);
    if (angle_end != std::string_view::npos) {
      const auto content = full_name.substr(last_angle_start + 1, angle_end - last_angle_start - 1);
      const auto comma_pos = content.rfind(',');
      if (comma_pos != std::string_view::npos) {
        const auto result = TrimWhitespace(content.substr(comma_pos + 1));

        // Handle scoped enum format
        const auto last_colon = result.rfind("::");
        if (last_colon != std::string_view::npos) {
          return result.substr(last_colon + 2);
        }
        return result;
      }
    }
  }
#endif

  return "unknown_value";
}

template <typename T>
constexpr std::string_view GetBuiltinTypeName() noexcept {
  if constexpr (std::same_as<T, void>)
    return "void";
  else if constexpr (std::same_as<T, bool>)
    return "bool";
  else if constexpr (std::same_as<T, char>)
    return "char";
  else if constexpr (std::same_as<T, signed char>)
    return "signed char";
  else if constexpr (std::same_as<T, unsigned char>)
    return "unsigned char";
  else if constexpr (std::same_as<T, short>)
    return "short";
  else if constexpr (std::same_as<T, unsigned short>)
    return "unsigned short";
  else if constexpr (std::same_as<T, int>)
    return "int";
  else if constexpr (std::same_as<T, unsigned int>)
    return "unsigned int";
  else if constexpr (std::same_as<T, long>)
    return "long";
  else if constexpr (std::same_as<T, unsigned long>)
    return "unsigned long";
  else if constexpr (std::same_as<T, long long>)
    return "long long";
  else if constexpr (std::same_as<T, unsigned long long>)
    return "unsigned long long";
  else if constexpr (std::same_as<T, float>)
    return "float";
  else if constexpr (std::same_as<T, double>)
    return "double";
  else if constexpr (std::same_as<T, long double>)
    return "long double";
  else if constexpr (std::same_as<T, char8_t>)
    return "char8_t";
  else if constexpr (std::same_as<T, char16_t>)
    return "char16_t";
  else if constexpr (std::same_as<T, char32_t>)
    return "char32_t";
  else if constexpr (std::same_as<T, wchar_t>)
    return "wchar_t";
  else if constexpr (std::same_as<T, std::nullptr_t>)
    return "std::nullptr_t";
  else if constexpr (std::same_as<T, std::string>)
    return "std::string";
  else if constexpr (std::same_as<T, std::string_view>)
    return "std::string_view";
  else
    return "";
}

template <typename T, T ValueParam>
constexpr std::string_view DefaultNameOfValue() noexcept {
  if constexpr (std::integral<T> && !std::same_as<T, char> && !std::same_as<T, bool>) {
    return GetIntegerString<static_cast<std::int64_t>(ValueParam)>();
  }

  if constexpr (std::same_as<T, bool>) {
    if constexpr (ValueParam) {
      return "true";
    } else {
      return "false";
    }
  }

  if constexpr (std::same_as<T, char>) {
    return GetIntegerString<static_cast<std::int64_t>(ValueParam)>();
  }

  if constexpr (std::is_enum_v<T>) {
    constexpr auto full_name = PrettyFunction::Value<T, ValueParam>();
    return ExtractEnumValueName(full_name);
  }

  return "unknown_value";
}

template <typename T, T ValueParam>
constexpr auto TryNameOfValueImpl(int) noexcept
    -> decltype(name_of_impl(Identity<T>{}, std::integral_constant<T, ValueParam>{})) {
  return name_of_impl(Identity<T>{}, std::integral_constant<T, ValueParam>{});
}

template <typename T, T ValueParam>
constexpr std::string_view TryNameOfValueImpl(...) noexcept {
  return DefaultNameOfValue<T, ValueParam>();
}

template <typename T, T ValueParam>
struct ValueNameOfImpl {
  static constexpr std::string_view Apply() noexcept { return TryNameOfValueImpl<T, ValueParam>(0); }
};

// Extract type name from pretty function signature
template <typename T>
constexpr std::string_view ExtractTypeNameFromSignature() noexcept {
  constexpr auto signature = PrettyFunction::Type<T>();

  // Check for builtin types first (includes std::string and std::string_view)
  constexpr auto builtin_name = GetBuiltinTypeName<T>();
  if constexpr (!builtin_name.empty()) {
    return builtin_name;
  }

  // Extract from compiler-specific signature
  if constexpr (kTypePrettyFunctionLeft < signature.size() && kTypePrettyFunctionRight < signature.size()) {
    constexpr auto start = kTypePrettyFunctionLeft;
    constexpr auto end = signature.size() - kTypePrettyFunctionRight;

    if constexpr (start < end) {
      constexpr auto extracted = signature.substr(start, end - start);
      return CleanupTypeName(extracted);
    }
  }

  // Fallback: return the whole signature for debugging
  return signature;
}

// Forward declaration for ADL lookup
template <typename T>
constexpr auto TryNameOfImpl(int) noexcept -> decltype(name_of_impl(Identity<T>{})) {
  return name_of_impl(Identity<T>{});
}

template <typename T>
constexpr std::string_view TryNameOfImpl(...) noexcept {
  return ExtractTypeNameFromSignature<T>();
}

// Main name implementation
template <typename T>
struct NameOfImpl {
  static constexpr std::string_view Apply() noexcept {
    if constexpr (requires { T::ctti_name_of(); }) {
      return T::ctti_name_of();
    } else {
      return TryNameOfImpl<T>(0);
    }
  }
};

class QualifiedName {
public:
  explicit constexpr QualifiedName(std::string_view full_name) noexcept {
    const auto pos = full_name.find(';');
    full_name_ = pos != std::string_view::npos ? full_name.substr(0, pos) : full_name;
  }

  constexpr QualifiedName(const QualifiedName&) noexcept = default;
  constexpr QualifiedName(QualifiedName&&) noexcept = default;
  constexpr ~QualifiedName() noexcept = default;

  constexpr QualifiedName& operator=(const QualifiedName&) noexcept = default;
  constexpr QualifiedName& operator=(QualifiedName&&) noexcept = default;

  constexpr std::string_view GetName() const noexcept {
    const auto last_colon_pos = full_name_.rfind("::");
    return last_colon_pos == std::string_view::npos ? full_name_ : full_name_.substr(last_colon_pos + 2);
  }

  constexpr std::string_view GetQualifier(std::size_t index) const noexcept {
    if (index == 0) {
      const auto first_separator = full_name_.find("::");
      return first_separator != std::string_view::npos ? full_name_.substr(0, first_separator) : std::string_view{};
    }

    const auto pos_prev = FindIth(full_name_, "::", index - 1);
    if (pos_prev == std::string_view::npos) return {};

    const auto pos_curr = full_name_.find("::", pos_prev + 2);
    if (pos_curr == std::string_view::npos) return {};

    return full_name_.substr(pos_prev + 2, pos_curr - pos_prev - 2);
  }

  constexpr std::string_view GetFullName() const noexcept { return full_name_; }

  constexpr bool operator==(const QualifiedName& other) const noexcept { return full_name_ == other.full_name_; }

private:
  static constexpr std::size_t FindIth(std::string_view name, std::string_view substring, std::size_t index) noexcept {
    if (substring.empty()) {
      return index <= name.size() ? index : std::string_view::npos;
    }

    std::size_t pos = 0;
    std::size_t found = 0;

    while (pos < name.size()) {
      const auto next_pos = name.find(substring, pos);
      if (next_pos == std::string_view::npos) {
        return std::string_view::npos;
      }

      if (found == index) return next_pos;

      ++found;
      pos = next_pos + substring.size();
    }

    return std::string_view::npos;
  }

  std::string_view full_name_;
};

}  // namespace ctti::detail
