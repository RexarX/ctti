#include "doctest.h"

#include <ctti/symbol.hpp>
#include <ctti/tie.hpp>

#include <string>

namespace {

struct TieTestStruct {
  int value = 42;
  std::string name = "test";
  double price = 99.99;
};

}  // namespace

TEST_SUITE("tie") {
  TEST_CASE("basic_tie") {
    TieTestStruct obj;

    int val = 0;
    std::string nm;
    double pr = 0.0;

    constexpr auto value_symbol = ctti::make_simple_symbol<"value", &TieTestStruct::value>();
    constexpr auto name_symbol = ctti::make_simple_symbol<"name", &TieTestStruct::name>();
    constexpr auto price_symbol = ctti::make_simple_symbol<"price", &TieTestStruct::price>();

    auto tied = ctti::tie<value_symbol, name_symbol, price_symbol>(val, nm, pr);
    tied = obj;

    CHECK_EQ(val, 42);
    CHECK_EQ(nm, "test");
    CHECK_EQ(pr, doctest::Approx(99.99));
  }

  TEST_CASE("partial_tie") {
    TieTestStruct obj;

    int val = 0;
    std::string nm;

    constexpr auto value_symbol = ctti::make_simple_symbol<"value", &TieTestStruct::value>();
    constexpr auto name_symbol = ctti::make_simple_symbol<"name", &TieTestStruct::name>();

    auto tied = ctti::tie<value_symbol, name_symbol>(val, nm);
    tied = obj;

    CHECK_EQ(val, 42);
    CHECK_EQ(nm, "test");
  }

  TEST_CASE("single_member_tie") {
    TieTestStruct obj;

    int val = 0;
    constexpr auto value_symbol = ctti::make_simple_symbol<"value", &TieTestStruct::value>();
    auto tied = ctti::tie<value_symbol>(val);
    tied = obj;

    CHECK_EQ(val, 42);
  }

  TEST_CASE("tie_with_modified_object") {
    TieTestStruct obj;
    obj.value = 100;
    obj.name = "modified";
    obj.price = 123.45;

    int val = 0;
    std::string nm;
    double pr = 0.0;

    constexpr auto value_symbol = ctti::make_simple_symbol<"value", &TieTestStruct::value>();
    constexpr auto name_symbol = ctti::make_simple_symbol<"name", &TieTestStruct::name>();
    constexpr auto price_symbol = ctti::make_simple_symbol<"price", &TieTestStruct::price>();

    auto tied = ctti::tie<value_symbol, name_symbol, price_symbol>(val, nm, pr);
    tied = obj;

    CHECK_EQ(val, 100);
    CHECK_EQ(nm, "modified");
    CHECK_EQ(pr, doctest::Approx(123.45));
  }
}
