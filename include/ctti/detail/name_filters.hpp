#ifndef CTTI_DETAIL_NAMEFILTERS_HPP
#define CTTI_DETAIL_NAMEFILTERS_HPP

#include <string_view>

namespace ctti::detail {

constexpr std::string_view filter_prefix(std::string_view str, std::string_view prefix) noexcept {
  if (str.size() >= prefix.size() && str.starts_with(prefix)) {
    return str.substr(prefix.size());
  }
  return str;
}

constexpr std::string_view leftpad(std::string_view str) noexcept {
  if (!str.empty() && str.front() == ' ') {
    return leftpad(str.substr(1));
  }
  return str;
}

constexpr std::string_view filter_class(std::string_view type_name) noexcept {
  return leftpad(filter_prefix(leftpad(type_name), "class"));
}

constexpr std::string_view filter_struct(std::string_view type_name) noexcept {
  return leftpad(filter_prefix(leftpad(type_name), "struct"));
}

constexpr std::string_view filter_typename_prefix(std::string_view type_name) noexcept {
  return filter_struct(filter_class(type_name));
}

// Helper functions to find substrings in string_view
constexpr std::size_t find_ith(std::string_view name, std::string_view substring, std::size_t index) {
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

constexpr std::string_view filter_enum_value(std::string_view name) noexcept {
  // Check if it looks like an enum value format "(Enum)N"
  const auto open_paren = name.find('(');
  const auto close_paren = name.find(')', open_paren);

  if (open_paren != std::string_view::npos && close_paren != std::string_view::npos && open_paren < close_paren) {
    // Return just enum type name and value without parentheses
    return name.substr(open_paren + 1, close_paren - open_paren - 1);
  }

  return name;
}

}  // namespace ctti::detail

#endif  // CTTI_DETAIL_NAMEFILTERS_HPP
