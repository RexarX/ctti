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

constexpr std::string_view name_of_impl([[maybe_unused]] ctti::detail::Identity<std::nullptr_t> identity) noexcept {
  return "std::nullptr_t";
}

constexpr std::string_view name_of_impl([[maybe_unused]] ctti::detail::Identity<int> identity) noexcept {
  return "int";
}

constexpr std::string_view name_of_impl([[maybe_unused]] ctti::detail::Identity<double> identity) noexcept {
  return "double";
}

constexpr std::string_view name_of_impl([[maybe_unused]] ctti::detail::Identity<float> identity) noexcept {
  return "float";
}

constexpr std::string_view name_of_impl([[maybe_unused]] ctti::detail::Identity<char> identity) noexcept {
  return "char";
}

constexpr std::string_view name_of_impl([[maybe_unused]] ctti::detail::Identity<bool> identity) noexcept {
  return "bool";
}

constexpr std::string_view name_of_impl([[maybe_unused]] ctti::detail::Identity<void> identity) noexcept {
  return "void";
}

constexpr std::string_view name_of_impl([[maybe_unused]] ctti::detail::Identity<long> identity) noexcept {
  return "long";
}

constexpr std::string_view name_of_impl([[maybe_unused]] ctti::detail::Identity<long long> identity) noexcept {
  return "long long";
}

constexpr std::string_view name_of_impl([[maybe_unused]] ctti::detail::Identity<unsigned int> identity) noexcept {
  return "unsigned int";
}

constexpr std::string_view name_of_impl([[maybe_unused]] ctti::detail::Identity<unsigned long> identity) noexcept {
  return "unsigned long";
}

constexpr std::string_view name_of_impl([[maybe_unused]] ctti::detail::Identity<unsigned long long> identity) noexcept {
  return "unsigned long long";
}

constexpr std::string_view name_of_impl([[maybe_unused]] ctti::detail::Identity<short> identity) noexcept {
  return "short";
}

constexpr std::string_view name_of_impl([[maybe_unused]] ctti::detail::Identity<unsigned short> identity) noexcept {
  return "unsigned short";
}

constexpr std::string_view name_of_impl([[maybe_unused]] ctti::detail::Identity<signed char> identity) noexcept {
  return "signed char";
}

constexpr std::string_view name_of_impl([[maybe_unused]] ctti::detail::Identity<unsigned char> identity) noexcept {
  return "unsigned char";
}

constexpr std::string_view name_of_impl([[maybe_unused]] ctti::detail::Identity<long double> identity) noexcept {
  return "long double";
}

constexpr std::string_view name_of_impl([[maybe_unused]] ctti::detail::Identity<char8_t> identity) noexcept {
  return "char8_t";
}

constexpr std::string_view name_of_impl([[maybe_unused]] ctti::detail::Identity<char16_t> identity) noexcept {
  return "char16_t";
}

constexpr std::string_view name_of_impl([[maybe_unused]] ctti::detail::Identity<char32_t> identity) noexcept {
  return "char32_t";
}

constexpr std::string_view name_of_impl([[maybe_unused]] ctti::detail::Identity<wchar_t> identity) noexcept {
  return "wchar_t";
}

template <typename T, std::size_t N>
constexpr std::string_view name_of_impl([[maybe_unused]] ctti::detail::Identity<std::array<T, N>> identity) noexcept {
  return "std::array";
}

constexpr std::string_view name_of_impl([[maybe_unused]] ctti::detail::Identity<std::string> identity) noexcept {
  return "std::string";
}

constexpr std::string_view name_of_impl([[maybe_unused]] ctti::detail::Identity<std::string_view> identity) noexcept {
  return "std::string_view";
}

template <typename T>
constexpr std::string_view name_of_impl([[maybe_unused]] ctti::detail::Identity<std::vector<T>> identity) noexcept {
  return "std::vector";
}

template <typename T>
constexpr std::string_view name_of_impl([[maybe_unused]] ctti::detail::Identity<std::unique_ptr<T>> identity) noexcept {
  return "std::unique_ptr";
}

template <typename T>
constexpr std::string_view name_of_impl([[maybe_unused]] ctti::detail::Identity<std::shared_ptr<T>> identity) noexcept {
  return "std::shared_ptr";
}
