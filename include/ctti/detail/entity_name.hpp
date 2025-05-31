#ifndef CTTI_DETAIL_ENTITY_NAME_HPP
#define CTTI_DETAIL_ENTITY_NAME_HPP

#include <string_view>

namespace ctti::detail {

class entity_name {
public:
  constexpr entity_name(std::string_view str) noexcept : str_{str} {}

  constexpr std::string_view str() const noexcept { return str_; }

  constexpr std::string_view operator[](std::size_t i) const { return get_qualifier(i); }

private:
  std::string_view str_;

  constexpr std::string_view get_qualifier(std::size_t i) const {
    if (str_.empty()) return {};

    std::size_t start = 0;
    std::size_t current_qualifier = 0;

    for (std::size_t pos = 0; pos < str_.size(); ++pos) {
      if (pos + 1 < str_.size() && str_[pos] == ':' && str_[pos + 1] == ':') {
        if (current_qualifier == i) {
          return str_.substr(start, pos - start);
        }

        start = pos + 2;
        ++pos;  // Skip the second colon
        ++current_qualifier;
      }
    }

    // Handle the last part (after the last ::)
    if (current_qualifier == i) {
      return str_.substr(start);
    }

    return {};
  }
};

}  // namespace ctti::detail

#endif  // CTTI_DETAIL_ENTITY_NAME_HPP
