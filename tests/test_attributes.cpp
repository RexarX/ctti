#include "doctest.h"

#include <ctti/attributes.hpp>
#include <ctti/name.hpp>

#include <iostream>
#include <string>

CTTI_NAMED_ATTRIBUTE(custom_attr);

struct TestStruct {
  int value = 0;
};

CTTI_DEFINE_ANNOTATED_SYMBOL(annotated_value, ctti::read_only, ctti::since<1>, ctti::description, custom_attr);

TEST_SUITE("attributes") {
  TEST_CASE("basic_attribute") {
    constexpr auto attr = ctti::attribute<42>{};

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

    CHECK(std::same_as<read_only::type, ctti::ReadOnlyTag>);
    CHECK(std::same_as<write_only::type, ctti::WriteOnlyTag>);
    CHECK(std::same_as<deprecated::type, ctti::DeprecatedTag>);
    CHECK(std::same_as<internal::type, ctti::InternalTag>);
  }

  TEST_CASE("since_attribute") {
    using since_v1 = ctti::since<1>;
    using since_v2 = ctti::since<2>;

    CHECK(since_v1::value == 1);
    CHECK(since_v2::value == 2);
  }

  TEST_CASE("named_attributes") {
    CHECK(ctti::description::name == "Description");
    CHECK(custom_attr::name == "custom_attr");
  }

  TEST_CASE("attribute_list") {
    using attr_list = ctti::attribute_list<ctti::read_only, ctti::since<1>, ctti::description>;

    CHECK(attr_list::size == 3);
    CHECK(attr_list::has<ctti::read_only>());
    CHECK(attr_list::has_value<1>());
    CHECK(attr_list::has_tag<ctti::ReadOnlyTag>());
    CHECK(attr_list::has_named<ctti::description>());

    CHECK_FALSE(attr_list::has<ctti::write_only>());
    CHECK_FALSE(attr_list::has_value<2>());
  }

  TEST_CASE("attribute_type_concept") {
    CHECK(ctti::attribute_type<ctti::attribute<42>>);
    CHECK(ctti::attribute_type<ctti::read_only>);
    CHECK(ctti::attribute_type<ctti::description>);
    CHECK_FALSE(ctti::attribute_type<int>);
  }

  TEST_CASE("annotated_symbol") {
    CHECK(ctti_symbols::annotated_value::symbol == "annotated_value");
    CHECK(ctti_symbols::annotated_value::has_attribute<ctti::read_only>());
    CHECK(ctti_symbols::annotated_value::has_attribute_value<1>());
    CHECK_FALSE(ctti_symbols::annotated_value::has_attribute<ctti::write_only>());
  }
}
