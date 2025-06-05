#ifndef CTTI_DETAIL_COMPILE_TIME_STRING_HPP
#define CTTI_DETAIL_COMPILE_TIME_STRING_HPP

#include <algorithm>
#include <array>
#include <string_view>

namespace ctti::detail {

template <std::size_t N>
struct CompileTimeString {
  std::array<char, N> data_ = {};

  consteval CompileTimeString() noexcept = default;
  consteval CompileTimeString(const char (&str)[N]) noexcept { std::copy_n(str, N, data_.begin()); }

  consteval bool starts_with(std::string_view prefix) const noexcept { return view().starts_with(prefix); }

  consteval bool ends_with(std::string_view suffix) const noexcept { return view().ends_with(suffix); }

  consteval std::size_t find(char ch) const noexcept {
    auto sv = view();
    auto pos = sv.find(ch);
    return pos == std::string_view::npos ? N - 1 : pos;
  }

  consteval std::size_t find(std::string_view substr) const noexcept {
    auto sv = view();
    auto pos = sv.find(substr);
    return pos == std::string_view::npos ? N - 1 : pos;
  }

  template <std::size_t Pos, std::size_t Len = N>
    requires(Pos <= N - 1)
  consteval auto substr() const noexcept {
    constexpr std::size_t actual_len = std::min(Len, (N - 1) - Pos);
    CompileTimeString<actual_len + 1> result;
    std::copy_n(data_.data() + Pos, actual_len, result.data_.begin());
    result.data_[actual_len] = '\0';
    return result;
  }

  template <std::size_t M>
  consteval auto operator+(const CompileTimeString<M>& other) const noexcept {
    CompileTimeString<N + M - 1> result;
    std::copy_n(data_.data(), N - 1, result.data_.begin());
    std::copy_n(other.data_.data(), M, result.data_.begin() + N - 1);
    return result;
  }

  consteval auto operator<=>(const CompileTimeString&) const noexcept = default;

  consteval char operator[](std::size_t index) const noexcept { return data_[index]; }

  consteval std::string_view view() const noexcept { return std::string_view{data_.data(), N - 1}; }

  consteval bool empty() const noexcept { return N <= 1; }

  consteval const char* c_str() const noexcept { return data_.data(); }

  consteval std::size_t size() const noexcept { return N - 1; }
};

template <std::size_t N>
CompileTimeString(const char (&)[N]) -> CompileTimeString<N>;

template <CompileTimeString Str>
consteval auto operator""_cts() noexcept {
  return Str;
}

template <std::size_t N>
consteval auto make_compile_time_string(const char (&str)[N]) noexcept {
  return CompileTimeString<N>{str};
}

consteval std::size_t string_literal_size(const char* str) noexcept {
  std::size_t size = 0;
  while (str[size] != '\0') {
    ++size;
  }
  return size + 1;
}

}  // namespace ctti::detail

#endif  // CTTI_DETAIL_COMPILE_TIME_STRING_HPP
