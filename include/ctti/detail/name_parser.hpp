#pragma once

#include <cstddef>
#include <cstdint>
#include <string_view>

namespace ctti::detail {

enum class ParseError : std::uint8_t { kInvalidFormat, kMissingDelimiter, kEmptyInput };

// Simple expected-like wrapper for C++20 compatibility
template <typename T>
class ParseResult {
public:
  constexpr ParseResult() noexcept : value_() {}
  constexpr ParseResult(T val) noexcept : value_(val) {}
  constexpr ParseResult(ParseError err) noexcept : error_(err) {}

  [[nodiscard]] constexpr bool has_value() const noexcept { return has_value_; }

  [[nodiscard]] constexpr T value() const noexcept { return value_; }
  [[nodiscard]] constexpr T operator*() const noexcept { return value_; }

  [[nodiscard]] constexpr ParseError error() const noexcept { return error_; }

private:
  union {
    T value_;
    ParseError error_;
  };
  bool has_value_ = false;
};

class NameParser {
public:
  [[nodiscard]] static constexpr auto FilterPrefix(std::string_view str, std::string_view prefix) noexcept
      -> ParseResult<std::string_view> {
    if (str.empty()) {
      return ParseError::kEmptyInput;
    }

    if (str.size() >= prefix.size() && str.starts_with(prefix)) {
      return str.substr(prefix.size());
    }
    return str;
  }

  [[nodiscard]] static constexpr std::string_view LeftPad(std::string_view str) noexcept {
    while (!str.empty() && str.front() == ' ') {
      str = str.substr(1);
    }
    return str;
  }

  [[nodiscard]] static constexpr std::string_view FilterClass(std::string_view type_name) noexcept {
    const auto result = FilterPrefix(LeftPad(type_name), "class");
    return result.has_value() ? LeftPad(*result) : type_name;
  }

  [[nodiscard]] static constexpr std::string_view FilterStruct(std::string_view type_name) noexcept {
    const auto result = FilterPrefix(LeftPad(type_name), "struct");
    return result.has_value() ? LeftPad(*result) : type_name;
  }

  [[nodiscard]] static constexpr std::string_view FilterTypenamePrefix(std::string_view type_name) noexcept {
    return FilterStruct(FilterClass(type_name));
  }

  [[nodiscard]] static constexpr std::size_t FindIth(std::string_view name, std::string_view substring,
                                                     std::size_t index) noexcept;

  [[nodiscard]] static constexpr std::string_view FilterEnumValue(std::string_view name) noexcept;
};

constexpr std::size_t NameParser::FindIth(std::string_view name, std::string_view substring,
                                          std::size_t index) noexcept {
  if (substring.empty()) {
    // For empty substring, return the index if it's within bounds, otherwise npos
    return index <= name.size() ? index : std::string_view::npos;
  }

  std::size_t pos = 0;
  std::size_t found = 0;

  while (pos < name.size()) {
    const auto next_pos = name.find(substring, pos);
    if (next_pos == std::string_view::npos) {
      return std::string_view::npos;
    }

    if (found == index) {
      return next_pos;
    }

    ++found;
    pos = next_pos + substring.size();
  }

  return std::string_view::npos;
}

constexpr std::string_view NameParser::FilterEnumValue(std::string_view name) noexcept {
  const auto open_paren = name.find('(');
  if (open_paren == std::string_view::npos) {
    return name;
  }

  // Find the matching closing parenthesis, handling nested parentheses
  std::size_t paren_count = 0;
  std::size_t close_paren = std::string_view::npos;

  for (std::size_t i = open_paren; i < name.size(); ++i) {
    if (name[i] == '(') {
      ++paren_count;
    } else if (name[i] == ')') {
      --paren_count;
      if (paren_count == 0) {
        close_paren = i;
        break;
      }
    }
  }

  if (close_paren != std::string_view::npos && open_paren < close_paren) {
    return name.substr(open_paren + 1, close_paren - open_paren - 1);
  }

  return name;
}

}  // namespace ctti::detail
