#ifndef CTTI_LANGUAGE_FEATURES_HPP
#define CTTI_LANGUAGE_FEATURES_HPP

// C++23 is the minimum requirement now
#define CTTI_HAS_VARIABLE_TEMPLATES 1
#define CTTI_HAS_CONSTEXPR_PRETTY_FUNCTION 1

#if defined(__clang__) || (defined(__GNUC__) && __GNUC__ >= 9)
#define CTTI_HAS_ENUM_AWARE_PRETTY_FUNCTION 1
#endif

#endif  // CTTI_LANGUAGE_FEATURES_HPP
