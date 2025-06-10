#ifndef CTTI_DETAIL_STD_TYPE_NAMES_HPP
#define CTTI_DETAIL_STD_TYPE_NAMES_HPP

#include <ctti/detail/meta.hpp>

#include <array>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

namespace ctti::detail {

template <typename T>
struct Identity;

}  // namespace ctti::detail

// Specializations for std types with their common aliases

constexpr std::string_view name_of_impl(ctti::detail::Identity<std::nullptr_t>) noexcept {
  return "std::nullptr_t";
}

constexpr std::string_view name_of_impl(ctti::detail::Identity<int>) noexcept {
  return "int";
}

constexpr std::string_view name_of_impl(ctti::detail::Identity<double>) noexcept {
  return "double";
}

constexpr std::string_view name_of_impl(ctti::detail::Identity<float>) noexcept {
  return "float";
}

constexpr std::string_view name_of_impl(ctti::detail::Identity<char>) noexcept {
  return "char";
}

constexpr std::string_view name_of_impl(ctti::detail::Identity<bool>) noexcept {
  return "bool";
}

constexpr std::string_view name_of_impl(ctti::detail::Identity<void>) noexcept {
  return "void";
}

constexpr std::string_view name_of_impl(ctti::detail::Identity<long>) noexcept {
  return "long";
}

constexpr std::string_view name_of_impl(ctti::detail::Identity<long long>) noexcept {
  return "long long";
}

constexpr std::string_view name_of_impl(ctti::detail::Identity<unsigned int>) noexcept {
  return "unsigned int";
}

constexpr std::string_view name_of_impl(ctti::detail::Identity<unsigned long>) noexcept {
  return "unsigned long";
}

constexpr std::string_view name_of_impl(ctti::detail::Identity<unsigned long long>) noexcept {
  return "unsigned long long";
}

constexpr std::string_view name_of_impl(ctti::detail::Identity<short>) noexcept {
  return "short";
}

constexpr std::string_view name_of_impl(ctti::detail::Identity<unsigned short>) noexcept {
  return "unsigned short";
}

constexpr std::string_view name_of_impl(ctti::detail::Identity<signed char>) noexcept {
  return "signed char";
}

constexpr std::string_view name_of_impl(ctti::detail::Identity<unsigned char>) noexcept {
  return "unsigned char";
}

constexpr std::string_view name_of_impl(ctti::detail::Identity<long double>) noexcept {
  return "long double";
}

constexpr std::string_view name_of_impl(ctti::detail::Identity<char8_t>) noexcept {
  return "char8_t";
}

constexpr std::string_view name_of_impl(ctti::detail::Identity<char16_t>) noexcept {
  return "char16_t";
}

constexpr std::string_view name_of_impl(ctti::detail::Identity<char32_t>) noexcept {
  return "char32_t";
}

constexpr std::string_view name_of_impl(ctti::detail::Identity<wchar_t>) noexcept {
  return "wchar_t";
}

template <typename T, std::size_t N>
constexpr std::string_view name_of_impl(ctti::detail::Identity<std::array<T, N>>) noexcept {
  return "std::array";
}

constexpr std::string_view name_of_impl(ctti::detail::Identity<std::string>) noexcept {
  return "std::string";
}

constexpr std::string_view name_of_impl(ctti::detail::Identity<std::string_view>) noexcept {
  return "std::string_view";
}

template <typename T>
constexpr std::string_view name_of_impl(ctti::detail::Identity<std::vector<T>>) noexcept {
  return "std::vector";
}

template <typename T>
constexpr std::string_view name_of_impl(ctti::detail::Identity<std::unique_ptr<T>>) noexcept {
  return "std::unique_ptr";
}

template <typename T>
constexpr std::string_view name_of_impl(ctti::detail::Identity<std::shared_ptr<T>>) noexcept {
  return "std::shared_ptr";
}

#endif  // CTTI_DETAIL_STD_TYPE_NAMES_HPP
