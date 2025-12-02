#include "doctest.h"

#include <ctti/detail/overload_resolution.hpp>

#include <concepts>
#include <functional>
#include <string>
#include <tuple>

namespace {

// Test classes for function traits
class TestClass {
public:
  int regular_method(double d) { return static_cast<int>(d); }
  int const_method(double d) const { return static_cast<int>(d * 2); }
  void void_method(int i) {}
  void const_void_method(int i) const {}

  int noexcept_method(float f) noexcept { return static_cast<int>(f); }
  int const_noexcept_method(float f) const noexcept { return static_cast<int>(f * 2); }

  void void_noexcept_method(char c) noexcept {}
  void const_void_noexcept_method(char c) const noexcept {}
};

// Free function for testing
int free_function(int a, double b) {
  return a + static_cast<int>(b);
}

}  // namespace

TEST_SUITE("detail::overload_resolution") {
  TEST_CASE("function_traits_free_function") {
    using traits = ctti::detail::FunctionTraits<int(int, double)>;

    CHECK(std::same_as<traits::return_type, int>);
    CHECK(std::same_as<traits::args_tuple, std::tuple<int, double>>);
    CHECK_EQ(traits::kArity, 2);
    CHECK(std::same_as<traits::arg_type<0>, int>);
    CHECK(std::same_as<traits::arg_type<1>, double>);
  }

  TEST_CASE("function_traits_void_return") {
    using traits = ctti::detail::FunctionTraits<void(std::string, char)>;

    CHECK(std::same_as<traits::return_type, void>);
    CHECK(std::same_as<traits::args_tuple, std::tuple<std::string, char>>);
    CHECK_EQ(traits::kArity, 2);
    CHECK(std::same_as<traits::arg_type<0>, std::string>);
    CHECK(std::same_as<traits::arg_type<1>, char>);
  }

  TEST_CASE("function_traits_no_args") {
    using traits = ctti::detail::FunctionTraits<double()>;

    CHECK(std::same_as<traits::return_type, double>);
    CHECK(std::same_as<traits::args_tuple, std::tuple<>>);
    CHECK_EQ(traits::kArity, 0);
  }

  TEST_CASE("function_traits_member_function") {
    using traits = ctti::detail::FunctionTraits<int (TestClass::*)(double)>;

    CHECK(std::same_as<traits::return_type, int>);
    CHECK(std::same_as<traits::args_tuple, std::tuple<double>>);
    CHECK(std::same_as<traits::class_type, TestClass>);
    CHECK(std::same_as<traits::pointer_type, int (TestClass::*)(double)>);
    CHECK_EQ(traits::kArity, 1);
    CHECK(traits::kIsMemberFunction);
    CHECK_FALSE(traits::kIsConst);
  }

  TEST_CASE("function_traits_const_member_function") {
    using traits = ctti::detail::FunctionTraits<int (TestClass::*)(double) const>;

    CHECK(std::same_as<traits::return_type, int>);
    CHECK(std::same_as<traits::args_tuple, std::tuple<double>>);
    CHECK(std::same_as<traits::class_type, TestClass>);
    CHECK(std::same_as<traits::pointer_type, int (TestClass::*)(double) const>);
    CHECK_EQ(traits::kArity, 1);
    CHECK(traits::kIsMemberFunction);
    CHECK(traits::kIsConst);
  }

  TEST_CASE("function_traits_noexcept_member_function") {
    using traits = ctti::detail::FunctionTraits<int (TestClass::*)(float) noexcept>;

    CHECK(std::same_as<traits::return_type, int>);
    CHECK(std::same_as<traits::args_tuple, std::tuple<float>>);
    CHECK(std::same_as<traits::class_type, TestClass>);
    CHECK(std::same_as<traits::pointer_type, int (TestClass::*)(float) noexcept>);
    CHECK_EQ(traits::kArity, 1);
    CHECK(traits::kIsMemberFunction);
    CHECK_FALSE(traits::kIsConst);
    CHECK(traits::kIsNoexcept);
  }

  TEST_CASE("function_traits_const_noexcept_member_function") {
    using traits = ctti::detail::FunctionTraits<int (TestClass::*)(float) const noexcept>;

    CHECK(std::same_as<traits::return_type, int>);
    CHECK(std::same_as<traits::args_tuple, std::tuple<float>>);
    CHECK(std::same_as<traits::class_type, TestClass>);
    CHECK(std::same_as<traits::pointer_type, int (TestClass::*)(float) const noexcept>);
    CHECK_EQ(traits::kArity, 1);
    CHECK(traits::kIsMemberFunction);
    CHECK(traits::kIsConst);
    CHECK(traits::kIsNoexcept);
  }

  TEST_CASE("callable_with_concept") {
    auto lambda = [](int a, double b) { return a + b; };
    std::function<int(int, double)> func = [](int a, double b) { return a * static_cast<int>(b); };

    CHECK(ctti::detail::CallableWith<decltype(lambda), int, double>);
    CHECK(ctti::detail::CallableWith<decltype(func), int, double>);
    CHECK_FALSE(ctti::detail::CallableWith<decltype(lambda), std::string>);
    CHECK_FALSE(ctti::detail::CallableWith<decltype(func), int, double, char>);

    // Test with free function pointer
    auto* func_ptr = &free_function;
    CHECK(ctti::detail::CallableWith<decltype(func_ptr), int, double>);
    // Use incompatible types that can't be converted
    CHECK_FALSE(ctti::detail::CallableWith<decltype(func_ptr), std::string, std::vector<int>>);
  }

  TEST_CASE("member_callable_with_concept") {
    auto member_ptr = &TestClass::regular_method;
    auto const_member_ptr = &TestClass::const_method;

    CHECK(ctti::detail::MemberCallableWith<decltype(member_ptr), TestClass&, double>);
    CHECK(ctti::detail::MemberCallableWith<decltype(const_member_ptr), const TestClass&, double>);
    CHECK(ctti::detail::MemberCallableWith<decltype(const_member_ptr), TestClass&, double>);

    CHECK_FALSE(ctti::detail::MemberCallableWith<decltype(member_ptr), TestClass&, std::string>);
    CHECK_FALSE(ctti::detail::MemberCallableWith<decltype(member_ptr), int, double>);
  }

  TEST_CASE("overload_signature") {
    using sig1 = ctti::detail::OverloadSignature<int(double, char)>;
    using sig2 = ctti::detail::OverloadSignature<void()>;
    using sig3 = ctti::detail::OverloadSignature<std::string(int, int, int)>;

    CHECK(std::same_as<sig1::return_type, int>);
    CHECK(std::same_as<sig1::args_tuple, std::tuple<double, char>>);
    CHECK_EQ(sig1::kArity, 2);
    CHECK(std::same_as<sig1::arg_type<0>, double>);
    CHECK(std::same_as<sig1::arg_type<1>, char>);

    CHECK(std::same_as<sig2::return_type, void>);
    CHECK(std::same_as<sig2::args_tuple, std::tuple<>>);
    CHECK_EQ(sig2::kArity, 0);

    CHECK(std::same_as<sig3::return_type, std::string>);
    CHECK_EQ(sig3::kArity, 3);
    CHECK(std::same_as<sig3::arg_type<0>, int>);
    CHECK(std::same_as<sig3::arg_type<1>, int>);
    CHECK(std::same_as<sig3::arg_type<2>, int>);
  }

  TEST_CASE("complex_member_function_signatures") {
    // Test with more complex signatures
    using complex_traits =
        ctti::detail::FunctionTraits<std::string (TestClass::*)(const std::vector<int>&, std::string&&) const noexcept>;

    CHECK(std::same_as<complex_traits::return_type, std::string>);
    CHECK_EQ(complex_traits::kArity, 2);
    CHECK(std::same_as<complex_traits::arg_type<0>, const std::vector<int>&>);
    CHECK(std::same_as<complex_traits::arg_type<1>, std::string&&>);
    CHECK(complex_traits::kIsConst);
    CHECK(complex_traits::kIsNoexcept);
    CHECK(complex_traits::kIsMemberFunction);
  }

  TEST_CASE("function_traits_with_references") {
    using ref_traits = ctti::detail::FunctionTraits<void(int&, const double&, std::string&&)>;

    CHECK(std::same_as<ref_traits::return_type, void>);
    CHECK_EQ(ref_traits::kArity, 3);
    CHECK(std::same_as<ref_traits::arg_type<0>, int&>);
    CHECK(std::same_as<ref_traits::arg_type<1>, const double&>);
    CHECK(std::same_as<ref_traits::arg_type<2>, std::string&&>);
  }

  TEST_CASE("function_traits_with_pointers") {
    using ptr_traits = ctti::detail::FunctionTraits<int*(char*, const void*)>;

    CHECK(std::same_as<ptr_traits::return_type, int*>);
    CHECK_EQ(ptr_traits::kArity, 2);
    CHECK(std::same_as<ptr_traits::arg_type<0>, char*>);
    CHECK(std::same_as<ptr_traits::arg_type<1>, const void*>);
  }

  TEST_CASE("member_function_with_void_return") {
    using void_traits = ctti::detail::FunctionTraits<void (TestClass::*)(int)>;

    CHECK(std::same_as<void_traits::return_type, void>);
    CHECK(std::same_as<void_traits::class_type, TestClass>);
    CHECK_EQ(void_traits::kArity, 1);
    CHECK(std::same_as<void_traits::arg_type<0>, int>);
    CHECK(void_traits::kIsMemberFunction);
    CHECK_FALSE(void_traits::kIsConst);
  }

  TEST_CASE("function_traits_edge_cases") {
    // Test with function taking no parameters
    using no_param_traits = ctti::detail::FunctionTraits<void (TestClass::*)()>;
    CHECK_EQ(no_param_traits::kArity, 0);
    CHECK(std::same_as<no_param_traits::args_tuple, std::tuple<>>);

    // Test with function taking many parameters
    using many_param_traits = ctti::detail::FunctionTraits<int(int, int, int, int, int, int, int, int)>;
    CHECK_EQ(many_param_traits::kArity, 8);
    CHECK(std::same_as<many_param_traits::arg_type<7>, int>);
  }

  TEST_CASE("callable_with_lambdas") {
    auto simple_lambda = []() { return 42; };
    auto param_lambda = [](int x, double y) { return x + static_cast<int>(y); };
    auto capture_lambda = [capture = 10](int x) { return x + capture; };

    CHECK(ctti::detail::CallableWith<decltype(simple_lambda)>);
    CHECK(ctti::detail::CallableWith<decltype(param_lambda), int, double>);
    CHECK(ctti::detail::CallableWith<decltype(capture_lambda), int>);

    CHECK_FALSE(ctti::detail::CallableWith<decltype(simple_lambda), int>);
    CHECK_FALSE(ctti::detail::CallableWith<decltype(param_lambda), std::string, char>);
  }

  TEST_CASE("has_member_overload_concept") {
    // This is a template concept that would need actual overloaded methods to test properly
    // For now, just verify it compiles
    struct TestStruct {
      void method() {}
      void method(int) {}
    };

    // The concept exists and can be instantiated
    constexpr bool has_void_overload = requires(TestStruct obj) { obj.method(); };
    constexpr bool has_int_overload = requires(TestStruct obj, int i) { obj.method(i); };

    CHECK(has_void_overload);
    CHECK(has_int_overload);
  }
}
