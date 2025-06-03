#include "doctest.h"

#include <ctti/type_tag.hpp>

#include <iostream>

TEST_SUITE("type_tag") {
  TEST_CASE("basic_functionality") {
    ctti::type_tag<int> int_tag;
    ctti::type_tag<std::string> string_tag;

    CHECK(std::same_as<ctti::type_tag<int>::type, int>);
    CHECK(std::same_as<ctti::type_tag<std::string>::type, std::string>);
  }

  TEST_CASE("constexpr_construction") {
    constexpr ctti::type_tag<double> double_tag;
    static_assert(std::same_as<decltype(double_tag)::type, double>);
  }

  TEST_CASE("void_type") {
    ctti::type_tag<void> void_tag;
    CHECK(std::same_as<decltype(void_tag)::type, void>);
  }
}
