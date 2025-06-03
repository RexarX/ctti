#include "doctest.h"

#include <ctti/name.hpp>

#include <iostream>
#include <string>
#include <vector>

enum class TestEnum { Value1, Value2 };

struct TestStruct {};

class TestClass {};

namespace test_ns {

struct NestedStruct {};

}  // namespace test_ns

TEST_SUITE("name") {
  TEST_CASE("basic_type_names") {
    CHECK(ctti::name_of<int>() == "int");
    CHECK(ctti::name_of<double>() == "double");
    CHECK(ctti::name_of<char>() == "char");
    CHECK(ctti::name_of<bool>() == "bool");
  }

  TEST_CASE("std_type_names") {
    CHECK(ctti::name_of<std::string>() == "std::string");
    CHECK(ctti::name_of<std::string_view>() == "std::string_view");
  }

  TEST_CASE("custom_type_names") {
    auto struct_name = ctti::name_of<TestStruct>();
    auto class_name = ctti::name_of<TestClass>();
    auto nested_name = ctti::name_of<test_ns::NestedStruct>();

    CHECK(struct_name.find("TestStruct") != std::string_view::npos);
    CHECK(class_name.find("TestClass") != std::string_view::npos);
    CHECK(nested_name.find("NestedStruct") != std::string_view::npos);
  }

  TEST_CASE("enum_value_names") {
    auto value1_name = ctti::name_of<TestEnum, TestEnum::Value1>();
    auto value2_name = ctti::name_of<TestEnum, TestEnum::Value2>();

    CHECK(value1_name.find("Value1") != std::string_view::npos);
    CHECK(value2_name.find("Value2") != std::string_view::npos);
  }

  TEST_CASE("qualified_names") {
    auto qualified = ctti::qualified_name_of<test_ns::NestedStruct>();
    auto full_name = qualified.get_full_name();
    auto simple_name = qualified.get_name();

    CHECK(full_name.find("test_ns") != std::string_view::npos);
    CHECK(full_name.find("NestedStruct") != std::string_view::npos);
    CHECK(simple_name.find("NestedStruct") != std::string_view::npos);
  }

  TEST_CASE("qualified_name_equality") {
    auto name1 = ctti::qualified_name_of<TestStruct>();
    auto name2 = ctti::qualified_name_of<TestStruct>();
    auto name3 = ctti::qualified_name_of<TestClass>();

    CHECK(name1 == name2);
    CHECK_FALSE(name1 == name3);
  }
}
