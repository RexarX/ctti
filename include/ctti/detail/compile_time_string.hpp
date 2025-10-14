#pragma once

#include <algorithm>
#include <array>
#include <cstddef>
#include <string_view>

namespace ctti::detail {

template <std::size_t N>
struct CompileTimeString {
  std::array<char, N> data_ = {};

  consteval CompileTimeString() noexcept = default;

  /**
   * @brief Construct a CompileTimeString from a string literal.
   * @note Constructor is not marked explicit to allow implicit conversions from string literals.
   * @param str The string literal to construct from.
   */
  consteval CompileTimeString(const char (&str)[N]) noexcept { std::copy_n(str, N, data_.begin()); }

  template <std::size_t M>
    requires(M <= N)
  explicit consteval CompileTimeString(const CompileTimeString<M>& other) noexcept {
    std::copy_n(other.data_.data(), M, data_.begin());
  }

  consteval CompileTimeString(const CompileTimeString&) noexcept = default;
  consteval CompileTimeString(CompileTimeString&&) noexcept = default;
  constexpr ~CompileTimeString() noexcept = default;

  consteval CompileTimeString& operator=(const CompileTimeString&) noexcept = default;
  consteval CompileTimeString& operator=(CompileTimeString&&) noexcept = default;

  [[nodiscard]] constexpr std::string_view View() const noexcept { return {data_.data(), N - 1}; }

  [[nodiscard]] constexpr bool StartsWith(std::string_view prefix) const noexcept { return View().starts_with(prefix); }
  [[nodiscard]] constexpr bool EndsWith(std::string_view suffix) const noexcept { return View().ends_with(suffix); }

  [[nodiscard]] constexpr std::size_t Find(char ch) const noexcept {
    const auto sv = View();
    const auto pos = sv.find(ch);
    return pos == std::string_view::npos ? N - 1 : pos;
  }

  [[nodiscard]] constexpr std::size_t Find(std::string_view substring) const noexcept {
    const auto sv = View();
    const auto pos = sv.find(substring);
    return pos == std::string_view::npos ? N - 1 : pos;
  }

  template <std::size_t Start, std::size_t Length>
  [[nodiscard]] consteval auto Substr() const noexcept {
    CompileTimeString<Length + 1> result = {};
    std::copy_n(data_.data() + Start, Length, result.data_.begin());
    result.data_[Length] = '\0';
    return result;
  }

  template <std::size_t Start>
  [[nodiscard]] consteval auto Substr() const noexcept {
    constexpr std::size_t remaining = (Start < N - 1) ? (N - Start - 1) : 0;
    return Substr<Start, remaining>();
  }

  template <std::size_t M>
  consteval auto operator+(const CompileTimeString<M>& other) const noexcept {
    CompileTimeString<N + M - 1> result = {};
    std::copy_n(data_.data(), N - 1, result.data_.begin());
    std::copy_n(other.data_.data(), M, result.data_.begin() + N - 1);
    return result;
  }

  consteval auto operator<=>(const CompileTimeString&) const noexcept = default;
  consteval char operator[](std::size_t index) const noexcept { return data_[index]; }

  [[nodiscard]] consteval bool Empty() const noexcept { return N <= 1; }
  [[nodiscard]] consteval const char* CStr() const noexcept { return data_.data(); }
  [[nodiscard]] consteval std::size_t Size() const noexcept { return N - 1; }
};

template <std::size_t N>
CompileTimeString(const char (&)[N]) -> CompileTimeString<N>;

template <std::size_t N>
[[nodiscard]] consteval auto make_compile_time_string(const char (&str)[N]) noexcept {
  return CompileTimeString<N>{str};
}

namespace literals {

template <CompileTimeString str>
consteval auto operator""_cts() noexcept {
  return str;
}

}  // namespace literals

using namespace literals;

}  // namespace ctti::detail
