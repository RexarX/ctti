#ifndef CTTI_DETAIL_ENTITY_NAME_HPP
#define CTTI_DETAIL_ENTITY_NAME_HPP

#include <string_view>

namespace ctti::detail {

class EntityName {
public:
  constexpr EntityName(std::string_view str) noexcept : str_(str) {}

  constexpr std::string_view GetStr() const noexcept { return str_; }
  constexpr std::string_view operator[](std::size_t i) const noexcept { return GetQualifier(i); }

private:
  constexpr std::string_view GetQualifier(std::size_t i) const noexcept {
    if (str_.empty()) return {};

    std::size_t start = 0;
    std::size_t current_qualifier = 0;

    for (std::size_t pos = 0; pos < str_.size();) {
      if (pos + 1 < str_.size() && str_[pos] == ':' && str_[pos + 1] == ':') {
        if (current_qualifier == i) {
          return str_.substr(start, pos - start);
        }

        start = pos + 2;
        pos += 2;
        ++current_qualifier;
      } else {
        ++pos;
      }
    }

    if (current_qualifier == i) {
      return str_.substr(start);
    }

    return {};
  }

  std::string_view str_;
};

}  // namespace ctti::detail

#endif  // CTTI_DETAIL_ENTITY_NAME_HPP
