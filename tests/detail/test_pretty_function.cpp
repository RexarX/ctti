#include "doctest.h"

#include <ctti/detail/pretty_function.hpp>

#include <string>
#include <string_view>
#include <vector>

namespace {

enum class TestEnum { A, B };

struct TestStruct {};

namespace test_ns {

class TestClass {};

}  // namespace test_ns

}  // namespace

TEST_SUITE("detail::pretty_function") {
  TEST_CASE("type_function_basic") {
    auto int_name = ctti::detail::pretty_function::Type<int>();
    auto double_name = ctti::detail::pretty_function::Type<double>();
    auto string_name = ctti::detail::pretty_function::Type<std::string>();

    CHECK_FALSE(int_name.empty());
    CHECK_FALSE(double_name.empty());
    CHECK_FALSE(string_name.empty());

    CHECK_NE(int_name, double_name);
    CHECK_NE(double_name, string_name);
    CHECK_NE(int_name, string_name);

    CHECK_NE(int_name.find("int"), std::string_view::npos);
    CHECK_NE(double_name.find("double"), std::string_view::npos);
  }

  TEST_CASE("type_function_custom_types") {
    auto struct_name = ctti::detail::pretty_function::Type<TestStruct>();
    auto class_name = ctti::detail::pretty_function::Type<test_ns::TestClass>();
    auto enum_name = ctti::detail::pretty_function::Type<TestEnum>();

    CHECK_FALSE(struct_name.empty());
    CHECK_FALSE(class_name.empty());
    CHECK_FALSE(enum_name.empty());

    CHECK_NE(struct_name.find("TestStruct"), std::string_view::npos);
    CHECK_NE(class_name.find("TestClass"), std::string_view::npos);
    CHECK_NE(enum_name.find("TestEnum"), std::string_view::npos);
  }

  TEST_CASE("value_function_basic") {
    constexpr int test_value = 42;
    constexpr char test_char = 'X';

    auto int_value_name = ctti::detail::pretty_function::Value<int, test_value>();
    auto char_value_name = ctti::detail::pretty_function::Value<char, test_char>();

    CHECK_FALSE(int_value_name.empty());
    CHECK_FALSE(char_value_name.empty());
    CHECK_NE(int_value_name, char_value_name);

    // Should contain the value somewhere in the string
    bool found_value =
        int_value_name.find("42") != std::string_view::npos || int_value_name.find("0x2a") != std::string_view::npos;
    bool found_name = int_value_name.find("test_value") != std::string_view::npos;
    bool found_int = int_value_name.find("int") != std::string_view::npos;

    // At least one of these should be true
    CHECK((found_value && (found_name || found_int)));
  }

  TEST_CASE("value_function_enum") {
    auto enum_value_name = ctti::detail::pretty_function::Value<TestEnum, TestEnum::A>();

    CHECK_FALSE(enum_value_name.empty());
    // Different compilers format enum values differently
    bool found_a = enum_value_name.find("A") != std::string_view::npos;
    bool found_value = enum_value_name.find("TestEnum") != std::string_view::npos;
    bool found_zero = enum_value_name.find("0") != std::string_view::npos;  // TestEnum::A is 0
    CHECK((found_a || found_value || found_zero));                          // Accept any reasonable representation
  }

  TEST_CASE("constexpr_evaluation") {
    constexpr auto type_name = ctti::detail::pretty_function::Type<int>();
    constexpr auto value_name = ctti::detail::pretty_function::Value<int, 123>();

    static_assert(!type_name.empty());
    static_assert(!value_name.empty());

    CHECK_FALSE(type_name.empty());
    CHECK_FALSE(value_name.empty());
  }

  TEST_CASE("different_compilers_produce_output") {
    // This test mainly ensures that the pretty function macros are properly defined and produce non-empty output
    auto name = ctti::detail::pretty_function::Type<double>();

    CHECK_GT(name.size(), 0);
    CHECK_NE(name.data(), nullptr);

    // The actual format will vary between compilers, but should contain the type
    // In some form (with or without prefixes like "class", "struct", etc.)
    bool contains_double = (name.find("double") != std::string_view::npos);
    CHECK(contains_double);
  }

  TEST_CASE("template_types") {
    auto vector_name = ctti::detail::pretty_function::Type<std::vector<int>>();
    auto string_name = ctti::detail::pretty_function::Type<std::basic_string<char>>();

    CHECK_FALSE(vector_name.empty());
    CHECK_FALSE(string_name.empty());

    CHECK_NE(vector_name.find("vector"), std::string_view::npos);

    // Split the complex condition into separate checks
    bool has_string = string_name.find("string") != std::string_view::npos;
    bool has_basic_string = string_name.find("basic_string") != std::string_view::npos;
    CHECK((has_string || has_basic_string));
  }

  TEST_CASE("pointer_and_reference_types") {
    auto ptr_name = ctti::detail::pretty_function::Type<int*>();
    auto ref_name = ctti::detail::pretty_function::Type<int&>();
    auto const_ptr_name = ctti::detail::pretty_function::Type<const int*>();

    CHECK_FALSE(ptr_name.empty());
    CHECK_FALSE(ref_name.empty());
    CHECK_FALSE(const_ptr_name.empty());

    CHECK_NE(ptr_name, ref_name);
    CHECK_NE(ptr_name, const_ptr_name);
    CHECK_NE(ref_name, const_ptr_name);
  }

  TEST_CASE("cv_qualified_types") {
    auto const_name = ctti::detail::pretty_function::Type<const int>();
    auto volatile_name = ctti::detail::pretty_function::Type<volatile int>();
    auto const_volatile_name = ctti::detail::pretty_function::Type<const volatile int>();
    auto plain_name = ctti::detail::pretty_function::Type<int>();

    CHECK_FALSE(const_name.empty());
    CHECK_FALSE(volatile_name.empty());
    CHECK_FALSE(const_volatile_name.empty());

    // All should be different (though some compilers might normalize them)
    CHECK_NE(const_name, plain_name);
    CHECK_NE(volatile_name, plain_name);
    CHECK_NE(const_volatile_name, plain_name);
  }

  TEST_CASE("function_signature_contains_expected_parts") {
    auto type_sig = ctti::detail::pretty_function::Type<int>();
    auto value_sig = ctti::detail::pretty_function::Value<int, 42>();

    // Should contain function name
    CHECK_NE(type_sig.find("Type"), std::string_view::npos);
    CHECK_NE(value_sig.find("Value"), std::string_view::npos);

    // Should be different signatures
    CHECK_NE(type_sig, value_sig);
  }
}
