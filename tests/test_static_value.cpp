#include "doctest.h"

#include <ctti/static_value.hpp>

TEST_SUITE("static_value") {
  TEST_CASE("basic_functionality") {
    constexpr auto val1 = ctti::static_value<int, 42>{};
    constexpr auto val2 = ctti::static_value<char, 'A'>{};

    CHECK_EQ(val1.get(), 42);
    CHECK_EQ(val2.get(), 'A');
    CHECK_EQ(static_cast<int>(val1), 42);
    CHECK_EQ(static_cast<char>(val2), 'A');
  }

  TEST_CASE("comparison_operators") {
    constexpr auto val1 = ctti::static_value<int, 10>{};
    constexpr auto val2 = ctti::static_value<int, 20>{};
    constexpr auto val3 = ctti::static_value<int, 10>{};

    CHECK_EQ(val1, val3);
    CHECK_NE(val1, val2);
    CHECK_LT(val1, val2);
    CHECK_GT(val2, val1);
    CHECK_LE(val1, val3);
    CHECK_GE(val1, val3);
  }

  TEST_CASE("value_comparison") {
    constexpr auto val = ctti::static_value<int, 42>{};

    CHECK_EQ(val, 42);
    CHECK_EQ(42, val);
    CHECK_NE(val, 43);
    CHECK_NE(43, val);
    CHECK_LT(val, 43);
    CHECK_LT(41, val);
  }

  TEST_CASE("macro_usage") {
    constexpr int test_value = 123;
    constexpr auto val = ctti::make_static_value<test_value>();

    CHECK_EQ(val.get(), 123);
    CHECK(std::same_as<decltype(val)::value_type, int>);
  }

  TEST_CASE("different_types") {
    constexpr auto bool_val = ctti::static_value<bool, true>{};
    constexpr auto char_val = ctti::static_value<char, 'X'>{};
    constexpr auto double_val = ctti::static_value<double, 3.14>{};

    CHECK_EQ(bool_val.get(), true);
    CHECK_EQ(char_val.get(), 'X');
    CHECK_EQ(double_val.get(), doctest::Approx(3.14));
  }

  TEST_CASE("constexpr_evaluation") {
    constexpr auto val = ctti::static_value<int, 999>{};
    static_assert(val.get() == 999);
    static_assert(val == 999);

    CHECK_EQ(val.get(), 999);
  }
}
