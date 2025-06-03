#ifndef CTTI_NAME_HPP
#define CTTI_NAME_HPP

#include <ctti/detail/name_impl.hpp>
#include <ctti/type_tag.hpp>

#include <string_view>

namespace ctti {

class qualified_name {
public:
  constexpr qualified_name(detail::QualifiedName qualified_name) noexcept : qualified_name_(qualified_name) {}

  constexpr std::string_view get_name() const noexcept { return qualified_name_.GetName(); }

  constexpr std::string_view get_full_name() const noexcept { return qualified_name_.GetFullName(); }

  constexpr std::string_view get_qualifier(std::size_t index) const noexcept {
    return qualified_name_.GetQualifier(index);
  }

  constexpr bool operator==(const qualified_name& other) const noexcept {
    return qualified_name_ == other.qualified_name_;
  }

private:
  detail::QualifiedName qualified_name_;
};

template <typename T>
constexpr std::string_view name_of() noexcept {
  return detail::NameOfImpl<T>::Apply();
}

template <typename T, T Value>
constexpr std::string_view name_of() noexcept {
  return detail::ValueNameOfImpl<T, Value>::Apply();
}

template <typename T>
constexpr qualified_name qualified_name_of() noexcept {
  return detail::QualifiedName(name_of<T>());
}

template <typename T, T Value>
constexpr qualified_name qualified_name_of() noexcept {
  return detail::QualifiedName(name_of<T, Value>());
}

}  // namespace ctti

#endif  // CTTI_NAME_HPP
