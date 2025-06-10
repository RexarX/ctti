#include "doctest.h"

#include <ctti/name.hpp>

#include <iostream>
#include <string>
#include <vector>

enum class TestEnum { Value1, Value2, Value3 = 42 };
enum OldEnum { OLD_A = 10, OLD_B = 20, OLD_C = 100 };

struct TestStruct {};

class TestClass {};

namespace test_ns {
struct NestedStruct {};
}  // namespace test_ns

struct CustomNameType {
  static constexpr std::string_view ctti_name_of() { return "CustomName"; }
};

TEST_SUITE("name") {
  TEST_CASE("basic_type_names") {
    auto int_name = ctti::name_of<int>();
    auto double_name = ctti::name_of<double>();
    auto char_name = ctti::name_of<char>();
    auto bool_name = ctti::name_of<bool>();

    CHECK(int_name == "int");
    CHECK(double_name == "double");
    CHECK(char_name == "char");
    CHECK(bool_name == "bool");
  }

  TEST_CASE("std_type_names") {
    CHECK(ctti::name_of<std::string>() == "std::string");
    CHECK(ctti::name_of<std::string_view>() == "std::string_view");
    CHECK(ctti::name_of<std::nullptr_t>() == "std::nullptr_t");
  }

  TEST_CASE("custom_type_names") {
    auto struct_name = ctti::name_of<TestStruct>();
    auto class_name = ctti::name_of<TestClass>();
    auto nested_name = ctti::name_of<test_ns::NestedStruct>();

    CHECK(struct_name.find("TestStruct") != std::string_view::npos);
    CHECK(class_name.find("TestClass") != std::string_view::npos);
    CHECK(nested_name.find("NestedStruct") != std::string_view::npos);
  }

  TEST_CASE("custom_name_of") {
    auto custom_name = ctti::name_of<CustomNameType>();
    CHECK(custom_name == "CustomName");
  }

  TEST_CASE("enum_value_names") {
    auto value1_name = ctti::name_of<TestEnum, TestEnum::Value1>();
    auto value2_name = ctti::name_of<TestEnum, TestEnum::Value2>();

    CHECK(value1_name.find("Value1") != std::string_view::npos);
    CHECK(value2_name.find("Value2") != std::string_view::npos);
  }

  TEST_CASE("integer_value_names") {
    auto name_0 = ctti::name_of<int, 0>();
    auto name_42 = ctti::name_of<int, 42>();
    auto name_123 = ctti::name_of<int, 123>();
    auto name_neg = ctti::name_of<int, -5>();

    CHECK(name_0 == "0");
    CHECK(name_42 == "42");
    CHECK(name_123 == "123");
    CHECK(name_neg == "-5");
  }

  TEST_CASE("bool_values") {
    auto bool_true = ctti::name_of<bool, true>();
    auto bool_false = ctti::name_of<bool, false>();

    CHECK(bool_true == "true");
    CHECK(bool_false == "false");
  }

  TEST_CASE("char_values") {
    auto char_a = ctti::name_of<char, 'A'>();
    auto char_zero = ctti::name_of<char, '\0'>();

    // char values are typically shown as integers
    CHECK(!char_a.empty());
    CHECK(!char_zero.empty());
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

  TEST_CASE("qualified_name_qualifiers") {
    auto qualified = ctti::qualified_name_of<test_ns::NestedStruct>();

    auto first_qualifier = qualified.get_qualifier(0);
    auto second_qualifier = qualified.get_qualifier(1);

    CHECK(first_qualifier == "test_ns");
    CHECK(second_qualifier.empty());  // No second level
  }

  TEST_CASE("template_type_names") {
    auto vector_name = ctti::name_of<std::vector<int>>();
    CHECK(vector_name.find("vector") != std::string_view::npos);
    CHECK(vector_name.find("int") != std::string_view::npos);
  }

  TEST_CASE("pointer_and_reference_names") {
    auto ptr_name = ctti::name_of<int*>();
    auto ref_name = ctti::name_of<int&>();
    auto const_ptr_name = ctti::name_of<const int*>();

    CHECK(!ptr_name.empty());
    CHECK(!ref_name.empty());
    CHECK(!const_ptr_name.empty());
    CHECK(ptr_name != ref_name);
  }
}
