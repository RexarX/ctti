#include "doctest.h"

#include <ctti/symbol.hpp>
#include <ctti/tie.hpp>

#include <iostream>
#include <string>

struct TieTestStruct {
  int value = 42;
  std::string name = "test";
  double price = 99.99;
};

CTTI_DEFINE_SYMBOL(value);
CTTI_DEFINE_SYMBOL(name);
CTTI_DEFINE_SYMBOL(price);

TEST_SUITE("tie") {
  TEST_CASE("basic_tie") {
    TieTestStruct obj;

    int val = 0;
    std::string nm;
    double pr = 0.0;

    auto tied = ctti::tie<ctti_symbols::value, ctti_symbols::name, ctti_symbols::price>(val, nm, pr);
    tied = obj;

    CHECK(val == 42);
    CHECK(nm == "test");
    CHECK(pr == 99.99);
  }

  TEST_CASE("partial_tie") {
    TieTestStruct obj;

    int val = 0;
    std::string nm;

    auto tied = ctti::tie<ctti_symbols::value, ctti_symbols::name>(val, nm);
    tied = obj;

    CHECK(val == 42);
    CHECK(nm == "test");
  }

  TEST_CASE("single_member_tie") {
    TieTestStruct obj;

    int val = 0;
    auto tied = ctti::tie<ctti_symbols::value>(val);
    tied = obj;

    CHECK(val == 42);
  }

  TEST_CASE("tie_with_modified_object") {
    TieTestStruct obj;
    obj.value = 100;
    obj.name = "modified";
    obj.price = 123.45;

    int val = 0;
    std::string nm;
    double pr = 0.0;

    auto tied = ctti::tie<ctti_symbols::value, ctti_symbols::name, ctti_symbols::price>(val, nm, pr);
    tied = obj;

    CHECK(val == 100);
    CHECK(nm == "modified");
    CHECK(pr == 123.45);
  }

  TEST_CASE("tie_type_conversion") {
    struct SourceStruct {
      double value = 3.14;
    };

    SourceStruct src;
    int dst_value = 0;

    auto tied = ctti::tie<ctti_symbols::value>(dst_value);
    tied = src;

    CHECK(dst_value == 3);  // Converted from double to int
  }

  TEST_CASE("tie_with_non_matching_symbols") {
    struct DifferentStruct {
      int other_member = 123;
    };

    DifferentStruct obj;
    int val = 999;

    auto tied = ctti::tie<ctti_symbols::value>(val);
    tied = obj;  // Should not modify val since 'value' is not a member of DifferentStruct

    CHECK(val == 999);  // Should remain unchanged
  }

  TEST_CASE("tie_assignment_operator") {
    TieTestStruct obj1;
    TieTestStruct obj2;
    obj2.value = 200;
    obj2.name = "second";
    obj2.price = 200.50;

    int val = 0;
    std::string nm;
    double pr = 0.0;

    auto tied = ctti::tie<ctti_symbols::value, ctti_symbols::name, ctti_symbols::price>(val, nm, pr);

    // First assignment
    tied = obj1;
    CHECK(val == 42);
    CHECK(nm == "test");
    CHECK(pr == 99.99);

    // Second assignment
    tied = obj2;
    CHECK(val == 200);
    CHECK(nm == "second");
    CHECK(pr == 200.50);
  }

  TEST_CASE("tie_with_const_object") {
    const TieTestStruct obj;

    int val = 0;
    std::string nm;
    double pr = 0.0;

    auto tied = ctti::tie<ctti_symbols::value, ctti_symbols::name, ctti_symbols::price>(val, nm, pr);
    tied = obj;

    CHECK(val == 42);
    CHECK(nm == "test");
    CHECK(pr == 99.99);
  }

  TEST_CASE("empty_tie") {
    TieTestStruct obj;

    auto tied = ctti::tie<>();
    tied = obj;  // Should compile and do nothing

    // No CHECK needed, just verify it compiles
  }

  TEST_CASE("debug_tie") {
    TieTestStruct obj;

    // Test individual symbol detection
    CHECK(ctti_symbols::value::is_owner_of<TieTestStruct>());
    CHECK(ctti_symbols::name::is_owner_of<TieTestStruct>());
    CHECK(ctti_symbols::price::is_owner_of<TieTestStruct>());

    // Test member access
    auto val_ptr = ctti_symbols::value::get_member<TieTestStruct>();
    auto name_ptr = ctti_symbols::name::get_member<TieTestStruct>();
    auto price_ptr = ctti_symbols::price::get_member<TieTestStruct>();

    CHECK(obj.*val_ptr == 42);
    CHECK(obj.*name_ptr == "test");
    CHECK(obj.*price_ptr == 99.99);

    // Test direct assignment
    double pr = 0.0;
    pr = obj.*price_ptr;
    CHECK(pr == 99.99);
  }
}
