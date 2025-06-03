#ifndef CTTI_DETAIL_NAME_FILTERS_HPP
#define CTTI_DETAIL_NAME_FILTERS_HPP

#include <string_view>

namespace ctti::detail {

constexpr std::string_view FilterPrefix(std::string_view str, std::string_view prefix) noexcept {
  if (str.size() >= prefix.size() && str.starts_with(prefix)) {
    return str.substr(prefix.size());
  }
  return str;
}

constexpr std::string_view LeftPad(std::string_view str) noexcept {
  if (!str.empty() && str.front() == ' ') {
    return LeftPad(str.substr(1));
  }
  return str;
}

constexpr std::string_view FilterClass(std::string_view type_name) noexcept {
  return LeftPad(FilterPrefix(LeftPad(type_name), "class"));
}

constexpr std::string_view FilterStruct(std::string_view type_name) noexcept {
  return LeftPad(FilterPrefix(LeftPad(type_name), "struct"));
}

constexpr std::string_view FilterTypenamePrefix(std::string_view type_name) noexcept {
  return FilterStruct(FilterClass(type_name));
}

constexpr std::size_t FindIth(std::string_view name, std::string_view substring, std::size_t index) {
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

constexpr std::string_view FilterEnumValue(std::string_view name) noexcept {
  const auto open_paren = name.find('(');
  if (open_paren == std::string_view::npos) {
    return name;
  }

  // Find the matching closing parenthesis
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

#endif  // CTTI_DETAIL_NAME_FILTERS_HPP
