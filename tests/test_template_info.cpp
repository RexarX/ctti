#include "doctest.h"

#include <ctti/template_info.hpp>

#include <array>
#include <iostream>
#include <map>
#include <vector>

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
    CHECK(info.parameter_count == 1);
    CHECK(std::same_as<decltype(info)::type, SimpleInt>);
    CHECK(std::same_as<decltype(info)::parameter<0>, int>);

    auto name = info.name();
    CHECK(name.find("SimpleTemplate") != std::string_view::npos);
  }

  TEST_CASE("pair_template_info") {
    auto info = ctti::get_template_info<PairStringDouble>();

    CHECK(info.is_template_instantiation);
    CHECK(info.parameter_count == 2);
    CHECK(std::same_as<decltype(info)::parameter<0>, std::string>);
    CHECK(std::same_as<decltype(info)::parameter<1>, double>);

    auto tag0 = info.type_parameter_tag<0>();
    auto tag1 = info.type_parameter_tag<1>();
    CHECK(std::same_as<typename decltype(tag0)::type, std::string>);
    CHECK(std::same_as<typename decltype(tag1)::type, double>);
  }

  TEST_CASE("non_template_info") {
    auto info = ctti::get_template_info<int>();

    CHECK_FALSE(info.is_template_instantiation);
    CHECK(info.parameter_count == 0);
    CHECK(std::same_as<decltype(info)::type, int>);

    auto name = info.name();
    CHECK(name == "int");
  }

  TEST_CASE("std_template_info") {
    auto vector_info = ctti::get_template_info<std::vector<int>>();

    CHECK(vector_info.is_template_instantiation);
    CHECK(vector_info.parameter_count >= 1);  // May have allocator parameter

    auto name = vector_info.name();
    CHECK(name.find("vector") != std::string_view::npos);
  }

  TEST_CASE("parameter_names") {
    auto info = ctti::get_template_info<PairStringDouble>();
    auto names = info.parameter_names();

    CHECK(names.size() == 2);
    CHECK(names[0].find("string") != std::string_view::npos);
    CHECK(names[1] == "double");
  }

  TEST_CASE("for_each_parameter") {
    auto info = ctti::get_template_info<PairStringDouble>();

    int count = 0;
    info.for_each_parameter([&count](auto tag) { ++count; });

    CHECK(count == 2);
  }

  TEST_CASE("utility_functions") {
    CHECK(ctti::is_template_instantiation<SimpleInt>());
    CHECK_FALSE(ctti::is_template_instantiation<int>());

    CHECK(ctti::template_parameter_count<PairStringDouble>() == 2);
    CHECK(ctti::template_parameter_count<int>() == 0);

    using param0 = ctti::template_parameter_t<PairStringDouble, 0>;
    using param1 = ctti::template_parameter_t<PairStringDouble, 1>;
    CHECK(std::same_as<param0, std::string>);
    CHECK(std::same_as<param1, double>);
  }

  TEST_CASE("int_template_info") {
    auto info = ctti::get_template_info<IntArray5>();

    CHECK(info.is_template_instantiation);
    CHECK(info.parameter_count == 1);

    auto value = info.value_parameter<0>();
    CHECK(value == 5);
  }

  TEST_CASE("mixed_template_info") {
    auto info = ctti::get_template_info<MixedFloatArray3>();

    CHECK(info.is_template_instantiation);
    CHECK(info.parameter_count == 2);

    CHECK(std::same_as<decltype(info)::type_parameter, float>);
    CHECK(info.value_parameter == 3);

    auto param0 = info.parameter<0>();
    auto param1 = info.parameter<1>();

    CHECK(std::same_as<typename decltype(param0)::type, float>);
    CHECK(param1 == 3);
  }

  TEST_CASE("is_variadic_instantiation_of") {
    CHECK(ctti::is_variadic_instantiation_of<SimpleTemplate, SimpleInt>());
    CHECK_FALSE(ctti::is_variadic_instantiation_of<PairTemplate, SimpleInt>());
    CHECK_FALSE(ctti::is_variadic_instantiation_of<SimpleTemplate, int>());

    static_assert(ctti::variadic_instantiation_of<SimpleTemplate, SimpleInt>);
    static_assert(!ctti::variadic_instantiation_of<PairTemplate, SimpleInt>);
  }

  TEST_CASE("template_specialization_concept") {
    CHECK(ctti::template_specialization<SimpleInt>);
    CHECK_FALSE(ctti::template_specialization<int>);
  }
}
