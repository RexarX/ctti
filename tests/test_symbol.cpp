#include "doctest.h"

#include <ctti/attributes.hpp>
#include <ctti/symbol.hpp>

#include <iostream>
#include <string>

struct SymbolTestStruct {
  int value = 42;
  std::string name = "test";

  void set_value(int v) { value = v; }
  int get_value() const { return value; }
};

TEST_SUITE("symbol") {
  TEST_CASE("basic_symbol_creation") {
    constexpr auto value_symbol = ctti::make_simple_symbol<"value", &SymbolTestStruct::value>();
    constexpr auto name_symbol = ctti::make_simple_symbol<"name", &SymbolTestStruct::name>();

    CHECK(decltype(value_symbol)::name == "value");
    CHECK(decltype(name_symbol)::name == "name");
    CHECK(decltype(value_symbol)::hash != 0);
    CHECK(decltype(name_symbol)::hash != decltype(value_symbol)::hash);
  }

  TEST_CASE("symbol_ownership") {
    constexpr auto value_symbol = ctti::make_simple_symbol<"value", &SymbolTestStruct::value>();

    CHECK(decltype(value_symbol)::is_owner_of<SymbolTestStruct>());
    CHECK_FALSE(decltype(value_symbol)::is_owner_of<int>());
  }

  TEST_CASE("symbol_with_attributes") {
    using since_attr = ctti::since<42>;
    constexpr auto attributed_symbol = ctti::make_attributed_symbol<"value", &SymbolTestStruct::value, since_attr>();

    CHECK(decltype(attributed_symbol)::template has_attribute_value<42>());
    CHECK_FALSE(decltype(attributed_symbol)::template has_attribute_value<43>());
  }

  TEST_CASE("symbol_member_access") {
    SymbolTestStruct obj;
    constexpr auto value_symbol = ctti::make_simple_symbol<"value", &SymbolTestStruct::value>();

    auto member_ptr = decltype(value_symbol)::get_member<SymbolTestStruct>();
    CHECK(obj.*member_ptr == 42);

    obj.*member_ptr = 100;
    CHECK(obj.value == 100);
  }

  TEST_CASE("symbol_function_calling") {
    SymbolTestStruct obj;
    constexpr auto set_symbol = ctti::make_simple_symbol<"set_value", &SymbolTestStruct::set_value>();
    constexpr auto get_symbol = ctti::make_simple_symbol<"get_value", &SymbolTestStruct::get_value>();

    CHECK(decltype(set_symbol)::has_overload<SymbolTestStruct&, int>());
    CHECK(decltype(get_symbol)::has_overload<const SymbolTestStruct&>());

    decltype(set_symbol)::call(obj, 200);
    CHECK(obj.value == 200);

    auto result = decltype(get_symbol)::call(obj);
    CHECK(result == 200);
  }

  TEST_CASE("symbol_overloads") {
    constexpr auto multi_symbol =
        ctti::make_symbol_with_overloads<"multi", ctti::no_attributes, &SymbolTestStruct::set_value,
                                         &SymbolTestStruct::get_value>();

    CHECK(decltype(multi_symbol)::overload_count == 2);
    CHECK(decltype(multi_symbol)::has_overloads);
  }

  TEST_CASE("symbol_value_access") {
    SymbolTestStruct obj;
    constexpr auto value_symbol = ctti::make_simple_symbol<"value", &SymbolTestStruct::value>();
    constexpr auto name_symbol = ctti::make_simple_symbol<"name", &SymbolTestStruct::name>();

    auto val = ctti::get_symbol_value<value_symbol>(obj);
    auto nm = ctti::get_symbol_value<name_symbol>(obj);

    CHECK(val == 42);
    CHECK(nm == "test");

    ctti::set_symbol_value<value_symbol>(obj, 100);
    ctti::set_symbol_value<name_symbol>(obj, std::string("modified"));

    CHECK(obj.value == 100);
    CHECK(obj.name == "modified");
  }

  TEST_CASE("symbol_utilities") {
    constexpr auto value_symbol = ctti::make_simple_symbol<"value", &SymbolTestStruct::value>();

    CHECK(ctti::symbol_name<value_symbol>() == "value");
    CHECK(ctti::symbol_hash<value_symbol>() != 0);
    CHECK(ctti::overload_count<value_symbol>() >= 1);
  }

  TEST_CASE("member_traits") {
    using value_traits = ctti::member_traits<decltype(&SymbolTestStruct::value)>;
    using func_traits = ctti::member_traits<decltype(&SymbolTestStruct::get_value)>;

    CHECK(std::same_as<value_traits::value_type, int>);
    CHECK(std::same_as<value_traits::class_type, SymbolTestStruct>);
    CHECK(value_traits::kIsDataMember);
    CHECK_FALSE(value_traits::kIsFunctionMember);

    CHECK(std::same_as<func_traits::return_type, int>);
    CHECK(std::same_as<func_traits::class_type, SymbolTestStruct>);
    CHECK_FALSE(func_traits::kIsDataMember);
    CHECK(func_traits::kIsFunctionMember);
    CHECK(func_traits::kIsConstMember);
  }
}
