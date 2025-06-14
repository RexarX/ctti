#include "doctest.h"

#include <ctti/static_value.hpp>

#include <iostream>

TEST_SUITE("static_value") {
  TEST_CASE("basic_functionality") {
    constexpr auto val1 = ctti::static_value<int, 42>{};
    constexpr auto val2 = ctti::static_value<char, 'A'>{};

    CHECK(val1.get() == 42);
    CHECK(val2.get() == 'A');
    CHECK(static_cast<int>(val1) == 42);
    CHECK(static_cast<char>(val2) == 'A');
  }

  TEST_CASE("comparison_operators") {
    constexpr auto val1 = ctti::static_value<int, 10>{};
    constexpr auto val2 = ctti::static_value<int, 20>{};
    constexpr auto val3 = ctti::static_value<int, 10>{};

    CHECK(val1 == val3);
    CHECK(val1 != val2);
    CHECK(val1 < val2);
    CHECK(val2 > val1);
    CHECK(val1 <= val3);
    CHECK(val1 >= val3);
  }

  TEST_CASE("value_comparison") {
    constexpr auto val = ctti::static_value<int, 42>{};

    CHECK(val == 42);
    CHECK(42 == val);
    CHECK(val != 43);
    CHECK(43 != val);
    CHECK(val < 43);
    CHECK(41 < val);
  }

  TEST_CASE("macro_usage") {
    constexpr int test_value = 123;
    constexpr auto val = ctti::make_static_value<test_value>();

    CHECK(val.get() == 123);
    CHECK(std::same_as<decltype(val)::value_type, int>);
  }

  TEST_CASE("different_types") {
    constexpr auto bool_val = ctti::static_value<bool, true>{};
    constexpr auto char_val = ctti::static_value<char, 'X'>{};
    constexpr auto double_val = ctti::static_value<double, 3.14>{};

    CHECK(bool_val.get() == true);
    CHECK(char_val.get() == 'X');
    CHECK(double_val.get() == 3.14);
  }

  TEST_CASE("constexpr_evaluation") {
    constexpr auto val = ctti::static_value<int, 999>{};
    static_assert(val.get() == 999);
    static_assert(val == 999);

    CHECK(val.get() == 999);
  }
}
