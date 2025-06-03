#include "doctest.h"

#include <ctti/concepts.hpp>
#include <ctti/type_tag.hpp>

#include <iostream>
#include <string>

enum class Color { Red, Green, Blue };
enum OldStyle { OLD_A, OLD_B };

struct CustomNameType {
  static constexpr std::string_view ctti_name_of() { return "CustomName"; }
};

TEST_SUITE("concepts") {
  TEST_CASE("enum_type_concept") {
    CHECK(ctti::enum_type<Color>);
    CHECK(ctti::enum_type<OldStyle>);
    CHECK_FALSE(ctti::enum_type<int>);
    CHECK_FALSE(ctti::enum_type<std::string>);
  }

  TEST_CASE("has_custom_name_of_concept") {
    CHECK(ctti::has_custom_name_of<CustomNameType>);
    CHECK_FALSE(ctti::has_custom_name_of<int>);
    CHECK_FALSE(ctti::has_custom_name_of<std::string>);
  }

  TEST_CASE("integral_constant_type_concept") {
    CHECK(ctti::integral_constant_type<std::integral_constant<int, 42>>);
    CHECK(ctti::integral_constant_type<std::true_type>);
    CHECK(ctti::integral_constant_type<std::false_type>);
    CHECK_FALSE(ctti::integral_constant_type<int>);
    CHECK_FALSE(ctti::integral_constant_type<std::string>);
  }
}
