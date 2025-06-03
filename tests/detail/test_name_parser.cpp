#include "doctest.h"

#include <ctti/detail/name_parser.hpp>

#include <iostream>

TEST_SUITE("detail::name_parser") {
  TEST_CASE("filter_prefix_success") {
    auto result1 = ctti::detail::NameParser::FilterPrefix("class MyClass", "class");
    auto result2 = ctti::detail::NameParser::FilterPrefix("struct MyStruct", "struct");
    auto result3 = ctti::detail::NameParser::FilterPrefix("MyType", "nonexistent");

    CHECK(result1.has_value());
    CHECK(result1.value() == " MyClass");

    CHECK(result2.has_value());
    CHECK(result2.value() == " MyStruct");

    CHECK(result3.has_value());
    CHECK(result3.value() == "MyType");
  }

  TEST_CASE("filter_prefix_empty_input") {
    auto result = ctti::detail::NameParser::FilterPrefix("", "class");

    CHECK_FALSE(result.has_value());
    CHECK(result.error() == ctti::detail::ParseError::kEmptyInput);
  }

  TEST_CASE("filter_prefix_edge_cases") {
    auto result1 = ctti::detail::NameParser::FilterPrefix("class", "class");
    auto result2 = ctti::detail::NameParser::FilterPrefix("cl", "class");
    auto result3 = ctti::detail::NameParser::FilterPrefix("classes", "class");

    CHECK(result1.has_value());
    CHECK(result1.value() == "");

    CHECK(result2.has_value());
    CHECK(result2.value() == "cl");

    CHECK(result3.has_value());
    CHECK(result3.value() == "es");
  }

  TEST_CASE("left_pad") {
    CHECK(ctti::detail::NameParser::LeftPad("   hello") == "hello");
    CHECK(ctti::detail::NameParser::LeftPad(" world") == "world");
    CHECK(ctti::detail::NameParser::LeftPad("no_spaces") == "no_spaces");
    CHECK(ctti::detail::NameParser::LeftPad("") == "");
    CHECK(ctti::detail::NameParser::LeftPad("   ") == "");
    CHECK(ctti::detail::NameParser::LeftPad("  a  ") == "a  ");
  }

  TEST_CASE("filter_class") {
    CHECK(ctti::detail::NameParser::FilterClass("class MyClass") == "MyClass");
    CHECK(ctti::detail::NameParser::FilterClass("  class   MyClass") == "MyClass");
    CHECK(ctti::detail::NameParser::FilterClass("MyClass") == "MyClass");
    CHECK(ctti::detail::NameParser::FilterClass("struct MyStruct") == "struct MyStruct");
    CHECK(ctti::detail::NameParser::FilterClass("") == "");
  }

  TEST_CASE("filter_struct") {
    CHECK(ctti::detail::NameParser::FilterStruct("struct MyStruct") == "MyStruct");
    CHECK(ctti::detail::NameParser::FilterStruct("  struct   MyStruct") == "MyStruct");
    CHECK(ctti::detail::NameParser::FilterStruct("MyStruct") == "MyStruct");
    CHECK(ctti::detail::NameParser::FilterStruct("class MyClass") == "class MyClass");
    CHECK(ctti::detail::NameParser::FilterStruct("") == "");
  }

  TEST_CASE("filter_typename_prefix") {
    CHECK(ctti::detail::NameParser::FilterTypenamePrefix("class MyClass") == "MyClass");
    CHECK(ctti::detail::NameParser::FilterTypenamePrefix("struct MyStruct") == "MyStruct");
    CHECK(ctti::detail::NameParser::FilterTypenamePrefix("  class   MyClass") == "MyClass");
    CHECK(ctti::detail::NameParser::FilterTypenamePrefix("  struct   MyStruct") == "MyStruct");
    CHECK(ctti::detail::NameParser::FilterTypenamePrefix("MyType") == "MyType");
    CHECK(ctti::detail::NameParser::FilterTypenamePrefix("") == "");
  }

  TEST_CASE("find_ith") {
    std::string_view text = "hello::world::test::end";

    CHECK(ctti::detail::NameParser::FindIth(text, "::", 0) == 5);
    CHECK(ctti::detail::NameParser::FindIth(text, "::", 1) == 12);
    CHECK(ctti::detail::NameParser::FindIth(text, "::", 2) == 18);
    CHECK(ctti::detail::NameParser::FindIth(text, "::", 3) == std::string_view::npos);

    CHECK(ctti::detail::NameParser::FindIth("no_separator", "::", 0) == std::string_view::npos);
    CHECK(ctti::detail::NameParser::FindIth("", "::", 0) == std::string_view::npos);
    CHECK(ctti::detail::NameParser::FindIth("one::two", "::", 1) == std::string_view::npos);
  }

  TEST_CASE("find_ith_single_occurrence") {
    std::string_view text = "before::after";

    CHECK(ctti::detail::NameParser::FindIth(text, "::", 0) == 6);
    CHECK(ctti::detail::NameParser::FindIth(text, "::", 1) == std::string_view::npos);
  }

  TEST_CASE("find_ith_empty_parts") {
    std::string_view text = "::middle::";

    CHECK(ctti::detail::NameParser::FindIth(text, "::", 0) == 0);
    CHECK(ctti::detail::NameParser::FindIth(text, "::", 1) == 8);
    CHECK(ctti::detail::NameParser::FindIth(text, "::", 2) == std::string_view::npos);
  }

  TEST_CASE("filter_enum_value_basic") {
    CHECK(ctti::detail::NameParser::FilterEnumValue("(Color)1") == "Color");
    CHECK(ctti::detail::NameParser::FilterEnumValue("(MyEnum)42") == "MyEnum");
    CHECK(ctti::detail::NameParser::FilterEnumValue("(ns::EnumType)0") == "ns::EnumType");
  }

  TEST_CASE("filter_enum_value_no_parentheses") {
    CHECK(ctti::detail::NameParser::FilterEnumValue("Color::Red") == "Color::Red");
    CHECK(ctti::detail::NameParser::FilterEnumValue("Red") == "Red");
    CHECK(ctti::detail::NameParser::FilterEnumValue("42") == "42");
    CHECK(ctti::detail::NameParser::FilterEnumValue("") == "");
  }

  TEST_CASE("filter_enum_value_malformed") {
    CHECK(ctti::detail::NameParser::FilterEnumValue("(Color") == "(Color");
    CHECK(ctti::detail::NameParser::FilterEnumValue("Color)") == "Color)");
    CHECK(ctti::detail::NameParser::FilterEnumValue("()") == "");
    CHECK(ctti::detail::NameParser::FilterEnumValue(")(") == ")(");
  }

  TEST_CASE("filter_enum_value_nested_parentheses") {
    CHECK(ctti::detail::NameParser::FilterEnumValue("(Outer(Inner))1") == "Outer(Inner)");
    CHECK(ctti::detail::NameParser::FilterEnumValue("(ns::Type<T>)value") == "ns::Type<T>");
  }

  TEST_CASE("comprehensive_parsing_chain") {
    // Test a complex parsing scenario
    std::string_view input = "  struct   MyNamespace::MyStruct<int>  ";

    auto step1 = ctti::detail::NameParser::LeftPad(input);
    CHECK(step1 == "struct   MyNamespace::MyStruct<int>  ");

    auto step2 = ctti::detail::NameParser::FilterStruct(step1);
    CHECK(step2 == "MyNamespace::MyStruct<int>  ");

    // Full chain
    auto result = ctti::detail::NameParser::FilterTypenamePrefix(input);
    CHECK(result == "MyNamespace::MyStruct<int>  ");
  }

  TEST_CASE("parse_error_enum") {
    // Just verify the enum values exist and are distinct
    CHECK(ctti::detail::ParseError::kInvalidFormat != ctti::detail::ParseError::kMissingDelimiter);
    CHECK(ctti::detail::ParseError::kMissingDelimiter != ctti::detail::ParseError::kEmptyInput);
    CHECK(ctti::detail::ParseError::kInvalidFormat != ctti::detail::ParseError::kEmptyInput);
  }

  TEST_CASE("edge_cases_and_boundaries") {
    // Empty substring search
    CHECK(ctti::detail::NameParser::FindIth("hello", "", 0) == 0);
    CHECK(ctti::detail::NameParser::FindIth("hello", "", 1) == 1);
    CHECK(ctti::detail::NameParser::FindIth("hello", "", 5) == 5);
    CHECK(ctti::detail::NameParser::FindIth("hello", "", 6) == std::string_view::npos);

    // Very long separator
    CHECK(ctti::detail::NameParser::FindIth("ab", "abc", 0) == std::string_view::npos);

    // Repeated characters
    CHECK(ctti::detail::NameParser::FindIth("aaaa", "aa", 0) == 0);
    CHECK(ctti::detail::NameParser::FindIth("aaaa", "aa", 1) == 2);
    CHECK(ctti::detail::NameParser::FindIth("aaaa", "aa", 2) == std::string_view::npos);
  }
}
