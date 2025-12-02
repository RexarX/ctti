#include "doctest.h"

#include <ctti/model.hpp>
#include <ctti/symbol.hpp>

#include <string>

namespace {

struct TestStruct {
  int value = 0;
  std::string name;
};

struct IntrusiveModelStruct {
  int data = 0;

  using ctti_model = ctti::model<>;
};

struct IntrusiveModelStructWithSymbols;

constexpr auto intrusive_data_symbol = ctti::make_simple_symbol<"data", nullptr>();

struct IntrusiveModelStructWithSymbols {
  int data = 0;

  using ctti_model = ctti::model<decltype(intrusive_data_symbol)>;
};

struct NoModelStruct {
  int x = 0;
  int y = 0;
};

// ADL model for TestStruct
constexpr auto ctti_model(ctti::type_tag<TestStruct>) {
  constexpr auto value_symbol = ctti::make_simple_symbol<"value", &TestStruct::value>();
  constexpr auto name_symbol = ctti::make_simple_symbol<"name", &TestStruct::name>();
  return ctti::model<decltype(value_symbol), decltype(name_symbol)>{};
}

}  // namespace

TEST_SUITE("model") {
  TEST_CASE("basic_model") {
    constexpr auto value_symbol = ctti::make_simple_symbol<"value", &TestStruct::value>();
    constexpr auto name_symbol = ctti::make_simple_symbol<"name", &TestStruct::name>();
    using test_model = ctti::model<decltype(value_symbol), decltype(name_symbol)>;

    CHECK_EQ(test_model::size, 2);
    static_assert(test_model::size == 2);
  }

  TEST_CASE("empty_model") {
    using empty_model = ctti::model<>;

    CHECK_EQ(empty_model::size, 0);
  }

  TEST_CASE("model_of_with_adl") {
    using test_struct_model = ctti::model_of<TestStruct>;

    CHECK_EQ(test_struct_model::size, 2);
    CHECK(ctti::has_model_v<TestStruct>);
  }

  TEST_CASE("model_of_intrusive") {
    using intrusive_model = ctti::model_of<IntrusiveModelStructWithSymbols>;

    CHECK_EQ(intrusive_model::size, 1);  // Has one symbol
    CHECK(ctti::has_model_v<IntrusiveModelStructWithSymbols>);
  }

  TEST_CASE("no_model") {
    using no_model = ctti::model_of<NoModelStruct>;

    CHECK_EQ(no_model::size, 0);
    CHECK_FALSE(ctti::has_model_v<NoModelStruct>);
  }

  TEST_CASE("reflect_model") {
    auto model = ctti::reflect_model(ctti::type_tag<TestStruct>{});
    CHECK_EQ(model.size, 2);
  }
}
