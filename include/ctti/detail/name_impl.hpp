#pragma once

#include <ctti/detail/meta.hpp>

#include <array>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>

namespace ctti::detail {

// =============================================================================
// Compiler-specific pretty function macros and detection
// =============================================================================

#if defined(__clang__)
#define CTTI_PRETTY_FUNCTION __PRETTY_FUNCTION__
#elif defined(__GNUC__) && !defined(__clang__)
#define CTTI_PRETTY_FUNCTION __PRETTY_FUNCTION__
#elif defined(_MSC_VER)
#define CTTI_PRETTY_FUNCTION __FUNCSIG__
#else
#error "Unsupported compiler"
#endif

// =============================================================================
// String manipulation utilities
// =============================================================================

[[nodiscard]] constexpr bool IsWhitespace(char ch) noexcept {
  return ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r';
}

[[nodiscard]] constexpr std::string_view TrimWhitespace(std::string_view str) noexcept {
  while (!str.empty() && IsWhitespace(str.front())) {
    str = str.substr(1);
  }
  while (!str.empty() && IsWhitespace(str.back())) {
    str = str.substr(0, str.length() - 1);
  }
  return str;
}

[[nodiscard]] constexpr std::string_view StripPrefix(std::string_view str, std::string_view prefix) noexcept {
  if (str.size() >= prefix.size() && str.starts_with(prefix)) {
    return str.substr(prefix.size());
  }
  return str;
}

[[nodiscard]] constexpr std::string_view CleanupTypeName(std::string_view name) noexcept {
  name = TrimWhitespace(name);
  name = StripPrefix(name, "class ");
  name = StripPrefix(name, "struct ");
  name = StripPrefix(name, "enum ");
  name = StripPrefix(name, "union ");
  return TrimWhitespace(name);
}

// =============================================================================
// Type name extraction from pretty function
// =============================================================================

template <typename T>
[[nodiscard]] constexpr std::string_view RawTypeName() noexcept {
#if defined(__clang__)
  // Format: std::string_view ctti::detail::RawTypeName() [T = TYPE]
  constexpr std::string_view function = CTTI_PRETTY_FUNCTION;
  constexpr std::string_view prefix = "[T = ";
  constexpr std::string_view suffix = "]";
#elif defined(__GNUC__) && !defined(__clang__)
  // Format: constexpr std::string_view ctti::detail::RawTypeName() [with T = TYPE; ...]
  constexpr std::string_view function = CTTI_PRETTY_FUNCTION;
  constexpr std::string_view prefix = "[with T = ";
  constexpr std::string_view suffix = ";";  // GCC may have ; or ]
#elif defined(_MSC_VER)
  // Format: std::string_view __cdecl ctti::detail::RawTypeName<TYPE>(void)
  constexpr std::string_view function = CTTI_PRETTY_FUNCTION;
  constexpr std::string_view prefix = "RawTypeName<";
  constexpr std::string_view suffix = ">(void)";
#endif

  constexpr std::size_t start = function.find(prefix) + prefix.size();

#if defined(__GNUC__) && !defined(__clang__)
  // GCC: Find either ';' or ']', whichever comes first
  constexpr std::size_t end_semicolon = function.find(';', start);
  constexpr std::size_t end_bracket = function.find(']', start);
  constexpr std::size_t end = (end_semicolon < end_bracket) ? end_semicolon : end_bracket;
#else
  constexpr std::size_t end = function.rfind(suffix);
#endif

  static_assert(start < end, "Failed to extract type name from pretty function");

  return CleanupTypeName(function.substr(start, end - start));
}

// =============================================================================
// Array-based static storage for type names
// =============================================================================

template <std::size_t... Indices>
constexpr auto SubstringToArray(std::string_view str, std::index_sequence<Indices...>) noexcept
    -> std::array<char, sizeof...(Indices) + 1> {
  return {{str[Indices]..., '\0'}};
}

template <typename T>
constexpr auto MakeTypeNameArray() noexcept {
  constexpr std::string_view name = RawTypeName<T>();
  return SubstringToArray(name, std::make_index_sequence<name.size()>{});
}

template <typename T>
struct TypeNameStorage {
  static constexpr auto kValue = MakeTypeNameArray<T>();
};

template <typename T>
[[nodiscard]] constexpr std::string_view GetStoredTypeName() noexcept {
  constexpr auto& value = TypeNameStorage<T>::kValue;
  return {value.data(), value.size() - 1};  // Exclude null terminator
}

// =============================================================================
// Builtin type name overrides for consistent cross-platform names
// =============================================================================

template <typename T>
[[nodiscard]] constexpr std::string_view GetBuiltinTypeName() noexcept {
  if constexpr (std::same_as<T, void>) {
    return "void";
  } else if constexpr (std::same_as<T, bool>) {
    return "bool";
  } else if constexpr (std::same_as<T, char>) {
    return "char";
  } else if constexpr (std::same_as<T, signed char>) {
    return "signed char";
  } else if constexpr (std::same_as<T, unsigned char>) {
    return "unsigned char";
  } else if constexpr (std::same_as<T, short>) {
    return "short";
  } else if constexpr (std::same_as<T, unsigned short>) {
    return "unsigned short";
  } else if constexpr (std::same_as<T, int>) {
    return "int";
  } else if constexpr (std::same_as<T, unsigned int>) {
    return "unsigned int";
  } else if constexpr (std::same_as<T, long>) {
    return "long";
  } else if constexpr (std::same_as<T, unsigned long>) {
    return "unsigned long";
  } else if constexpr (std::same_as<T, long long>) {
    return "long long";
  } else if constexpr (std::same_as<T, unsigned long long>) {
    return "unsigned long long";
  } else if constexpr (std::same_as<T, float>) {
    return "float";
  } else if constexpr (std::same_as<T, double>) {
    return "double";
  } else if constexpr (std::same_as<T, long double>) {
    return "long double";
  } else if constexpr (std::same_as<T, char8_t>) {
    return "char8_t";
  } else if constexpr (std::same_as<T, char16_t>) {
    return "char16_t";
  } else if constexpr (std::same_as<T, char32_t>) {
    return "char32_t";
  } else if constexpr (std::same_as<T, wchar_t>) {
    return "wchar_t";
  } else if constexpr (std::same_as<T, std::nullptr_t>) {
    return "std::nullptr_t";
  } else if constexpr (std::same_as<T, std::string>) {
    return "std::string";
  } else if constexpr (std::same_as<T, std::string_view>) {
    return "std::string_view";
  } else {
    return "";
  }
}

template <typename T>
concept HasBuiltinName = !GetBuiltinTypeName<T>().empty();

// =============================================================================
// Default type name extraction
// =============================================================================

template <typename T>
[[nodiscard]] constexpr std::string_view DefaultNameOfType() noexcept {
  if constexpr (HasBuiltinName<T>) {
    return GetBuiltinTypeName<T>();
  } else {
    return GetStoredTypeName<T>();
  }
}

// =============================================================================
// Integer to string conversion for value names
// =============================================================================

template <std::int64_t N>
class IntToStringHelper final {
private:
  [[nodiscard]] static constexpr std::size_t CalculateDigits() noexcept {
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

  [[nodiscard]] static constexpr auto Generate() noexcept -> std::array<char, kDigits + 1> {
    std::array<char, kDigits + 1> result = {};

    if constexpr (N == 0) {
      result[0] = '0';
      result[1] = '\0';
    } else {
      std::int64_t temp = N < 0 ? -N : N;
      std::size_t pos = kDigits;
      result[pos] = '\0';

      while (temp > 0) {
        result[--pos] = static_cast<char>('0' + (temp % 10));
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
[[nodiscard]] constexpr std::string_view GetIntegerString() noexcept {
  constexpr auto& str_array = IntToStringHelper<N>::kValue;
  return {str_array.data(), IntToStringHelper<N>::kDigits};
}

// =============================================================================
// Value name extraction from pretty function
// =============================================================================

template <typename T, T Value>
[[nodiscard]] constexpr std::string_view RawValueName() noexcept {
#if defined(__clang__)
  // Format: std::string_view ctti::detail::RawValueName() [T = TYPE; Value = VALUE]
  constexpr std::string_view function = CTTI_PRETTY_FUNCTION;
  constexpr std::string_view prefix = "Value = ";
  constexpr std::string_view suffix = "]";
#elif defined(__GNUC__) && !defined(__clang__)
  // Format: constexpr std::string_view ctti::detail::RawValueName() [with T = TYPE; T Value = VALUE; ...]
  constexpr std::string_view function = CTTI_PRETTY_FUNCTION;
  constexpr std::string_view prefix = "Value = ";
  constexpr std::string_view suffix = ";";
#elif defined(_MSC_VER)
  // Format: std::string_view __cdecl ctti::detail::RawValueName<TYPE, VALUE>(void)
  constexpr std::string_view function = CTTI_PRETTY_FUNCTION;
  constexpr std::string_view prefix = ",";  // After the type, find the comma
  constexpr std::string_view suffix = ">(void)";
#endif

  constexpr std::size_t prefix_pos = function.rfind(prefix);
  constexpr std::size_t start = prefix_pos + prefix.size();

#if defined(__GNUC__) && !defined(__clang__)
  // GCC: Find either ';' or ']', whichever comes first
  constexpr std::size_t end_semicolon = function.find(';', start);
  constexpr std::size_t end_bracket = function.find(']', start);
  constexpr std::size_t end = (end_semicolon < end_bracket) ? end_semicolon : end_bracket;
#else
  constexpr std::size_t end = function.rfind(suffix);
#endif

  static_assert(start < end, "Failed to extract value name from pretty function");

  constexpr std::string_view raw = TrimWhitespace(function.substr(start, end - start));

  // For scoped enums, extract just the value part (after the last ::)
  constexpr std::size_t last_colon = raw.rfind("::");
  if constexpr (last_colon != std::string_view::npos) {
    return raw.substr(last_colon + 2);
  } else {
    return raw;
  }
}

template <typename T, T Value>
constexpr auto MakeValueNameArray() noexcept {
  constexpr std::string_view name = RawValueName<T, Value>();
  return SubstringToArray(name, std::make_index_sequence<name.size()>{});
}

template <typename T, T Value>
struct ValueNameStorage {
  static constexpr auto kValue = MakeValueNameArray<T, Value>();
};

template <typename T, T Value>
[[nodiscard]] constexpr std::string_view GetStoredValueName() noexcept {
  constexpr auto& value = ValueNameStorage<T, Value>::kValue;
  return {value.data(), value.size() - 1};  // Exclude null terminator
}

// =============================================================================
// Default value name extraction
// =============================================================================

template <typename T, T Value>
[[nodiscard]] constexpr std::string_view DefaultNameOfValue() noexcept {
  if constexpr (std::same_as<T, bool>) {
    if constexpr (Value) {
      return "true";
    } else {
      return "false";
    }
  } else if constexpr (std::integral<T> && !std::same_as<T, char>) {
    return GetIntegerString<static_cast<std::int64_t>(Value)>();
  } else if constexpr (std::same_as<T, char>) {
    return GetIntegerString<static_cast<std::int64_t>(Value)>();
  } else if constexpr (std::is_enum_v<T>) {
    return GetStoredValueName<T, Value>();
  } else {
    return GetStoredValueName<T, Value>();
  }
}

// =============================================================================
// ADL customization point detection
// =============================================================================

template <typename T, typename = void>
struct HasAdlNameOfImpl : std::false_type {};

template <typename T>
struct HasAdlNameOfImpl<T, std::void_t<decltype(name_of_impl(Identity<T>{}))>> : std::true_type {};

template <typename T>
concept HasAdlNameOf = HasAdlNameOfImpl<T>::value;

template <typename T, T Value, typename = void>
struct HasAdlValueNameOfImpl : std::false_type {};

template <typename T, T Value>
struct HasAdlValueNameOfImpl<T, Value,
                             std::void_t<decltype(name_of_impl(Identity<T>{}, std::integral_constant<T, Value>{}))>>
    : std::true_type {};

template <typename T, T Value>
concept HasAdlValueNameOf = HasAdlValueNameOfImpl<T, Value>::value;

// =============================================================================
// Static member customization point detection
// =============================================================================

template <typename T>
concept HasStaticCttiNameOf = requires {
  { T::ctti_name_of() } -> std::convertible_to<std::string_view>;
};

// =============================================================================
// Main name_of implementation
// =============================================================================

template <typename T>
struct NameOfImpl {
  [[nodiscard]] static constexpr std::string_view Apply() noexcept {
    if constexpr (HasStaticCttiNameOf<T>) {
      return T::ctti_name_of();
    } else if constexpr (HasAdlNameOf<T>) {
      return name_of_impl(Identity<T>{});
    } else {
      return DefaultNameOfType<T>();
    }
  }
};

template <typename T, T Value>
struct ValueNameOfImpl {
  [[nodiscard]] static constexpr std::string_view Apply() noexcept {
    if constexpr (HasAdlValueNameOf<T, Value>) {
      return name_of_impl(Identity<T>{}, std::integral_constant<T, Value>{});
    } else {
      return DefaultNameOfValue<T, Value>();
    }
  }
};

// =============================================================================
// Qualified name support
// =============================================================================

class QualifiedName final {
public:
  explicit constexpr QualifiedName(std::string_view full_name) noexcept : full_name_(full_name) {}

  constexpr QualifiedName(const QualifiedName&) noexcept = default;
  constexpr QualifiedName(QualifiedName&&) noexcept = default;
  constexpr ~QualifiedName() noexcept = default;

  constexpr QualifiedName& operator=(const QualifiedName&) noexcept = default;
  constexpr QualifiedName& operator=(QualifiedName&&) noexcept = default;

  /**
   * @brief Gets the unqualified name (last component).
   * @return The base name without namespace qualifiers.
   */
  [[nodiscard]] constexpr std::string_view GetName() const noexcept {
    const auto last_colon_pos = full_name_.rfind("::");
    return last_colon_pos == std::string_view::npos ? full_name_ : full_name_.substr(last_colon_pos + 2);
  }

  /**
   * @brief Gets the full qualified name.
   * @return The complete name with all qualifiers.
   */
  [[nodiscard]] constexpr std::string_view GetFullName() const noexcept { return full_name_; }

  /**
   * @brief Gets a specific qualifier by index.
   * @param index The zero-based index of the qualifier.
   * @return The qualifier at the given index, or empty if out of bounds.
   */
  [[nodiscard]] constexpr std::string_view GetQualifier(std::size_t index) const noexcept;

  constexpr bool operator==(const QualifiedName& other) const noexcept { return full_name_ == other.full_name_; }

private:
  std::string_view full_name_;
};

constexpr std::string_view QualifiedName::GetQualifier(std::size_t index) const noexcept {
  std::size_t current_index = 0;
  std::size_t start = 0;

  while (start < full_name_.size()) {
    const std::size_t separator = full_name_.find("::", start);

    if (separator == std::string_view::npos) {
      // No more separators - this is the last component (the name itself)
      return {};
    }

    if (current_index == index) {
      return full_name_.substr(start, separator - start);
    }

    ++current_index;
    start = separator + 2;
  }

  return {};
}

}  // namespace ctti::detail
