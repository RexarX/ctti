#ifndef CTTI_DETAIL_PRETTY_FUNCTION_HPP
#define CTTI_DETAIL_PRETTY_FUNCTION_HPP

#include <string_view>

#if defined(__clang__)
#define CTTI_PRETTY_FUNCTION __PRETTY_FUNCTION__
#elif defined(__GNUC__) && !defined(__clang__)
#define CTTI_PRETTY_FUNCTION __PRETTY_FUNCTION__
#elif defined(_MSC_VER)
#define CTTI_PRETTY_FUNCTION __FUNCSIG__
#else
#error "No support for this compiler."
#endif

namespace ctti::detail {

struct PrettyFunction {
  template <typename T>
  static constexpr std::string_view Type() noexcept {
    return CTTI_PRETTY_FUNCTION;
  }

  template <typename T, T ValueParam>
  static constexpr std::string_view Value() noexcept {
    return CTTI_PRETTY_FUNCTION;
  }
};

inline namespace pretty_function {

template <typename T>
constexpr std::string_view type() noexcept {
  return PrettyFunction::Type<T>();
}

template <typename T, T ValueParam>
constexpr std::string_view value() noexcept {
  return PrettyFunction::Value<T, ValueParam>();
}

template <typename T>
constexpr std::string_view Type() noexcept {
  return PrettyFunction::Type<T>();
}

template <typename T, T ValueParam>
constexpr std::string_view Value() noexcept {
  return PrettyFunction::Value<T, ValueParam>();
}

}  // namespace pretty_function

constexpr bool IsAlphaNumeric(char c) noexcept {
  return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9');
}

constexpr bool IsValidIdentifierChar(char c) noexcept {
  return IsAlphaNumeric(c) || c == '_';
}

constexpr std::string_view FindTypeInSignature(std::string_view signature, std::string_view known_type) noexcept {
  const auto pos = signature.find(known_type);
  if (pos == std::string_view::npos) {
    return {};
  }

  const bool is_start_valid = (pos == 0) || !IsValidIdentifierChar(signature[pos - 1]);
  const bool is_end_valid =
      (pos + known_type.size() >= signature.size()) || !IsValidIdentifierChar(signature[pos + known_type.size()]);

  return (is_start_valid && is_end_valid) ? known_type : std::string_view{};
}

constexpr std::pair<std::size_t, std::size_t> DetectPrettyFunctionOffsetsForCompiler() noexcept {
#if defined(__clang__)
  constexpr std::string_view test_signature = PrettyFunction::Type<int>();

  constexpr auto eq_pos = test_signature.find(" = ");
  if (eq_pos != std::string_view::npos) {
    constexpr auto bracket_pos = test_signature.find(']', eq_pos);
    if (bracket_pos != std::string_view::npos) {
      return {eq_pos + 3, test_signature.size() - bracket_pos};
    }
  }

  constexpr auto int_pos = test_signature.find("int");
  if (int_pos != std::string_view::npos) {
    constexpr bool is_start_valid = (int_pos == 0) || !IsValidIdentifierChar(test_signature[int_pos - 1]);
    constexpr bool is_end_valid =
        (int_pos + 3 >= test_signature.size()) || !IsValidIdentifierChar(test_signature[int_pos + 3]);
    if (is_start_valid && is_end_valid) {
      return {int_pos, test_signature.size() - int_pos - 3};
    }
  }

  return {75, 1};

#elif defined(__GNUC__) && !defined(__clang__)
  constexpr std::string_view test_signature = PrettyFunction::Type<int>();

  constexpr auto with_pos = test_signature.find("with ");
  if (with_pos != std::string_view::npos) {
    constexpr auto eq_pos = test_signature.find(" = ", with_pos);
    if (eq_pos != std::string_view::npos) {
      constexpr auto bracket_pos = test_signature.find(']', eq_pos);
      if (bracket_pos != std::string_view::npos) {
        return {eq_pos + 3, test_signature.size() - bracket_pos};
      }
    }
  }

  constexpr auto int_pos = test_signature.find("int");
  if (int_pos != std::string_view::npos) {
    constexpr bool is_start_valid = (int_pos == 0) || !IsValidIdentifierChar(test_signature[int_pos - 1]);
    constexpr bool is_end_valid =
        (int_pos + 3 >= test_signature.size()) || !IsValidIdentifierChar(test_signature[int_pos + 3]);
    if (is_start_valid && is_end_valid) {
      return {int_pos, test_signature.size() - int_pos - 3};
    }
  }

  return {81, 1};

#elif defined(_MSC_VER) && !defined(__clang__)
  constexpr std::string_view test_signature = PrettyFunction::Type<int>();

  // MSVC format is typically: "return_type __cdecl function_name<type>(void)"
  // Look for the last '<' and first '>' to find the type
  constexpr auto last_angle_start = test_signature.rfind('<');
  constexpr auto first_angle_end = test_signature.find('>', last_angle_start);
  if (last_angle_start != std::string_view::npos && first_angle_end != std::string_view::npos) {
    return {last_angle_start + 1, test_signature.size() - first_angle_end};
  }

  // Fallback values for MSVC
  return {84, 7};

#else
  return {0, 0};
#endif
}

constexpr std::pair<std::size_t, std::size_t> kPrettyFunctionOffsets = DetectPrettyFunctionOffsetsForCompiler();
constexpr std::size_t kTypePrettyFunctionLeft = kPrettyFunctionOffsets.first;
constexpr std::size_t kTypePrettyFunctionRight = kPrettyFunctionOffsets.second;

}  // namespace ctti::detail

#endif  // CTTI_DETAIL_PRETTY_FUNCTION_HPP
