#include "doctest.h"

#include <ctti/attributes.hpp>
#include <ctti/symbol.hpp>

#include <iostream>
#include <string>
#include <vector>

struct SymbolTestStruct {
  int value = 42;
  std::string name = "test";

  void set_value(int v) { value = v; }
  int get_value() const { return value; }
};

struct OverloadTestStruct {
  int value = 0;
  double result = 0.0;
  std::vector<std::string> logs;

  void process(int x) {
    value = x;
    logs.push_back("process(int)");
  }

  void process(double d) {
    result = d;
    logs.push_back("process(double)");
  }

  void process(const std::string& s) { logs.push_back("process(string): " + s); }

  int calculate() {
    logs.push_back("calculate()");
    return value;
  }

  int calculate(int x) {
    logs.push_back("calculate(int)");
    return value + x;
  }

  double calculate(double x, double y) {
    logs.push_back("calculate(double, double)");
    return x * y;
  }

  std::string get_status() {
    logs.push_back("get_status()");
    return "active";
  }

  std::string get_status() const { return "readonly"; }
};

TEST_SUITE("symbol") {
  TEST_CASE("basic_symbol_creation") {
    constexpr auto value_symbol = ctti::make_simple_symbol<"value", &SymbolTestStruct::value>();
    constexpr auto name_symbol = ctti::make_simple_symbol<"name", &SymbolTestStruct::name>();

    CHECK(value_symbol.name == "value");
    CHECK(name_symbol.name == "name");
    CHECK(value_symbol.hash != 0);
    CHECK(name_symbol.hash != value_symbol.hash);
  }

  TEST_CASE("symbol_ownership") {
    constexpr auto value_symbol = ctti::make_simple_symbol<"value", &SymbolTestStruct::value>();

    CHECK(value_symbol.is_owner_of<SymbolTestStruct>());
    CHECK_FALSE(value_symbol.is_owner_of<int>());
  }

  TEST_CASE("symbol_with_attributes") {
    using since_attr = ctti::since<42>;
    constexpr auto attributed_symbol = ctti::make_attributed_symbol<"value", &SymbolTestStruct::value, since_attr>();

    CHECK(attributed_symbol.has_attribute_value<42>());
    CHECK_FALSE(attributed_symbol.has_attribute_value<43>());
  }

  TEST_CASE("symbol_member_access") {
    SymbolTestStruct obj;
    constexpr auto value_symbol = ctti::make_simple_symbol<"value", &SymbolTestStruct::value>();

    auto member_ptr = value_symbol.get_member<SymbolTestStruct>();
    CHECK(obj.*member_ptr == 42);

    obj.*member_ptr = 100;
    CHECK(obj.value == 100);
  }

  TEST_CASE("symbol_function_calling") {
    SymbolTestStruct obj;
    constexpr auto set_symbol = ctti::make_simple_symbol<"set_value", &SymbolTestStruct::set_value>();
    constexpr auto get_symbol = ctti::make_simple_symbol<"get_value", &SymbolTestStruct::get_value>();

    CHECK(set_symbol.has_overload<void(int)>());
    CHECK(get_symbol.has_overload<int() const>());

    set_symbol.call(obj, 200);
    CHECK(obj.value == 200);

    auto result = get_symbol.call(obj);
    CHECK(result == 200);
  }

  TEST_CASE("symbol_overloads") {
    constexpr auto multi_symbol =
        ctti::make_symbol_with_overloads<"multi", ctti::no_attributes, &SymbolTestStruct::set_value,
                                         &SymbolTestStruct::get_value>();

    CHECK(multi_symbol.overload_count == 2);
    CHECK(multi_symbol.has_overloads);
  }

  TEST_CASE("symbol_value_access") {
    SymbolTestStruct obj;
    constexpr auto value_symbol = ctti::make_simple_symbol<"value", &SymbolTestStruct::value>();
    constexpr auto name_symbol = ctti::make_simple_symbol<"name", &SymbolTestStruct::name>();

    auto val = value_symbol.get_value(obj);
    auto nm = name_symbol.get_value(obj);

    CHECK(val == 42);
    CHECK(nm == "test");

    value_symbol.set_value(obj, 100);
    name_symbol.set_value(obj, std::string("modified"));

    CHECK(obj.value == 100);
    CHECK(obj.name == "modified");
  }

  TEST_CASE("overloaded_method_with_different_parameter_types") {
    OverloadTestStruct obj;

    constexpr auto process_symbol = ctti::make_symbol_with_overloads<
        "process", ctti::no_attributes, static_cast<void (OverloadTestStruct::*)(int)>(&OverloadTestStruct::process),
        static_cast<void (OverloadTestStruct::*)(double)>(&OverloadTestStruct::process),
        static_cast<void (OverloadTestStruct::*)(const std::string&)>(&OverloadTestStruct::process)>();

    CHECK(process_symbol.overload_count == 3);
    CHECK(process_symbol.has_overloads);

    CHECK(process_symbol.has_overload<void(int)>());
    process_symbol.call(obj, 42);
    CHECK(obj.value == 42);
    CHECK(obj.logs.back() == "process(int)");

    CHECK(process_symbol.has_overload<void(double)>());
    process_symbol.call(obj, 3.14);
    CHECK(obj.result == 3.14);
    CHECK(obj.logs.back() == "process(double)");

    CHECK(process_symbol.has_overload<void(const std::string&)>());
    process_symbol.call(obj, std::string("test"));
    CHECK(obj.logs.back() == "process(string): test");

    CHECK_FALSE(process_symbol.has_overload<void(bool)>());
  }

  TEST_CASE("overloaded_method_with_different_parameter_counts") {
    OverloadTestStruct obj;
    obj.value = 10;

    constexpr auto calculate_symbol = ctti::make_symbol_with_overloads<
        "calculate", ctti::no_attributes, static_cast<int (OverloadTestStruct::*)()>(&OverloadTestStruct::calculate),
        static_cast<int (OverloadTestStruct::*)(int)>(&OverloadTestStruct::calculate),
        static_cast<double (OverloadTestStruct::*)(double, double)>(&OverloadTestStruct::calculate)>();

    CHECK(calculate_symbol.overload_count == 3);

    CHECK(calculate_symbol.has_overload<int()>());
    auto result1 = calculate_symbol.call(obj);
    CHECK(result1 == 10);
    CHECK(obj.logs.back() == "calculate()");

    CHECK(calculate_symbol.has_overload<int(int)>());
    auto result2 = calculate_symbol.call(obj, 5);
    CHECK(result2 == 15);
    CHECK(obj.logs.back() == "calculate(int)");

    CHECK(calculate_symbol.has_overload<double(double, double)>());
    auto result3 = calculate_symbol.call(obj, 2.5, 3.0);
    CHECK(result3 == 7.5);
    CHECK(obj.logs.back() == "calculate(double, double)");
  }

  TEST_CASE("const_and_non_const_overloaded_methods") {
    OverloadTestStruct obj;
    const OverloadTestStruct const_obj;

    constexpr auto status_symbol = ctti::make_symbol_with_overloads<
        "status", ctti::no_attributes,
        static_cast<std::string (OverloadTestStruct::*)()>(&OverloadTestStruct::get_status),
        static_cast<std::string (OverloadTestStruct::*)() const>(&OverloadTestStruct::get_status)>();

    CHECK(status_symbol.overload_count == 2);

    CHECK(status_symbol.has_overload<std::string()>());
    auto result1 = status_symbol.call(obj);
    CHECK(result1 == "active");
    CHECK(obj.logs.back() == "get_status()");

    CHECK(status_symbol.has_overload<std::string() const>());
    auto result2 = status_symbol.call(const_obj);
    CHECK(result2 == "readonly");
  }

  TEST_CASE("overloaded_symbol_with_attributes") {
    constexpr auto multi_symbol =
        ctti::make_symbol_with_overloads<"process", ctti::attribute_list<ctti::read_only, ctti::deprecated>,
                                         static_cast<void (OverloadTestStruct::*)(int)>(&OverloadTestStruct::process),
                                         static_cast<void (OverloadTestStruct::*)(double)>(
                                             &OverloadTestStruct::process)>();

    CHECK(multi_symbol.name == "process");
    CHECK(multi_symbol.has_tag<ctti::read_only_tag>());
    CHECK(multi_symbol.has_attribute<ctti::deprecated>());
  }

  TEST_CASE("overloaded_symbol_unique_hashes") {
    constexpr auto process_symbol = ctti::make_symbol_with_overloads<
        "process", ctti::no_attributes, static_cast<void (OverloadTestStruct::*)(int)>(&OverloadTestStruct::process),
        static_cast<void (OverloadTestStruct::*)(double)>(&OverloadTestStruct::process)>();

    constexpr auto calculate_symbol = ctti::make_symbol_with_overloads<
        "calculate", ctti::no_attributes, static_cast<int (OverloadTestStruct::*)()>(&OverloadTestStruct::calculate),
        static_cast<int (OverloadTestStruct::*)(int)>(&OverloadTestStruct::calculate)>();

    CHECK(process_symbol.hash != calculate_symbol.hash);

    constexpr auto process_symbol2 = ctti::make_symbol_with_overloads<
        "process", ctti::no_attributes, static_cast<void (OverloadTestStruct::*)(int)>(&OverloadTestStruct::process)>();

    CHECK(process_symbol.hash == process_symbol2.hash);
  }

  TEST_CASE("non_overloaded_function_signature_check") {
    constexpr auto set_symbol = ctti::make_simple_symbol<"set_value", &SymbolTestStruct::set_value>();
    constexpr auto get_symbol = ctti::make_simple_symbol<"get_value", &SymbolTestStruct::get_value>();

    CHECK(set_symbol.has_overload<void(int)>());
    CHECK_FALSE(set_symbol.has_overload<void(double)>());
    CHECK_FALSE(set_symbol.has_overload<int(int)>());

    CHECK(get_symbol.has_overload<int() const>());
    CHECK_FALSE(get_symbol.has_overload<int()>());
    CHECK_FALSE(get_symbol.has_overload<void() const>());
  }
}
