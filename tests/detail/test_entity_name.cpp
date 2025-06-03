#include "doctest.h"

#include <ctti/detail/entity_name.hpp>

#include <iostream>

TEST_SUITE("detail::entity_name") {
  TEST_CASE("basic_entity_name") {
    ctti::detail::EntityName name("simple");

    CHECK(name.GetStr() == "simple");
    CHECK(name[0] == "simple");
    CHECK(name[1] == "");
  }

  TEST_CASE("qualified_name_single_level") {
    ctti::detail::EntityName name("namespace::class");

    CHECK(name.GetStr() == "namespace::class");
    CHECK(name[0] == "namespace");
    CHECK(name[1] == "class");
    CHECK(name[2] == "");
  }

  TEST_CASE("qualified_name_multiple_levels") {
    ctti::detail::EntityName name("level1::level2::level3::final");

    CHECK(name.GetStr() == "level1::level2::level3::final");
    CHECK(name[0] == "level1");
    CHECK(name[1] == "level2");
    CHECK(name[2] == "level3");
    CHECK(name[3] == "final");
    CHECK(name[4] == "");
  }

  TEST_CASE("empty_entity_name") {
    ctti::detail::EntityName name("");

    CHECK(name.GetStr() == "");
    CHECK(name[0] == "");
    CHECK(name[1] == "");
  }

  TEST_CASE("name_with_leading_separator") {
    ctti::detail::EntityName name("::global::name");

    CHECK(name.GetStr() == "::global::name");
    CHECK(name[0] == "");  // Empty part before first ::
    CHECK(name[1] == "global");
    CHECK(name[2] == "name");
    CHECK(name[3] == "");
  }

  TEST_CASE("name_with_trailing_separator") {
    ctti::detail::EntityName name("namespace::class::");

    CHECK(name.GetStr() == "namespace::class::");
    CHECK(name[0] == "namespace");
    CHECK(name[1] == "class");
    CHECK(name[2] == "");  // Empty part after last ::
    CHECK(name[3] == "");
  }

  TEST_CASE("name_with_empty_parts") {
    ctti::detail::EntityName name("first::::last");

    CHECK(name.GetStr() == "first::::last");
    CHECK(name[0] == "first");
    CHECK(name[1] == "");  // Empty part between first pair of ::
    CHECK(name[2] == "last");
    CHECK(name[3] == "");
  }

  TEST_CASE("name_only_separators") {
    ctti::detail::EntityName name("::");

    CHECK(name.GetStr() == "::");
    CHECK(name[0] == "");
    CHECK(name[1] == "");
    CHECK(name[2] == "");
  }

  TEST_CASE("name_multiple_separators") {
    ctti::detail::EntityName name("::::");

    CHECK(name.GetStr() == "::::");
    CHECK(name[0] == "");
    CHECK(name[1] == "");
    CHECK(name[2] == "");
    CHECK(name[3] == "");
  }

  TEST_CASE("real_world_cpp_names") {
    ctti::detail::EntityName std_name("std::vector");
    CHECK(std_name[0] == "std");
    CHECK(std_name[1] == "vector");
    CHECK(std_name[2] == "");

    ctti::detail::EntityName nested_name("outer::inner::class::member");
    CHECK(nested_name[0] == "outer");
    CHECK(nested_name[1] == "inner");
    CHECK(nested_name[2] == "class");
    CHECK(nested_name[3] == "member");
    CHECK(nested_name[4] == "");

    ctti::detail::EntityName template_name("std::basic_string<char>");
    CHECK(template_name[0] == "std");
    CHECK(template_name[1] == "basic_string<char>");
    CHECK(template_name[2] == "");
  }

  TEST_CASE("single_colon_not_separator") {
    ctti::detail::EntityName name("not:separator");

    CHECK(name.GetStr() == "not:separator");
    CHECK(name[0] == "not:separator");
    CHECK(name[1] == "");
  }

  TEST_CASE("mixed_single_and_double_colons") {
    ctti::detail::EntityName name("a:b::c:d");

    CHECK(name.GetStr() == "a:b::c:d");
    CHECK(name[0] == "a:b");
    CHECK(name[1] == "c:d");
    CHECK(name[2] == "");
  }

  TEST_CASE("operator_bracket_constexpr") {
    constexpr ctti::detail::EntityName name("const::expr::test");

    static_assert(name[0] == "const");
    static_assert(name[1] == "expr");
    static_assert(name[2] == "test");
    static_assert(name[3] == "");

    CHECK(name[0] == "const");
    CHECK(name[1] == "expr");
    CHECK(name[2] == "test");
  }

  TEST_CASE("large_index_returns_empty") {
    ctti::detail::EntityName name("a::b::c");

    CHECK(name[100] == "");
    CHECK(name[1000] == "");
  }

  TEST_CASE("boundary_conditions") {
    // Test with string that's exactly "::"
    ctti::detail::EntityName just_sep("::");
    CHECK(just_sep[0] == "");
    CHECK(just_sep[1] == "");

    // Test with very long qualified name
    ctti::detail::EntityName long_name("a::b::c::d::e::f::g::h::i::j::k::l::m::n::o::p");
    CHECK(long_name[0] == "a");
    CHECK(long_name[15] == "p");
    CHECK(long_name[16] == "");

    // Test with string containing :: at the very end
    ctti::detail::EntityName end_sep("name::");
    CHECK(end_sep[0] == "name");
    CHECK(end_sep[1] == "");
  }

  TEST_CASE("get_str_consistency") {
    std::string_view original = "test::name::here";
    ctti::detail::EntityName name(original);

    CHECK(name.GetStr() == original);
    CHECK(name.GetStr().data() == original.data());
    CHECK(name.GetStr().size() == original.size());
  }

  TEST_CASE("constexpr_construction") {
    constexpr ctti::detail::EntityName name("constexpr::test");
    constexpr auto str = name.GetStr();

    static_assert(str == "constexpr::test");
    CHECK(str == "constexpr::test");
  }

  TEST_CASE("unicode_and_special_chars") {
    // Test that EntityName handles special characters properly
    ctti::detail::EntityName special("namespace::class<int>::member");
    CHECK(special[0] == "namespace");
    CHECK(special[1] == "class<int>");
    CHECK(special[2] == "member");

    ctti::detail::EntityName with_numbers("ns1::class2::member3");
    CHECK(with_numbers[0] == "ns1");
    CHECK(with_numbers[1] == "class2");
    CHECK(with_numbers[2] == "member3");
  }
}
