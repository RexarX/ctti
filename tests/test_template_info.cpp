#include "doctest.h"

#include <ctti/template_info.hpp>

#include <array>
#include <concepts>
#include <vector>

namespace {

template <typename T>
struct SimpleTemplate {
  T value;
};

template <typename T, typename U>
struct PairTemplate {
  T first;
  U second;
};

template <int N>
struct IntTemplate {
  std::array<int, N> data;
};

template <typename T, int Size>
struct MixedTemplate {
  std::array<T, Size> data;
};

}  // namespace

using SimpleInt = SimpleTemplate<int>;
using PairStringDouble = PairTemplate<std::string, double>;
using IntArray5 = IntTemplate<5>;
using MixedFloatArray3 = MixedTemplate<float, 3>;

TEST_SUITE("template_info") {
  TEST_CASE("template_instantiation_concept") {
    CHECK(ctti::template_instantiation<SimpleInt>);
    CHECK(ctti::template_instantiation<PairStringDouble>);
    CHECK(ctti::template_instantiation<std::vector<int>>);
    CHECK(ctti::template_instantiation<IntArray5>);
    CHECK(ctti::template_instantiation<MixedFloatArray3>);

    CHECK_FALSE(ctti::template_instantiation<int>);
    CHECK_FALSE(ctti::template_instantiation<std::string>);
  }

  TEST_CASE("basic_template_info") {
    auto info = ctti::get_template_info<SimpleInt>();

    CHECK(info.is_template_instantiation);
    CHECK_EQ(info.parameter_count, 1);
    CHECK(std::same_as<decltype(info)::type, SimpleInt>);

    CHECK_EQ(info.name(), "SimpleTemplate<int>");
  }

  TEST_CASE("non_template_info") {
    auto info = ctti::get_template_info<int>();

    CHECK_FALSE(info.is_template_instantiation);
    CHECK_EQ(info.parameter_count, 0);
    CHECK(std::same_as<decltype(info)::type, int>);

    CHECK_EQ(info.name(), "int");
  }

  TEST_CASE("std_template_info") {
    auto vector_info = ctti::get_template_info<std::vector<int>>();

    CHECK(vector_info.is_template_instantiation);
    CHECK_GE(vector_info.parameter_count, 1);  // Has at least the element type

    CHECK_EQ(vector_info.name(), "std::vector<int>");
  }

  TEST_CASE("utility_functions") {
    CHECK(ctti::is_template_instantiation<SimpleInt>());
    CHECK_FALSE(ctti::is_template_instantiation<int>());

    CHECK_EQ(ctti::template_parameter_count<SimpleInt>(), 1);
    CHECK_EQ(ctti::template_parameter_count<int>(), 0);
  }

  TEST_CASE("variadic_type_template_concept") {
    CHECK(ctti::variadic_type_template<SimpleInt>);
    CHECK(ctti::variadic_type_template<std::vector<int>>);
    CHECK_FALSE(ctti::variadic_type_template<IntArray5>);
    CHECK_FALSE(ctti::variadic_type_template<int>);
  }

  TEST_CASE("variadic_value_template_concept") {
    CHECK(ctti::variadic_value_template<IntArray5>);
    CHECK_FALSE(ctti::variadic_value_template<SimpleInt>);
    CHECK_FALSE(ctti::variadic_value_template<int>);
  }

  TEST_CASE("mixed_variadic_template_concept") {
    CHECK(ctti::mixed_variadic_template<MixedFloatArray3>);
    CHECK_FALSE(ctti::mixed_variadic_template<SimpleInt>);
    CHECK_FALSE(ctti::mixed_variadic_template<IntArray5>);
    CHECK_FALSE(ctti::mixed_variadic_template<int>);
  }

  TEST_CASE("template_specialization_concept") {
    CHECK(ctti::template_specialization<SimpleInt>);
    CHECK(ctti::template_specialization<std::vector<int>>);
    CHECK_FALSE(ctti::template_specialization<int>);
  }

  TEST_CASE("type_template_parameter_access") {
    auto info = ctti::get_template_info<SimpleInt>();

    static_assert(info.type_parameter_count == 1);
    CHECK_EQ(info.type_parameter_count, 1);

    if constexpr (ctti::variadic_type_template<SimpleInt>) {
      using param0 = decltype(info)::type_parameter<0>;
      CHECK(std::same_as<param0, int>);

      auto tag = info.type_parameter_tag<0>();
      CHECK(std::same_as<typename decltype(tag)::type, int>);
    }
  }

  TEST_CASE("value_template_parameter_access") {
    auto info = ctti::get_template_info<IntArray5>();

    static_assert(info.value_parameter_count == 1);
    CHECK_EQ(info.value_parameter_count, 1);

    if constexpr (ctti::variadic_value_template<IntArray5>) {
      auto value = info.value_parameter<0>();
      CHECK_EQ(value, 5);
    }
  }

  TEST_CASE("mixed_template_parameter_access") {
    auto info = ctti::get_template_info<MixedFloatArray3>();

    CHECK_EQ(info.parameter_count, 2);
    CHECK_EQ(info.type_parameter_count, 1);
    CHECK_EQ(info.value_parameter_count, 1);

    if constexpr (ctti::mixed_variadic_template<MixedFloatArray3>) {
      CHECK(std::same_as<decltype(info)::type_parameter, float>);
      CHECK_EQ(info.value_parameter, 3);
    }
  }

  TEST_CASE("for_each_parameter") {
    auto info = ctti::get_template_info<SimpleInt>();

    if constexpr (ctti::variadic_type_template<SimpleInt>) {
      int count = 0;
      info.for_each_type_parameter([&count](auto tag) { ++count; });
      CHECK_EQ(count, 1);
    }
  }

  TEST_CASE("parameter_names") {
    auto info = ctti::get_template_info<SimpleInt>();

    if constexpr (ctti::variadic_type_template<SimpleInt>) {
      auto names = info.type_parameter_names();
      CHECK_EQ(names.size(), 1);
      CHECK_EQ(names[0], "int");
    }
  }

  TEST_CASE("shared_ptr_template") {
    auto info = ctti::get_template_info<std::shared_ptr<double>>();

    CHECK(info.is_template_instantiation);
    CHECK_GE(info.parameter_count, 1);

    CHECK_EQ(info.name(), "std::shared_ptr<double>");
  }
}
