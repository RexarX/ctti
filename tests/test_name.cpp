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

struct DebugValues {
  static constexpr int values[] = {0, 1, 42, 100, 255, 1000, -1, -42};
};

TEST_SUITE("name") {
  TEST_CASE("basic_type_names") {
    auto int_name = ctti::name_of<int>();
    auto double_name = ctti::name_of<double>();
    auto char_name = ctti::name_of<char>();
    auto bool_name = ctti::name_of<bool>();

    // Should contain the type name
    CHECK(int_name.find("int") != std::string_view::npos);
    CHECK(double_name.find("double") != std::string_view::npos);
    CHECK(char_name.find("char") != std::string_view::npos);
    CHECK(bool_name.find("bool") != std::string_view::npos);

    // On most compilers, these should be exact matches
    CHECK(int_name == "int");
    CHECK(double_name == "double");
    CHECK(char_name == "char");
    CHECK(bool_name == "bool");
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

  TEST_CASE("large_integer_values") {
    auto name_1000 = ctti::name_of<int, 1000>();
    auto name_65535 = ctti::name_of<int, 65535>();

    CHECK(name_1000 == "1000");
    CHECK(name_65535 == "65535");
  }

  TEST_CASE("integer_edge_cases") {
    auto name_1 = ctti::name_of<int, 1>();
    auto name_9 = ctti::name_of<int, 9>();
    auto name_10 = ctti::name_of<int, 10>();
    auto name_99 = ctti::name_of<int, 99>();
    auto name_100 = ctti::name_of<int, 100>();
    auto name_255 = ctti::name_of<int, 255>();
    auto name_256 = ctti::name_of<int, 256>();

    CHECK(name_1 == "1");
    CHECK(name_9 == "9");
    CHECK(name_10 == "10");
    CHECK(name_99 == "99");
    CHECK(name_100 == "100");
    CHECK(name_255 == "255");
    CHECK(name_256 == "256");
  }

  TEST_CASE("negative_integer_values") {
    auto name_neg1 = ctti::name_of<int, -1>();
    auto name_neg10 = ctti::name_of<int, -10>();
    auto name_neg123 = ctti::name_of<int, -123>();
    auto name_neg1000 = ctti::name_of<int, -1000>();

    CHECK(name_neg1 == "-1");
    CHECK(name_neg10 == "-10");
    CHECK(name_neg123 == "-123");
    CHECK(name_neg1000 == "-1000");
  }

  TEST_CASE("different_integer_types") {
    auto char_value = ctti::name_of<char, 'A'>();
    auto short_value = ctti::name_of<short, 42>();
    auto long_value = ctti::name_of<long, 12345L>();
    auto uint_value = ctti::name_of<unsigned int, 999U>();

    // char might show as 'A' or as 65 depending on compiler
    CHECK(!char_value.empty());
    CHECK(short_value == "42");
    CHECK(long_value == "12345");
    CHECK(uint_value == "999");
  }

  TEST_CASE("bool_values") {
    auto bool_true = ctti::name_of<bool, true>();
    auto bool_false = ctti::name_of<bool, false>();

    // bool values typically show as "true"/"false" or "1"/"0"
    CHECK(!bool_true.empty());
    CHECK(!bool_false.empty());
    CHECK(bool_true != bool_false);
  }

  TEST_CASE("scoped_enum_values") {
    auto enum_val1 = ctti::name_of<TestEnum, TestEnum::Value1>();
    auto enum_val2 = ctti::name_of<TestEnum, TestEnum::Value2>();
    auto enum_val3 = ctti::name_of<TestEnum, TestEnum::Value3>();

    // Should contain the enum value name
    CHECK(enum_val1.find("Value1") != std::string_view::npos);
    CHECK(enum_val2.find("Value2") != std::string_view::npos);
    CHECK(enum_val3.find("Value3") != std::string_view::npos);
  }

  TEST_CASE("unscoped_enum_values") {
    auto old_a = ctti::name_of<OldEnum, OLD_A>();
    auto old_b = ctti::name_of<OldEnum, OLD_B>();
    auto old_c = ctti::name_of<OldEnum, OLD_C>();

    // Should contain the enum value name or numeric representation
    CHECK(!old_a.empty());
    CHECK(!old_b.empty());
    CHECK(!old_c.empty());
  }

  TEST_CASE("enum_with_custom_values") {
    // TestEnum::Value3 = 42, should show the name, not the number
    auto enum_42 = ctti::name_of<TestEnum, TestEnum::Value3>();

    // Compare with regular int 42
    auto int_42 = ctti::name_of<int, 42>();

    CHECK(enum_42.find("Value3") != std::string_view::npos);
    CHECK(int_42 == "42");
    CHECK(enum_42 != int_42); // Should be different
  }

  TEST_CASE("zero_values_different_types") {
    auto int_zero = ctti::name_of<int, 0>();
    auto long_zero = ctti::name_of<long, 0L>();
    auto uint_zero = ctti::name_of<unsigned int, 0U>();

    // All should represent zero but might have different representations
    CHECK(int_zero == "0");
    CHECK(long_zero == "0");
    CHECK(uint_zero == "0");
  }

  TEST_CASE("value_name_consistency") {
    // Test that the same value gives the same name consistently
    auto first_call = ctti::name_of<int, 777>();
    auto second_call = ctti::name_of<int, 777>();

    CHECK(first_call == second_call);
    CHECK(first_call == "777");
  }

  TEST_CASE("msvc_hex_conversion_verification") {
    // These values are commonly represented in hex by MSVC
    auto val_10 = ctti::name_of<int, 10>();    // 0xa
    auto val_15 = ctti::name_of<int, 15>();    // 0xf
    auto val_16 = ctti::name_of<int, 16>();    // 0x10
    auto val_42 = ctti::name_of<int, 42>();    // 0x2a
    auto val_255 = ctti::name_of<int, 255>();  // 0xff

    CHECK(val_10 == "10");
    CHECK(val_15 == "15");
    CHECK(val_16 == "16");
    CHECK(val_42 == "42");
    CHECK(val_255 == "255");
  }

  TEST_CASE("large_values") {
    // Test some larger values that would definitely be hex in MSVC
    auto val_1024 = ctti::name_of<int, 1024>();      // 0x400
    auto val_4096 = ctti::name_of<int, 4096>();      // 0x1000
    auto val_65536 = ctti::name_of<int, 65536>();    // 0x10000

    CHECK(val_1024 == "1024");
    CHECK(val_4096 == "4096");
    CHECK(val_65536 == "65536");
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

  TEST_CASE("qualified_value_names") {
    // Test qualified names for enum values
    auto qualified_enum = ctti::qualified_name_of<TestEnum, TestEnum::Value1>();
    auto full_name = qualified_enum.get_full_name();
    auto simple_name = qualified_enum.get_name();

    CHECK(!full_name.empty());
    CHECK(!simple_name.empty());
  }

  TEST_CASE("debug_output") {
    // This test case is for debugging - shows what we get on different compilers
    for (auto val : DebugValues::values) {
      switch (val) {
        case 0: MESSAGE("Value 0: ", ctti::name_of<int, 0>()); break;
        case 1: MESSAGE("Value 1: ", ctti::name_of<int, 1>()); break;
        case 42: MESSAGE("Value 42: ", ctti::name_of<int, 42>()); break;
        case 100: MESSAGE("Value 100: ", ctti::name_of<int, 100>()); break;
        case 255: MESSAGE("Value 255: ", ctti::name_of<int, 255>()); break;
        case 1000: MESSAGE("Value 1000: ", ctti::name_of<int, 1000>()); break;
        case -1: MESSAGE("Value -1: ", ctti::name_of<int, -1>()); break;
        case -42: MESSAGE("Value -42: ", ctti::name_of<int, -42>()); break;
      }
    }
  }
}
