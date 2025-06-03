#include "doctest.h"

#include <ctti/detail/pretty_function.hpp>

#include <iostream>
#include <string>
#include <vector>

enum class TestEnum { A, B };

struct TestStruct {};

namespace test_ns {

class TestClass {};

}  // namespace test_ns

TEST_SUITE("detail::pretty_function") {
  TEST_CASE("type_function_basic") {
    auto int_name = ctti::detail::pretty_function::Type<int>();
    auto double_name = ctti::detail::pretty_function::Type<double>();
    auto string_name = ctti::detail::pretty_function::Type<std::string>();

    CHECK(!int_name.empty());
    CHECK(!double_name.empty());
    CHECK(!string_name.empty());

    CHECK(int_name != double_name);
    CHECK(double_name != string_name);
    CHECK(int_name != string_name);

    CHECK(int_name.find("int") != std::string_view::npos);
    CHECK(double_name.find("double") != std::string_view::npos);
  }

  TEST_CASE("type_function_custom_types") {
    auto struct_name = ctti::detail::pretty_function::Type<TestStruct>();
    auto class_name = ctti::detail::pretty_function::Type<test_ns::TestClass>();
    auto enum_name = ctti::detail::pretty_function::Type<TestEnum>();

    CHECK(!struct_name.empty());
    CHECK(!class_name.empty());
    CHECK(!enum_name.empty());

    CHECK(struct_name.find("TestStruct") != std::string_view::npos);
    CHECK(class_name.find("TestClass") != std::string_view::npos);
    CHECK(enum_name.find("TestEnum") != std::string_view::npos);
  }

  TEST_CASE("value_function_basic") {
    constexpr int test_value = 42;
    constexpr char test_char = 'X';

    auto int_value_name = ctti::detail::pretty_function::Value<int, test_value>();
    auto char_value_name = ctti::detail::pretty_function::Value<char, test_char>();

    CHECK(!int_value_name.empty());
    CHECK(!char_value_name.empty());
    CHECK(int_value_name != char_value_name);

    // Should contain the value somewhere in the string
    CHECK(int_value_name.find("42") != std::string_view::npos);
  }

  TEST_CASE("value_function_enum") {
    auto enum_value_name = ctti::detail::pretty_function::Value<TestEnum, TestEnum::A>();

    CHECK(!enum_value_name.empty());
    // Different compilers format enum values differently
    bool found_a = enum_value_name.find("A") != std::string_view::npos;
    bool found_value = enum_value_name.find("TestEnum") != std::string_view::npos;
    CHECK((found_a || found_value)); // Accept either the enum name or value name
  }

  TEST_CASE("constexpr_evaluation") {
    constexpr auto type_name = ctti::detail::pretty_function::Type<int>();
    constexpr auto value_name = ctti::detail::pretty_function::Value<int, 123>();

    static_assert(!type_name.empty());
    static_assert(!value_name.empty());

    CHECK(!type_name.empty());
    CHECK(!value_name.empty());
  }

  TEST_CASE("different_compilers_produce_output") {
    // This test mainly ensures that the pretty function macros
    // are properly defined and produce non-empty output
    auto name = ctti::detail::pretty_function::Type<double>();

    CHECK(name.size() > 0);
    CHECK(name.data() != nullptr);

    // The actual format will vary between compilers, but should contain the type
    // In some form (with or without prefixes like "class", "struct", etc.)
    bool contains_double = (name.find("double") != std::string_view::npos);
    CHECK(contains_double);
  }

  TEST_CASE("template_types") {
    auto vector_name = ctti::detail::pretty_function::Type<std::vector<int>>();
    auto string_name = ctti::detail::pretty_function::Type<std::basic_string<char>>();

    CHECK(!vector_name.empty());
    CHECK(!string_name.empty());

    CHECK(vector_name.find("vector") != std::string_view::npos);

    // Split the complex condition into separate checks
    bool has_string = string_name.find("string") != std::string_view::npos;
    bool has_basic_string = string_name.find("basic_string") != std::string_view::npos;
    CHECK((has_string || has_basic_string));
  }

  TEST_CASE("pointer_and_reference_types") {
    auto ptr_name = ctti::detail::pretty_function::Type<int*>();
    auto ref_name = ctti::detail::pretty_function::Type<int&>();
    auto const_ptr_name = ctti::detail::pretty_function::Type<const int*>();

    CHECK(!ptr_name.empty());
    CHECK(!ref_name.empty());
    CHECK(!const_ptr_name.empty());

    CHECK(ptr_name != ref_name);
    CHECK(ptr_name != const_ptr_name);
    CHECK(ref_name != const_ptr_name);
  }

  TEST_CASE("cv_qualified_types") {
    auto const_name = ctti::detail::pretty_function::Type<const int>();
    auto volatile_name = ctti::detail::pretty_function::Type<volatile int>();
    auto const_volatile_name = ctti::detail::pretty_function::Type<const volatile int>();
    auto plain_name = ctti::detail::pretty_function::Type<int>();

    CHECK(!const_name.empty());
    CHECK(!volatile_name.empty());
    CHECK(!const_volatile_name.empty());

    // All should be different (though some compilers might normalize them)
    CHECK(const_name != plain_name);
    CHECK(volatile_name != plain_name);
    CHECK(const_volatile_name != plain_name);
  }

  TEST_CASE("function_signature_contains_expected_parts") {
    auto type_sig = ctti::detail::pretty_function::Type<int>();
    auto value_sig = ctti::detail::pretty_function::Value<int, 42>();

    // Should contain function name
    CHECK(type_sig.find("Type") != std::string_view::npos);
    CHECK(value_sig.find("Value") != std::string_view::npos);

    // Should be different signatures
    CHECK(type_sig != value_sig);
  }
}
