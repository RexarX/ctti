#include "catch.hpp"

#include <ctti/symbol.hpp>

#include <string>

CTTI_DEFINE_SYMBOL(value);
CTTI_DEFINE_SYMBOL(getValue);
CTTI_DEFINE_SYMBOL(setValue);
CTTI_DEFINE_SYMBOL(compute);
CTTI_DEFINE_SYMBOL(nonExistent);
CTTI_DEFINE_SYMBOL(other);

// Test classes
struct SimpleGetter {
  int value = 42;
  int getValue() const { return value; }
};

struct SimpleSetter {
  int value = 0;
  void setValue(int v) { value = v; }
};

struct Combined {
  int value = 0;
  int getValue() const { return value; }
  void setValue(int v) { value = v; }
  int compute(int factor) const { return value * factor; }
};

struct WithNoMembers {
  // No matching members
};

enum class TestEnum { value, other };

TEST_CASE("advanced symbol operations") {
  SECTION("is_member_of detection") {
    REQUIRE(value::is_member_of<SimpleGetter>());
    REQUIRE(getValue::is_member_of<SimpleGetter>());
    REQUIRE_FALSE(setValue::is_member_of<SimpleGetter>());

    REQUIRE(value::is_member_of<SimpleSetter>());
    REQUIRE_FALSE(getValue::is_member_of<SimpleSetter>());
    REQUIRE(setValue::is_member_of<SimpleSetter>());

    REQUIRE(value::is_member_of<Combined>());
    REQUIRE(getValue::is_member_of<Combined>());
    REQUIRE(setValue::is_member_of<Combined>());
    REQUIRE(compute::is_member_of<Combined>());

    REQUIRE_FALSE(nonExistent::is_member_of<SimpleGetter>());
    REQUIRE_FALSE(value::is_member_of<WithNoMembers>());

    REQUIRE(value::is_member_of<TestEnum>());
    REQUIRE(other::is_member_of<TestEnum>());
  }

  SECTION("get_member() access") {
    SimpleGetter getter;
    REQUIRE(getter.*value::get_member<SimpleGetter>() == 42);

    SimpleSetter setter;
    setter.*value::get_member<SimpleSetter>() = 42;
    REQUIRE(setter.value == 42);
  }

  SECTION("get_member_value() utility") {
    SimpleGetter getter;
    REQUIRE(ctti::get_member_value<value>(getter) == 42);
    REQUIRE(ctti::get_member_value<getValue>(getter) == 42);

    Combined combined;
    combined.value = 10;
    REQUIRE(ctti::get_member_value<value>(combined) == 10);
    REQUIRE(ctti::get_member_value<getValue>(combined) == 10);
    REQUIRE(ctti::get_member_value<compute>(combined, 5) == 50);
  }

  SECTION("set_member_value() utility") {
    SimpleSetter setter;
    ctti::set_member_value<setValue>(setter, 42);
    REQUIRE(setter.value == 42);

    Combined combined;
    ctti::set_member_value<setValue>(combined, 10);
    REQUIRE(combined.value == 10);
  }

  SECTION("member_name() method") {
    REQUIRE(value::member_name<SimpleGetter>() == "value");
    REQUIRE(getValue::member_name<SimpleGetter>() == "getValue");
  }
}
