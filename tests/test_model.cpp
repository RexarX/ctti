#include "doctest.h"

#include <ctti/model.hpp>
#include <ctti/symbol.hpp>

#include <iostream>

struct TestStruct {
  int value = 0;
  std::string name;
};

struct IntrusiveModelStruct {
  int data = 0;
  using ctti_model = decltype([]() {
    constexpr auto value_symbol = CTTI_SYMBOL(value);
    return ctti::model<decltype(value_symbol)>{};
  }());
};

struct NoModelStruct {
  int x = 0;
  int y = 0;
};

auto ctti_model(ctti::type_tag<TestStruct>) {
  constexpr auto value_symbol = CTTI_SYMBOL(value);
  constexpr auto name_symbol = CTTI_SYMBOL(name);
  return ctti::model<decltype(value_symbol), decltype(name_symbol)>{};
}

auto ctti_model(ctti::type_tag<int>) {
  return ctti::model<>{};
}

auto ctti_model(ctti::type_tag<double>) {
  return ctti::model<>{};
}

CTTI_REGISTER_MEMBER(TestStruct, value);
CTTI_REGISTER_MEMBER(TestStruct, name);
CTTI_REGISTER_MEMBER(IntrusiveModelStruct, data);

TEST_SUITE("model") {
  TEST_CASE("basic_model") {
    constexpr auto value_symbol = CTTI_SYMBOL(value);
    constexpr auto name_symbol = CTTI_SYMBOL(name);
    using test_model = ctti::model<decltype(value_symbol), decltype(name_symbol)>;

    CHECK(test_model::size == 2);
    static_assert(test_model::size == 2);
  }

  TEST_CASE("empty_model") {
    using empty_model = ctti::model<>;

    CHECK(empty_model::size == 0);
  }

  TEST_CASE("model_of_with_adl") {
    using test_struct_model = ctti::model_of<TestStruct>;

    CHECK(test_struct_model::size == 2);
    CHECK(ctti::has_model_v<TestStruct>);
  }

  TEST_CASE("model_of_intrusive") {
    using intrusive_model = ctti::model_of<IntrusiveModelStruct>;

    CHECK(intrusive_model::size == 1);
    CHECK(ctti::has_model_v<IntrusiveModelStruct>);
  }

  TEST_CASE("no_model") {
    using no_model = ctti::model_of<NoModelStruct>;

    CHECK(no_model::size == 0);
    CHECK_FALSE(ctti::has_model_v<NoModelStruct>);
  }

  TEST_CASE("has_model_trait") {
    CHECK(ctti::has_model_v<TestStruct>);
    CHECK(ctti::has_model_v<IntrusiveModelStruct>);
    CHECK_FALSE(ctti::has_model_v<NoModelStruct>);
    CHECK_FALSE(ctti::has_model_v<int>);
  }

  TEST_CASE("default_reflect_model") {
    // Default should return empty model
    auto default_model = ctti::reflect_model(ctti::type_tag<double>{});
    CHECK(default_model.size == 0);
  }

  TEST_CASE("model_compilation") {
    // Test that models compile correctly with different symbol counts
    constexpr auto value_symbol = CTTI_SYMBOL(value);
    constexpr auto name_symbol = CTTI_SYMBOL(name);

    using model1 = ctti::model<decltype(value_symbol)>;
    using model2 = ctti::model<decltype(value_symbol), decltype(name_symbol)>;
    using model3 = ctti::model<>;

    static_assert(model1::size == 1);
    static_assert(model2::size == 2);
    static_assert(model3::size == 0);
  }
}
