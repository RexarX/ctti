#include "doctest.h"

#include <ctti/concepts.hpp>
#include <ctti/model.hpp>
#include <ctti/symbol.hpp>
#include <ctti/type_tag.hpp>

#include <string>
#include <string_view>
#include <type_traits>

namespace {

struct CustomNameType {
  static constexpr std::string_view ctti_name_of() { return "CustomName"; }
};

struct ReflectableType {
  using ctti_model = ctti::model<>;
};

struct ReflectableTypeWithSymbols {
  int dummy = 0;
};

// ADL model for ReflectableTypeWithSymbols with actual symbols
constexpr auto ctti_model(ctti::type_tag<ReflectableTypeWithSymbols>) {
  constexpr auto dummy_symbol = ctti::make_simple_symbol<"dummy", &ReflectableTypeWithSymbols::dummy>();
  return ctti::model<decltype(dummy_symbol)>{};
}

}  // namespace

TEST_SUITE("concepts") {
  TEST_CASE("has_custom_name_of_concept") {
    CHECK(ctti::has_custom_name_of<CustomNameType>);
    CHECK_FALSE(ctti::has_custom_name_of<int>);
    CHECK_FALSE(ctti::has_custom_name_of<std::string>);
  }

  TEST_CASE("reflectable_type_concept") {
    CHECK(ctti::reflectable_type<ReflectableTypeWithSymbols>);
    CHECK_FALSE(ctti::reflectable_type<ReflectableType>);  // Empty model
    CHECK_FALSE(ctti::reflectable_type<int>);
    CHECK_FALSE(ctti::reflectable_type<std::string>);
  }

  TEST_CASE("integral_constant_type_concept") {
    CHECK(ctti::integral_constant_type<std::integral_constant<int, 42>>);
    CHECK(ctti::integral_constant_type<std::true_type>);
    CHECK(ctti::integral_constant_type<std::false_type>);
    CHECK_FALSE(ctti::integral_constant_type<int>);
    CHECK_FALSE(ctti::integral_constant_type<std::string>);
  }
}
