#include "doctest.h"

#include <ctti/detail/name_filters.hpp>

#include <string_view>

TEST_SUITE("detail::name_filters") {
  TEST_CASE("filter_prefix") {
    CHECK_EQ(ctti::detail::FilterPrefix("class MyClass", "class"), "MyClass");
    CHECK_EQ(ctti::detail::FilterPrefix("struct MyStruct", "struct"), "MyStruct");
    CHECK_EQ(ctti::detail::FilterPrefix("MyClass", "class"), "MyClass");
    CHECK_EQ(ctti::detail::FilterPrefix("", "class"), "");
    CHECK_EQ(ctti::detail::FilterPrefix("MyClass", ""), "MyClass");
  }

  TEST_CASE("trim_whitespace") {
    CHECK_EQ(ctti::detail::TrimWhitespace("   hello"), "hello");
    CHECK_EQ(ctti::detail::TrimWhitespace(" world"), "world");
    CHECK_EQ(ctti::detail::TrimWhitespace("no_spaces"), "no_spaces");
    CHECK_EQ(ctti::detail::TrimWhitespace(""), "");
    CHECK_EQ(ctti::detail::TrimWhitespace("   "), "");
    CHECK_EQ(ctti::detail::TrimWhitespace("  a  "), "a");
  }

  TEST_CASE("filter_class") {
    CHECK_EQ(ctti::detail::FilterClass("class MyClass"), "MyClass");
    CHECK_EQ(ctti::detail::FilterClass("  class   MyClass"), "MyClass");
    CHECK_EQ(ctti::detail::FilterClass("MyClass"), "MyClass");
    CHECK_EQ(ctti::detail::FilterClass("struct MyStruct"), "struct MyStruct");
    CHECK_EQ(ctti::detail::FilterClass(""), "");
  }

  TEST_CASE("filter_struct") {
    CHECK_EQ(ctti::detail::FilterStruct("struct MyStruct"), "MyStruct");
    CHECK_EQ(ctti::detail::FilterStruct("  struct   MyStruct"), "MyStruct");
    CHECK_EQ(ctti::detail::FilterStruct("MyStruct"), "MyStruct");
    CHECK_EQ(ctti::detail::FilterStruct("class MyClass"), "class MyClass");
    CHECK_EQ(ctti::detail::FilterStruct(""), "");
  }

  TEST_CASE("filter_typename_prefix") {
    CHECK_EQ(ctti::detail::FilterTypenamePrefix("class MyClass"), "MyClass");
    CHECK_EQ(ctti::detail::FilterTypenamePrefix("struct MyStruct"), "MyStruct");
    CHECK_EQ(ctti::detail::FilterTypenamePrefix("  class   MyClass"), "MyClass");
    CHECK_EQ(ctti::detail::FilterTypenamePrefix("  struct   MyStruct"), "MyStruct");
    CHECK_EQ(ctti::detail::FilterTypenamePrefix("MyType"), "MyType");
    CHECK_EQ(ctti::detail::FilterTypenamePrefix(""), "");
  }

  TEST_CASE("find_ith") {
    std::string_view text = "hello::world::test::end";

    CHECK_EQ(ctti::detail::FindIth(text, "::", 0), 5);
    CHECK_EQ(ctti::detail::FindIth(text, "::", 1), 12);
    CHECK_EQ(ctti::detail::FindIth(text, "::", 2), 18);
    CHECK_EQ(ctti::detail::FindIth(text, "::", 3), std::string_view::npos);

    CHECK_EQ(ctti::detail::FindIth("no_separator", "::", 0), std::string_view::npos);
    CHECK_EQ(ctti::detail::FindIth("", "::", 0), std::string_view::npos);
    CHECK_EQ(ctti::detail::FindIth("one::two", "::", 1), std::string_view::npos);
  }

  TEST_CASE("find_ith_single_char") {
    std::string_view text = "a,b,c,d";

    CHECK_EQ(ctti::detail::FindIth(text, ",", 0), 1);
    CHECK_EQ(ctti::detail::FindIth(text, ",", 1), 3);
    CHECK_EQ(ctti::detail::FindIth(text, ",", 2), 5);
    CHECK_EQ(ctti::detail::FindIth(text, ",", 3), std::string_view::npos);
  }

  TEST_CASE("find_ith_overlapping") {
    std::string_view text = "aaaaa";

    CHECK_EQ(ctti::detail::FindIth(text, "aa", 0), 0);
    CHECK_EQ(ctti::detail::FindIth(text, "aa", 1), 2);
    CHECK_EQ(ctti::detail::FindIth(text, "aa", 2), std::string_view::npos);
  }

  TEST_CASE("filter_enum_value_basic") {
    CHECK_EQ(ctti::detail::FilterEnumValue("(Color)1"), "Color");
    CHECK_EQ(ctti::detail::FilterEnumValue("(MyEnum)42"), "MyEnum");
    CHECK_EQ(ctti::detail::FilterEnumValue("(ns::EnumType)0"), "ns::EnumType");
  }

  TEST_CASE("filter_enum_value_no_parentheses") {
    CHECK_EQ(ctti::detail::FilterEnumValue("Color::Red"), "Color::Red");
    CHECK_EQ(ctti::detail::FilterEnumValue("Red"), "Red");
    CHECK_EQ(ctti::detail::FilterEnumValue("42"), "42");
    CHECK_EQ(ctti::detail::FilterEnumValue(""), "");
  }

  TEST_CASE("filter_enum_value_malformed") {
    CHECK_EQ(ctti::detail::FilterEnumValue("(Color"), "(Color");
    CHECK_EQ(ctti::detail::FilterEnumValue("Color)"), "Color)");
    CHECK_EQ(ctti::detail::FilterEnumValue("()"), "");
    CHECK_EQ(ctti::detail::FilterEnumValue(")("), ")(");
  }

  TEST_CASE("filter_enum_value_multiple_parentheses") {
    CHECK_EQ(ctti::detail::FilterEnumValue("(Outer(Inner))1"), "Outer(Inner)");
    CHECK_EQ(ctti::detail::FilterEnumValue("(Type)value(extra)"), "Type");
  }

  TEST_CASE("complex_filtering_chain") {
    std::string_view input = "  class   MyNamespace::MyClass  ";
    auto step1 = ctti::detail::TrimWhitespace(input);
    auto step2 = ctti::detail::FilterClass(step1);

    CHECK_EQ(step1, "class   MyNamespace::MyClass");
    CHECK_EQ(step2, "MyNamespace::MyClass");

    auto result = ctti::detail::FilterTypenamePrefix(input);
    CHECK_EQ(result, "MyNamespace::MyClass");
  }

  TEST_CASE("edge_cases") {
    // Empty strings
    CHECK_EQ(ctti::detail::FilterPrefix("", "prefix"), "");
    CHECK_EQ(ctti::detail::FilterClass(""), "");
    CHECK_EQ(ctti::detail::FilterStruct(""), "");
    CHECK_EQ(ctti::detail::FilterEnumValue(""), "");

    // Single characters
    CHECK_EQ(ctti::detail::FilterPrefix("a", "a"), "");
    CHECK_EQ(ctti::detail::FindIth("a", "a", 0), 0);

    // Longer prefix than string
    CHECK_EQ(ctti::detail::FilterPrefix("ab", "abc"), "ab");
  }
}
