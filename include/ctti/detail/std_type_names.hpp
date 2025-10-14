#pragma once

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

[[nodiscard]] constexpr std::string_view name_of_impl(ctti::detail::Identity<std::nullptr_t> /*identity*/) noexcept {
  return "std::nullptr_t";
}

[[nodiscard]] constexpr std::string_view name_of_impl(ctti::detail::Identity<int> /*identity*/) noexcept {
  return "int";
}

[[nodiscard]] constexpr std::string_view name_of_impl(ctti::detail::Identity<double> /*identity*/) noexcept {
  return "double";
}

[[nodiscard]] constexpr std::string_view name_of_impl(ctti::detail::Identity<float> /*identity*/) noexcept {
  return "float";
}

[[nodiscard]] constexpr std::string_view name_of_impl(ctti::detail::Identity<char> /*identity*/) noexcept {
  return "char";
}

[[nodiscard]] constexpr std::string_view name_of_impl(ctti::detail::Identity<bool> /*identity*/) noexcept {
  return "bool";
}

[[nodiscard]] constexpr std::string_view name_of_impl(ctti::detail::Identity<void> /*identity*/) noexcept {
  return "void";
}

[[nodiscard]] constexpr std::string_view name_of_impl(ctti::detail::Identity<long> /*identity*/) noexcept {
  return "long";
}

[[nodiscard]] constexpr std::string_view name_of_impl(ctti::detail::Identity<long long> /*identity*/) noexcept {
  return "long long";
}

[[nodiscard]] constexpr std::string_view name_of_impl(ctti::detail::Identity<unsigned int> /*identity*/) noexcept {
  return "unsigned int";
}

[[nodiscard]] constexpr std::string_view name_of_impl(ctti::detail::Identity<unsigned long> /*identity*/) noexcept {
  return "unsigned long";
}

[[nodiscard]] constexpr std::string_view name_of_impl(
    ctti::detail::Identity<unsigned long long> /*identity*/) noexcept {
  return "unsigned long long";
}

[[nodiscard]] constexpr std::string_view name_of_impl(ctti::detail::Identity<short> /*identity*/) noexcept {
  return "short";
}

[[nodiscard]] constexpr std::string_view name_of_impl(ctti::detail::Identity<unsigned short> /*identity*/) noexcept {
  return "unsigned short";
}

[[nodiscard]] constexpr std::string_view name_of_impl(ctti::detail::Identity<signed char> /*identity*/) noexcept {
  return "signed char";
}

[[nodiscard]] constexpr std::string_view name_of_impl(ctti::detail::Identity<unsigned char> /*identity*/) noexcept {
  return "unsigned char";
}

[[nodiscard]] constexpr std::string_view name_of_impl(ctti::detail::Identity<long double> /*identity*/) noexcept {
  return "long double";
}

[[nodiscard]] constexpr std::string_view name_of_impl(ctti::detail::Identity<char8_t> /*identity*/) noexcept {
  return "char8_t";
}

[[nodiscard]] constexpr std::string_view name_of_impl(ctti::detail::Identity<char16_t> /*identity*/) noexcept {
  return "char16_t";
}

[[nodiscard]] constexpr std::string_view name_of_impl(ctti::detail::Identity<char32_t> /*identity*/) noexcept {
  return "char32_t";
}

[[nodiscard]] constexpr std::string_view name_of_impl(ctti::detail::Identity<wchar_t> /*identity*/) noexcept {
  return "wchar_t";
}

template <typename T, std::size_t N>
[[nodiscard]] constexpr std::string_view name_of_impl(ctti::detail::Identity<std::array<T, N>> /*identity*/) noexcept {
  return "std::array";
}

[[nodiscard]] constexpr std::string_view name_of_impl(ctti::detail::Identity<std::string> /*identity*/) noexcept {
  return "std::string";
}

[[nodiscard]] constexpr std::string_view name_of_impl(ctti::detail::Identity<std::string_view> /*identity*/) noexcept {
  return "std::string_view";
}

template <typename T>
[[nodiscard]] constexpr std::string_view name_of_impl(ctti::detail::Identity<std::vector<T>> /*identity*/) noexcept {
  return "std::vector";
}

template <typename T>
[[nodiscard]] constexpr std::string_view name_of_impl(
    ctti::detail::Identity<std::unique_ptr<T>> /*identity*/) noexcept {
  return "std::unique_ptr";
}

template <typename T>
[[nodiscard]] constexpr std::string_view name_of_impl(
    ctti::detail::Identity<std::shared_ptr<T>> /*identity*/) noexcept {
  return "std::shared_ptr";
}
