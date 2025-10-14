#pragma once

#include <cstddef>
#include <string_view>

namespace ctti::detail {

class EntityName {
public:
  explicit constexpr EntityName(std::string_view str) noexcept : str_(str) {}
  constexpr EntityName(const EntityName&) noexcept = default;
  constexpr EntityName(EntityName&&) noexcept = default;
  constexpr ~EntityName() noexcept = default;

  constexpr EntityName& operator=(const EntityName&) noexcept = default;
  constexpr EntityName& operator=(EntityName&&) noexcept = default;

  [[nodiscard]] constexpr std::string_view GetStr() const noexcept { return str_; }
  [[nodiscard]] constexpr std::string_view operator[](std::size_t index) const noexcept { return GetQualifier(index); }

private:
  [[nodiscard]] constexpr std::string_view GetQualifier(std::size_t index) const noexcept;

  std::string_view str_;
};

constexpr std::string_view EntityName::GetQualifier(std::size_t index) const noexcept {
  if (str_.empty()) {
    return {};
  }

  std::size_t start = 0;
  std::size_t current_qualifier = 0;

  for (std::size_t pos = 0; pos < str_.size();) {
    if (pos + 1 < str_.size() && str_[pos] == ':' && str_[pos + 1] == ':') {
      if (current_qualifier == index) {
        return str_.substr(start, pos - start);
      }

      start = pos + 2;
      pos += 2;
      ++current_qualifier;
    } else {
      ++pos;
    }
  }

  if (current_qualifier == index) {
    return str_.substr(start);
  }

  return {};
}

}  // namespace ctti::detail
