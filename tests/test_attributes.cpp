#include "doctest.h"

#include <ctti/attributes.hpp>

#include <iostream>
#include <string>

TEST_SUITE("attributes") {
  TEST_CASE("attribute_value") {
    constexpr auto attr = ctti::attribute_value<42>{};

    CHECK(attr.get() == 42);
    CHECK(static_cast<int>(attr) == 42);
    CHECK(attr == 42);
    CHECK(42 == attr);
  }

  TEST_CASE("tag_attributes") {
    using read_only = ctti::read_only;
    using write_only = ctti::write_only;
    using deprecated = ctti::deprecated;
    using internal = ctti::internal;
    using validated = ctti::validated;

    CHECK(std::same_as<read_only::tag_type, ctti::read_only_tag>);
    CHECK(std::same_as<write_only::tag_type, ctti::write_only_tag>);
    CHECK(std::same_as<deprecated::tag_type, ctti::deprecated_tag>);
    CHECK(std::same_as<internal::tag_type, ctti::internal_tag>);
    CHECK(std::same_as<validated::tag_type, ctti::validated_tag>);
  }

  TEST_CASE("since_attribute") {
    using since_v1 = ctti::since<1>;
    using since_v2 = ctti::since<2>;

    CHECK(since_v1::value == 1);
    CHECK(since_v2::value == 2);
  }

  TEST_CASE("named_attributes") {
    CHECK(ctti::description::name == "description");
  }

  TEST_CASE("description_with_value") {
    using desc_with_val = ctti::description_with_value<"custom description">;
    CHECK(desc_with_val::name == "custom description");
  }

  TEST_CASE("attribute_list") {
    using attr_list = ctti::attribute_list<ctti::read_only, ctti::since<1>, ctti::description>;

    CHECK(attr_list::size == 3);
    CHECK(attr_list::has<ctti::read_only>());
    CHECK(attr_list::has_value<1>());
    CHECK(attr_list::has_tag<ctti::read_only_tag>());
    CHECK(attr_list::has_named<ctti::description>());

    CHECK_FALSE(attr_list::has<ctti::write_only>());
    CHECK_FALSE(attr_list::has_value<2>());
  }

  TEST_CASE("attribute_list_for_each") {
    using attr_list = ctti::attribute_list<ctti::read_only, ctti::since<42>>;

    int count = 0;
    attr_list::for_each([&count](auto identity) { ++count; });
    CHECK(count == 2);
  }
}
