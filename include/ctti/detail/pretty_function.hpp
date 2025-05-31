#ifndef CTTI_PRETTY_FUNCTION_HPP
#define CTTI_PRETTY_FUNCTION_HPP

#include <string_view>

#if defined(__clang__)
#define CTTI_PRETTY_FUNCTION __PRETTY_FUNCTION__
#elif defined(__GNUC__) && !defined(__clang__)
#define CTTI_PRETTY_FUNCTION __PRETTY_FUNCTION__
#elif defined(_MSC_VER)
#define CTTI_PRETTY_FUNCTION __FUNCSIG__
#else
#error "No support for this compiler."
#endif

#if defined(__clang__)
#define CTTI_HAS_ENUM_AWARE_PRETTY_FUNCTION 1
#elif defined(__GNUC__) && (__GNUC__ >= 9)
#define CTTI_HAS_ENUM_AWARE_PRETTY_FUNCTION 1
#endif

namespace ctti::pretty_function {

template <typename T>
constexpr std::string_view type() {
  return CTTI_PRETTY_FUNCTION;
}

template <typename T, T Value>
constexpr std::string_view value() {
  return CTTI_PRETTY_FUNCTION;
}

}  // namespace ctti::pretty_function

#if defined(__clang__)
#define CTTI_TYPE_PRETTY_FUNCTION_PREFIX "std::string_view ctti::pretty_function::type() [T = "
#define CTTI_TYPE_PRETTY_FUNCTION_SUFFIX "]"
#elif defined(__GNUC__) && !defined(__clang__)
#define CTTI_TYPE_PRETTY_FUNCTION_PREFIX "constexpr std::string_view ctti::pretty_function::type() [with T = "
#define CTTI_TYPE_PRETTY_FUNCTION_SUFFIX "]"
#elif defined(_MSC_VER)
#define CTTI_TYPE_PRETTY_FUNCTION_PREFIX \
  "class std::basic_string_view<char,struct std::char_traits<char> > __cdecl ctti::pretty_function::type<"
#define CTTI_TYPE_PRETTY_FUNCTION_SUFFIX ">(void)"
#else
#error "No support for this compiler."
#endif

#define CTTI_TYPE_PRETTY_FUNCTION_LEFT (sizeof(CTTI_TYPE_PRETTY_FUNCTION_PREFIX) - 1)
#define CTTI_TYPE_PRETTY_FUNCTION_RIGHT (sizeof(CTTI_TYPE_PRETTY_FUNCTION_SUFFIX) - 1)

#if defined(__clang__)
#define CTTI_VALUE_PRETTY_FUNCTION_PREFIX "std::string_view ctti::pretty_function::value() [T = "
#define CTTI_VALUE_PRETTY_FUNCTION_SEPARATOR "; Value = "
#define CTTI_VALUE_PRETTY_FUNCTION_SUFFIX "]"
#elif defined(__GNUC__) && !defined(__clang__)
#define CTTI_VALUE_PRETTY_FUNCTION_PREFIX "constexpr std::string_view ctti::pretty_function::value() [with T = "
#define CTTI_VALUE_PRETTY_FUNCTION_SEPARATOR "; T Value = "
#define CTTI_VALUE_PRETTY_FUNCTION_SUFFIX "]"
#elif defined(_MSC_VER)
#define CTTI_VALUE_PRETTY_FUNCTION_PREFIX \
  "class std::basic_string_view<char,struct std::char_traits<char> > __cdecl ctti::pretty_function::value<"
#define CTTI_VALUE_PRETTY_FUNCTION_SEPARATOR "; T Value = "
#define CTTI_VALUE_PRETTY_FUNCTION_SUFFIX ">(void)"
#else
#error "No support for this compiler."
#endif

#define CTTI_VALUE_PRETTY_FUNCTION_LEFT (sizeof(CTTI_VALUE_PRETTY_FUNCTION_PREFIX) - 1)
#define CTTI_VALUE_PRETTY_FUNCTION_SEPARATION (sizeof(CTTI_VALUE_PRETTY_FUNCTION_SEPARATOR) - 1)
#define CTTI_VALUE_PRETTY_FUNCTION_RIGHT (sizeof(CTTI_VALUE_PRETTY_FUNCTION_SUFFIX) - 1)

#endif  // CTTI_PRETTY_FUNCTION_HPP
