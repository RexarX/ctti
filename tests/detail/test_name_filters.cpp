#include "doctest.h"

#include <ctti/detail/name_filters.hpp>

#include <iostream>

TEST_SUITE("detail::name_filters") {
  TEST_CASE("filter_prefix") {
    CHECK(ctti::detail::FilterPrefix("class MyClass", "class") == "MyClass");
    CHECK(ctti::detail::FilterPrefix("struct MyStruct", "struct") == "MyStruct");
    CHECK(ctti::detail::FilterPrefix("MyClass", "class") == "MyClass");
    CHECK(ctti::detail::FilterPrefix("", "class") == "");
    CHECK(ctti::detail::FilterPrefix("MyClass", "") == "MyClass");
  }

  TEST_CASE("trim_whitespace") {
    CHECK(ctti::detail::TrimWhitespace("   hello") == "hello");
    CHECK(ctti::detail::TrimWhitespace(" world") == "world");
    CHECK(ctti::detail::TrimWhitespace("no_spaces") == "no_spaces");
    CHECK(ctti::detail::TrimWhitespace("") == "");
    CHECK(ctti::detail::TrimWhitespace("   ") == "");
    CHECK(ctti::detail::TrimWhitespace("  a  ") == "a");
  }

  TEST_CASE("filter_class") {
    CHECK(ctti::detail::FilterClass("class MyClass") == "MyClass");
    CHECK(ctti::detail::FilterClass("  class   MyClass") == "MyClass");
    CHECK(ctti::detail::FilterClass("MyClass") == "MyClass");
    CHECK(ctti::detail::FilterClass("struct MyStruct") == "struct MyStruct");
    CHECK(ctti::detail::FilterClass("") == "");
  }

  TEST_CASE("filter_struct") {
    CHECK(ctti::detail::FilterStruct("struct MyStruct") == "MyStruct");
    CHECK(ctti::detail::FilterStruct("  struct   MyStruct") == "MyStruct");
    CHECK(ctti::detail::FilterStruct("MyStruct") == "MyStruct");
    CHECK(ctti::detail::FilterStruct("class MyClass") == "class MyClass");
    CHECK(ctti::detail::FilterStruct("") == "");
  }

  TEST_CASE("filter_typename_prefix") {
    CHECK(ctti::detail::FilterTypenamePrefix("class MyClass") == "MyClass");
    CHECK(ctti::detail::FilterTypenamePrefix("struct MyStruct") == "MyStruct");
    CHECK(ctti::detail::FilterTypenamePrefix("  class   MyClass") == "MyClass");
    CHECK(ctti::detail::FilterTypenamePrefix("  struct   MyStruct") == "MyStruct");
    CHECK(ctti::detail::FilterTypenamePrefix("MyType") == "MyType");
    CHECK(ctti::detail::FilterTypenamePrefix("") == "");
  }

  TEST_CASE("find_ith") {
    std::string_view text = "hello::world::test::end";

    CHECK(ctti::detail::FindIth(text, "::", 0) == 5);
    CHECK(ctti::detail::FindIth(text, "::", 1) == 12);
    CHECK(ctti::detail::FindIth(text, "::", 2) == 18);
    CHECK(ctti::detail::FindIth(text, "::", 3) == std::string_view::npos);

    CHECK(ctti::detail::FindIth("no_separator", "::", 0) == std::string_view::npos);
    CHECK(ctti::detail::FindIth("", "::", 0) == std::string_view::npos);
    CHECK(ctti::detail::FindIth("one::two", "::", 1) == std::string_view::npos);
  }

  TEST_CASE("find_ith_single_char") {
    std::string_view text = "a,b,c,d";

    CHECK(ctti::detail::FindIth(text, ",", 0) == 1);
    CHECK(ctti::detail::FindIth(text, ",", 1) == 3);
    CHECK(ctti::detail::FindIth(text, ",", 2) == 5);
    CHECK(ctti::detail::FindIth(text, ",", 3) == std::string_view::npos);
  }

  TEST_CASE("find_ith_overlapping") {
    std::string_view text = "aaaaa";

    CHECK(ctti::detail::FindIth(text, "aa", 0) == 0);
    CHECK(ctti::detail::FindIth(text, "aa", 1) == 2);
    CHECK(ctti::detail::FindIth(text, "aa", 2) == std::string_view::npos);
  }

  TEST_CASE("filter_enum_value_basic") {
    CHECK(ctti::detail::FilterEnumValue("(Color)1") == "Color");
    CHECK(ctti::detail::FilterEnumValue("(MyEnum)42") == "MyEnum");
    CHECK(ctti::detail::FilterEnumValue("(ns::EnumType)0") == "ns::EnumType");
  }

  TEST_CASE("filter_enum_value_no_parentheses") {
    CHECK(ctti::detail::FilterEnumValue("Color::Red") == "Color::Red");
    CHECK(ctti::detail::FilterEnumValue("Red") == "Red");
    CHECK(ctti::detail::FilterEnumValue("42") == "42");
    CHECK(ctti::detail::FilterEnumValue("") == "");
  }

  TEST_CASE("filter_enum_value_malformed") {
    CHECK(ctti::detail::FilterEnumValue("(Color") == "(Color");
    CHECK(ctti::detail::FilterEnumValue("Color)") == "Color)");
    CHECK(ctti::detail::FilterEnumValue("()") == "");
    CHECK(ctti::detail::FilterEnumValue(")(") == ")(");
  }

  TEST_CASE("filter_enum_value_multiple_parentheses") {
    CHECK(ctti::detail::FilterEnumValue("(Outer(Inner))1") == "Outer(Inner)");
    CHECK(ctti::detail::FilterEnumValue("(Type)value(extra)") == "Type");
  }

  TEST_CASE("complex_filtering_chain") {
    std::string_view input = "  class   MyNamespace::MyClass  ";
    auto step1 = ctti::detail::TrimWhitespace(input);
    auto step2 = ctti::detail::FilterClass(step1);

    CHECK(step1 == "class   MyNamespace::MyClass");
    CHECK(step2 == "MyNamespace::MyClass");

    auto result = ctti::detail::FilterTypenamePrefix(input);
    CHECK(result == "MyNamespace::MyClass");
  }

  TEST_CASE("edge_cases") {
    // Empty strings
    CHECK(ctti::detail::FilterPrefix("", "prefix") == "");
    CHECK(ctti::detail::FilterClass("") == "");
    CHECK(ctti::detail::FilterStruct("") == "");
    CHECK(ctti::detail::FilterEnumValue("") == "");

    // Single characters
    CHECK(ctti::detail::FilterPrefix("a", "a") == "");
    CHECK(ctti::detail::FindIth("a", "a", 0) == 0);

    // Longer prefix than string
    CHECK(ctti::detail::FilterPrefix("ab", "abc") == "ab");
  }
}
