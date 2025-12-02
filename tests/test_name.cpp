#include "doctest.h"

#include <ctti/name.hpp>

#include <string>
#include <string_view>
#include <vector>

namespace {

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

}  // namespace

TEST_SUITE("name") {
  TEST_CASE("basic_type_names") {
    CHECK_EQ(ctti::name_of<int>(), "int");
    CHECK_EQ(ctti::name_of<double>(), "double");
    CHECK_EQ(ctti::name_of<char>(), "char");
    CHECK_EQ(ctti::name_of<bool>(), "bool");
    CHECK_EQ(ctti::name_of<float>(), "float");
    CHECK_EQ(ctti::name_of<void>(), "void");
    CHECK_EQ(ctti::name_of<long>(), "long");
    CHECK_EQ(ctti::name_of<long long>(), "long long");
    CHECK_EQ(ctti::name_of<short>(), "short");
    CHECK_EQ(ctti::name_of<unsigned int>(), "unsigned int");
    CHECK_EQ(ctti::name_of<unsigned long>(), "unsigned long");
    CHECK_EQ(ctti::name_of<unsigned long long>(), "unsigned long long");
    CHECK_EQ(ctti::name_of<unsigned short>(), "unsigned short");
    CHECK_EQ(ctti::name_of<signed char>(), "signed char");
    CHECK_EQ(ctti::name_of<unsigned char>(), "unsigned char");
    CHECK_EQ(ctti::name_of<long double>(), "long double");
    CHECK_EQ(ctti::name_of<wchar_t>(), "wchar_t");
    CHECK_EQ(ctti::name_of<char8_t>(), "char8_t");
    CHECK_EQ(ctti::name_of<char16_t>(), "char16_t");
    CHECK_EQ(ctti::name_of<char32_t>(), "char32_t");
  }

  TEST_CASE("std_type_names") {
    CHECK_EQ(ctti::name_of<std::string>(), "std::string");
    CHECK_EQ(ctti::name_of<std::string_view>(), "std::string_view");
    CHECK_EQ(ctti::name_of<std::nullptr_t>(), "std::nullptr_t");
  }

  TEST_CASE("custom_type_names") {
    CHECK_EQ(ctti::name_of<TestStruct>(), "TestStruct");
    CHECK_EQ(ctti::name_of<TestClass>(), "TestClass");
    CHECK_EQ(ctti::name_of<test_ns::NestedStruct>(), "test_ns::NestedStruct");
  }

  TEST_CASE("custom_name_of") {
    CHECK_EQ(ctti::name_of<CustomNameType>(), "CustomName");
  }

  TEST_CASE("enum_value_names") {
    CHECK_EQ(ctti::name_of<TestEnum, TestEnum::Value1>(), "Value1");
    CHECK_EQ(ctti::name_of<TestEnum, TestEnum::Value2>(), "Value2");
    CHECK_EQ(ctti::name_of<TestEnum, TestEnum::Value3>(), "Value3");
  }

  TEST_CASE("old_enum_value_names") {
    CHECK_EQ(ctti::name_of<OldEnum, OLD_A>(), "OLD_A");
    CHECK_EQ(ctti::name_of<OldEnum, OLD_B>(), "OLD_B");
    CHECK_EQ(ctti::name_of<OldEnum, OLD_C>(), "OLD_C");
  }

  TEST_CASE("integer_value_names") {
    CHECK_EQ(ctti::name_of<int, 0>(), "0");
    CHECK_EQ(ctti::name_of<int, 42>(), "42");
    CHECK_EQ(ctti::name_of<int, 123>(), "123");
    CHECK_EQ(ctti::name_of<int, -5>(), "-5");
    CHECK_EQ(ctti::name_of<int, -999>(), "-999");
    CHECK_EQ(ctti::name_of<int, 1000000>(), "1000000");
  }

  TEST_CASE("bool_values") {
    CHECK_EQ(ctti::name_of<bool, true>(), "true");
    CHECK_EQ(ctti::name_of<bool, false>(), "false");
  }

  TEST_CASE("char_values") {
    // Char values are represented as integers
    CHECK_EQ(ctti::name_of<char, 'A'>(), "65");
    CHECK_EQ(ctti::name_of<char, '\0'>(), "0");
    CHECK_EQ(ctti::name_of<char, 'z'>(), "122");
  }

  TEST_CASE("qualified_names") {
    auto qualified = ctti::qualified_name_of<test_ns::NestedStruct>();

    CHECK_EQ(qualified.get_full_name(), "test_ns::NestedStruct");
    CHECK_EQ(qualified.get_name(), "NestedStruct");
  }

  TEST_CASE("qualified_name_equality") {
    auto name1 = ctti::qualified_name_of<TestStruct>();
    auto name2 = ctti::qualified_name_of<TestStruct>();
    auto name3 = ctti::qualified_name_of<TestClass>();

    CHECK_EQ(name1, name2);
    CHECK_NE(name1, name3);
  }

  TEST_CASE("qualified_name_qualifiers") {
    auto qualified = ctti::qualified_name_of<test_ns::NestedStruct>();

    CHECK_EQ(qualified.get_qualifier(0), "test_ns");
    CHECK(qualified.get_qualifier(1).empty());
  }

  TEST_CASE("qualified_name_simple_type") {
    auto qualified = ctti::qualified_name_of<TestStruct>();

    CHECK_EQ(qualified.get_full_name(), "TestStruct");
    CHECK_EQ(qualified.get_name(), "TestStruct");
    CHECK(qualified.get_qualifier(0).empty());
  }

  TEST_CASE("template_type_names") {
    auto vector_name = ctti::name_of<std::vector<int>>();
    CHECK_EQ(vector_name, "std::vector<int>");
  }

  TEST_CASE("pointer_and_reference_names") {
    CHECK_EQ(ctti::name_of<int*>(), "int*");
    CHECK_EQ(ctti::name_of<int&>(), "int&");
    CHECK_EQ(ctti::name_of<const int*>(), "const int*");
    CHECK_EQ(ctti::name_of<int* const>(), "int* const");
    CHECK_EQ(ctti::name_of<const int&>(), "const int&");
    CHECK_EQ(ctti::name_of<int&&>(), "int&&");
  }

  TEST_CASE("const_volatile_names") {
    CHECK_EQ(ctti::name_of<const int>(), "const int");
    CHECK_EQ(ctti::name_of<volatile int>(), "volatile int");
    CHECK_EQ(ctti::name_of<const volatile int>(), "const volatile int");
  }

  TEST_CASE("name_of_from_object") {
    TestStruct obj;
    CHECK_EQ(ctti::name_of(obj), "TestStruct");

    int value = 42;
    CHECK_EQ(ctti::name_of(value), "int");
  }

  TEST_CASE("qualified_name_of_from_object") {
    test_ns::NestedStruct obj;
    auto qualified = ctti::qualified_name_of(obj);

    CHECK_EQ(qualified.get_full_name(), "test_ns::NestedStruct");
    CHECK_EQ(qualified.get_name(), "NestedStruct");
  }
}
