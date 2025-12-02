#include "doctest.h"

#include <ctti/detail/name_parser.hpp>

#include <string_view>

TEST_SUITE("detail::name_parser") {
  TEST_CASE("filter_prefix_success") {
    auto result1 = ctti::detail::NameParser::FilterPrefix("class MyClass", "class");
    auto result2 = ctti::detail::NameParser::FilterPrefix("struct MyStruct", "struct");
    auto result3 = ctti::detail::NameParser::FilterPrefix("MyType", "nonexistent");

    CHECK(result1.has_value());
    CHECK_EQ(result1.value(), " MyClass");

    CHECK(result2.has_value());
    CHECK_EQ(result2.value(), " MyStruct");

    CHECK(result3.has_value());
    CHECK_EQ(result3.value(), "MyType");
  }

  TEST_CASE("filter_prefix_empty_input") {
    auto result = ctti::detail::NameParser::FilterPrefix("", "class");

    CHECK_FALSE(result.has_value());
    CHECK_EQ(result.error(), ctti::detail::ParseError::kEmptyInput);
  }

  TEST_CASE("filter_prefix_edge_cases") {
    auto result1 = ctti::detail::NameParser::FilterPrefix("class", "class");
    auto result2 = ctti::detail::NameParser::FilterPrefix("cl", "class");
    auto result3 = ctti::detail::NameParser::FilterPrefix("classes", "class");

    CHECK(result1.has_value());
    CHECK_EQ(result1.value(), "");

    CHECK(result2.has_value());
    CHECK_EQ(result2.value(), "cl");

    CHECK(result3.has_value());
    CHECK_EQ(result3.value(), "es");
  }

  TEST_CASE("left_pad") {
    CHECK_EQ(ctti::detail::NameParser::LeftPad("   hello"), "hello");
    CHECK_EQ(ctti::detail::NameParser::LeftPad(" world"), "world");
    CHECK_EQ(ctti::detail::NameParser::LeftPad("no_spaces"), "no_spaces");
    CHECK_EQ(ctti::detail::NameParser::LeftPad(""), "");
    CHECK_EQ(ctti::detail::NameParser::LeftPad("   "), "");
    CHECK_EQ(ctti::detail::NameParser::LeftPad("  a  "), "a  ");
  }

  TEST_CASE("filter_class") {
    CHECK_EQ(ctti::detail::NameParser::FilterClass("class MyClass"), "MyClass");
    CHECK_EQ(ctti::detail::NameParser::FilterClass("  class   MyClass"), "MyClass");
    CHECK_EQ(ctti::detail::NameParser::FilterClass("MyClass"), "MyClass");
    CHECK_EQ(ctti::detail::NameParser::FilterClass("struct MyStruct"), "struct MyStruct");
    CHECK_EQ(ctti::detail::NameParser::FilterClass(""), "");
  }

  TEST_CASE("filter_struct") {
    CHECK_EQ(ctti::detail::NameParser::FilterStruct("struct MyStruct"), "MyStruct");
    CHECK_EQ(ctti::detail::NameParser::FilterStruct("  struct   MyStruct"), "MyStruct");
    CHECK_EQ(ctti::detail::NameParser::FilterStruct("MyStruct"), "MyStruct");
    CHECK_EQ(ctti::detail::NameParser::FilterStruct("class MyClass"), "class MyClass");
    CHECK_EQ(ctti::detail::NameParser::FilterStruct(""), "");
  }

  TEST_CASE("filter_typename_prefix") {
    CHECK_EQ(ctti::detail::NameParser::FilterTypenamePrefix("class MyClass"), "MyClass");
    CHECK_EQ(ctti::detail::NameParser::FilterTypenamePrefix("struct MyStruct"), "MyStruct");
    CHECK_EQ(ctti::detail::NameParser::FilterTypenamePrefix("  class   MyClass"), "MyClass");
    CHECK_EQ(ctti::detail::NameParser::FilterTypenamePrefix("  struct   MyStruct"), "MyStruct");
    CHECK_EQ(ctti::detail::NameParser::FilterTypenamePrefix("MyType"), "MyType");
    CHECK_EQ(ctti::detail::NameParser::FilterTypenamePrefix(""), "");
  }

  TEST_CASE("find_ith") {
    std::string_view text = "hello::world::test::end";

    CHECK_EQ(ctti::detail::NameParser::FindIth(text, "::", 0), 5);
    CHECK_EQ(ctti::detail::NameParser::FindIth(text, "::", 1), 12);
    CHECK_EQ(ctti::detail::NameParser::FindIth(text, "::", 2), 18);
    CHECK_EQ(ctti::detail::NameParser::FindIth(text, "::", 3), std::string_view::npos);

    CHECK_EQ(ctti::detail::NameParser::FindIth("no_separator", "::", 0), std::string_view::npos);
    CHECK_EQ(ctti::detail::NameParser::FindIth("", "::", 0), std::string_view::npos);
    CHECK_EQ(ctti::detail::NameParser::FindIth("one::two", "::", 1), std::string_view::npos);
  }

  TEST_CASE("find_ith_single_occurrence") {
    std::string_view text = "before::after";

    CHECK_EQ(ctti::detail::NameParser::FindIth(text, "::", 0), 6);
    CHECK_EQ(ctti::detail::NameParser::FindIth(text, "::", 1), std::string_view::npos);
  }

  TEST_CASE("find_ith_empty_parts") {
    std::string_view text = "::middle::";

    CHECK_EQ(ctti::detail::NameParser::FindIth(text, "::", 0), 0);
    CHECK_EQ(ctti::detail::NameParser::FindIth(text, "::", 1), 8);
    CHECK_EQ(ctti::detail::NameParser::FindIth(text, "::", 2), std::string_view::npos);
  }

  TEST_CASE("filter_enum_value_basic") {
    CHECK_EQ(ctti::detail::NameParser::FilterEnumValue("(Color)1"), "Color");
    CHECK_EQ(ctti::detail::NameParser::FilterEnumValue("(MyEnum)42"), "MyEnum");
    CHECK_EQ(ctti::detail::NameParser::FilterEnumValue("(ns::EnumType)0"), "ns::EnumType");
  }

  TEST_CASE("filter_enum_value_no_parentheses") {
    CHECK_EQ(ctti::detail::NameParser::FilterEnumValue("Color::Red"), "Color::Red");
    CHECK_EQ(ctti::detail::NameParser::FilterEnumValue("Red"), "Red");
    CHECK_EQ(ctti::detail::NameParser::FilterEnumValue("42"), "42");
    CHECK_EQ(ctti::detail::NameParser::FilterEnumValue(""), "");
  }

  TEST_CASE("filter_enum_value_malformed") {
    CHECK_EQ(ctti::detail::NameParser::FilterEnumValue("(Color"), "(Color");
    CHECK_EQ(ctti::detail::NameParser::FilterEnumValue("Color)"), "Color)");
    CHECK_EQ(ctti::detail::NameParser::FilterEnumValue("()"), "");
    CHECK_EQ(ctti::detail::NameParser::FilterEnumValue(")("), ")(");
  }

  TEST_CASE("filter_enum_value_nested_parentheses") {
    CHECK_EQ(ctti::detail::NameParser::FilterEnumValue("(Outer(Inner))1"), "Outer(Inner)");
    CHECK_EQ(ctti::detail::NameParser::FilterEnumValue("(ns::Type<T>)value"), "ns::Type<T>");
  }

  TEST_CASE("comprehensive_parsing_chain") {
    // Test a complex parsing scenario
    std::string_view input = "  struct   MyNamespace::MyStruct<int>  ";

    auto step1 = ctti::detail::NameParser::LeftPad(input);
    CHECK_EQ(step1, "struct   MyNamespace::MyStruct<int>  ");

    auto step2 = ctti::detail::NameParser::FilterStruct(step1);
    CHECK_EQ(step2, "MyNamespace::MyStruct<int>  ");

    // Full chain
    auto result = ctti::detail::NameParser::FilterTypenamePrefix(input);
    CHECK_EQ(result, "MyNamespace::MyStruct<int>  ");
  }

  TEST_CASE("parse_error_enum") {
    // Just verify the enum values exist and are distinct
    CHECK_NE(ctti::detail::ParseError::kInvalidFormat, ctti::detail::ParseError::kMissingDelimiter);
    CHECK_NE(ctti::detail::ParseError::kMissingDelimiter, ctti::detail::ParseError::kEmptyInput);
    CHECK_NE(ctti::detail::ParseError::kInvalidFormat, ctti::detail::ParseError::kEmptyInput);
  }

  TEST_CASE("edge_cases_and_boundaries") {
    // Empty substring search
    CHECK_EQ(ctti::detail::NameParser::FindIth("hello", "", 0), 0);
    CHECK_EQ(ctti::detail::NameParser::FindIth("hello", "", 1), 1);
    CHECK_EQ(ctti::detail::NameParser::FindIth("hello", "", 5), 5);
    CHECK_EQ(ctti::detail::NameParser::FindIth("hello", "", 6), std::string_view::npos);

    // Very long separator
    CHECK_EQ(ctti::detail::NameParser::FindIth("ab", "abc", 0), std::string_view::npos);

    // Repeated characters
    CHECK_EQ(ctti::detail::NameParser::FindIth("aaaa", "aa", 0), 0);
    CHECK_EQ(ctti::detail::NameParser::FindIth("aaaa", "aa", 1), 2);
    CHECK_EQ(ctti::detail::NameParser::FindIth("aaaa", "aa", 2), std::string_view::npos);
  }
}
