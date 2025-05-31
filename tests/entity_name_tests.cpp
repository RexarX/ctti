#include "catch.hpp"

#include <ctti/detail/entity_name.hpp>

TEST_CASE("entity_name") {
  SECTION("basic functionality") {
    ctti::detail::entity_name simple_name("simple");
    REQUIRE(simple_name.str() == "simple");

    ctti::detail::entity_name qualified_name("namespace::Class::member");
    REQUIRE(qualified_name.str() == "namespace::Class::member");
    REQUIRE(qualified_name[0] == "namespace");
    REQUIRE(qualified_name[1] == "Class");
    REQUIRE(qualified_name[2] == "member");
  }

  SECTION("complex qualifiers") {
    ctti::detail::entity_name complex_name("std::vector<int>::iterator");
    REQUIRE(complex_name.str() == "std::vector<int>::iterator");
    REQUIRE(complex_name[0] == "std");
    REQUIRE(complex_name[1] == "vector<int>");
    REQUIRE(complex_name[2] == "iterator");

    ctti::detail::entity_name empty_name("");
    REQUIRE(empty_name.str() == "");
    REQUIRE(empty_name[0] == "");

    ctti::detail::entity_name single_qualifier("single");
    REQUIRE(single_qualifier.str() == "single");
    REQUIRE(single_qualifier[0] == "single");
    REQUIRE(single_qualifier[1] == "");
  }

  SECTION("boundary cases") {
    ctti::detail::entity_name trailing_colons("namespace::");
    REQUIRE(trailing_colons[0] == "namespace");
    REQUIRE(trailing_colons[1] == "");

    ctti::detail::entity_name leading_colons("::namespace::Class");
    REQUIRE(leading_colons.str() == "::namespace::Class");
    // Different implementations may handle leading :: differently
  }
}
