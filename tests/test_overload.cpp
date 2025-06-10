#include "doctest.h"

#include <ctti/overload.hpp>
#include <ctti/symbol.hpp>

#include <iostream>
#include <string>

struct OverloadedStruct {
  int value = 42;

  void set(int v) { value = v; }
  void set(const std::string& s) { value = std::stoi(s); }
  void set(double d) { value = static_cast<int>(d); }

  int get() const { return value; }
  std::string get(bool as_string) const { return as_string ? std::to_string(value) : ""; }

  void process() { value *= 2; }
  void process(int multiplier) { value *= multiplier; }
  void process(int a, int b) { value = a * b; }
};

TEST_SUITE("overload") {
  TEST_CASE("overload_detection") {
    constexpr auto set_symbol = ctti::make_symbol_with_overloads<
        "set", ctti::no_attributes, static_cast<void (OverloadedStruct::*)(int)>(&OverloadedStruct::set),
        static_cast<void (OverloadedStruct::*)(const std::string&)>(&OverloadedStruct::set),
        static_cast<void (OverloadedStruct::*)(double)>(&OverloadedStruct::set)>();

    CHECK(decltype(set_symbol)::has_overload<OverloadedStruct&, int>());
    CHECK(decltype(set_symbol)::has_overload<OverloadedStruct&, const std::string&>());
    CHECK(decltype(set_symbol)::has_overload<OverloadedStruct&, double>());

    struct NonConvertible {};
    CHECK_FALSE(decltype(set_symbol)::has_overload<OverloadedStruct&, NonConvertible>());
  }

  TEST_CASE("overload_calling") {
    OverloadedStruct obj;

    constexpr auto set_symbol = ctti::make_symbol_with_overloads<
        "set", ctti::no_attributes, static_cast<void (OverloadedStruct::*)(int)>(&OverloadedStruct::set),
        static_cast<void (OverloadedStruct::*)(const std::string&)>(&OverloadedStruct::set),
        static_cast<void (OverloadedStruct::*)(double)>(&OverloadedStruct::set)>();

    ctti::call_overload<set_symbol>(obj, 100);
    CHECK(obj.value == 100);

    ctti::call_overload<set_symbol>(obj, std::string("200"));
    CHECK(obj.value == 200);

    ctti::call_overload<set_symbol>(obj, 3.14);
    CHECK(obj.value == 3);
  }

  TEST_CASE("overload_wrapper") {
    OverloadedStruct obj;

    constexpr auto set_symbol = ctti::make_symbol_with_overloads<
        "set", ctti::no_attributes, static_cast<void (OverloadedStruct::*)(int)>(&OverloadedStruct::set),
        static_cast<void (OverloadedStruct::*)(const std::string&)>(&OverloadedStruct::set),
        static_cast<void (OverloadedStruct::*)(double)>(&OverloadedStruct::set)>();

    auto wrapper = ctti::get_overload_wrapper<set_symbol>(obj);

    wrapper(42);
    CHECK(obj.value == 42);

    wrapper(std::string("99"));
    CHECK(obj.value == 99);

    wrapper(2.7);
    CHECK(obj.value == 2);
  }

  TEST_CASE("overload_query") {
    constexpr auto set_symbol = ctti::make_symbol_with_overloads<
        "set", ctti::no_attributes, static_cast<void (OverloadedStruct::*)(int)>(&OverloadedStruct::set),
        static_cast<void (OverloadedStruct::*)(const std::string&)>(&OverloadedStruct::set),
        static_cast<void (OverloadedStruct::*)(double)>(&OverloadedStruct::set)>();

    auto query = ctti::query_overloads<set_symbol, OverloadedStruct>();

    CHECK(query.has<int>());
    CHECK(query.has<const std::string&>());
    CHECK(query.has<double>());

    struct NonConvertibleType {
      NonConvertibleType() = delete;
      NonConvertibleType(const NonConvertibleType&) = delete;
      NonConvertibleType& operator=(const NonConvertibleType&) = delete;
    };
    CHECK_FALSE(query.has<NonConvertibleType>());
  }

  TEST_CASE("overload_symbol_properties") {
    constexpr auto set_symbol =
        ctti::make_symbol_with_overloads<"set", ctti::no_attributes,
                                         static_cast<void (OverloadedStruct::*)(int)>(&OverloadedStruct::set)>();

    CHECK(decltype(set_symbol)::name == "set");
    CHECK(decltype(set_symbol)::hash != 0);
    CHECK(decltype(set_symbol)::has_overloads);
    CHECK(decltype(set_symbol)::overload_count == 1);
  }

  TEST_CASE("overload_ownership") {
    constexpr auto set_symbol =
        ctti::make_symbol_with_overloads<"set", ctti::no_attributes,
                                         static_cast<void (OverloadedStruct::*)(int)>(&OverloadedStruct::set)>();

    CHECK(decltype(set_symbol)::is_owner_of<OverloadedStruct>());
    CHECK_FALSE(decltype(set_symbol)::is_owner_of<int>());
  }

  TEST_CASE("can_call_with") {
    constexpr auto set_symbol =
        ctti::make_symbol_with_overloads<"set", ctti::no_attributes,
                                         static_cast<void (OverloadedStruct::*)(int)>(&OverloadedStruct::set),
                                         static_cast<void (OverloadedStruct::*)(double)>(&OverloadedStruct::set)>();

    CHECK(ctti::can_call_with<set_symbol, OverloadedStruct&, int>());
    CHECK(ctti::can_call_with<set_symbol, OverloadedStruct&, double>());
    CHECK_FALSE(ctti::can_call_with<set_symbol, OverloadedStruct&, std::string>());
  }

  TEST_CASE("get_overload_count") {
    constexpr auto multi_symbol = ctti::make_symbol_with_overloads<
        "multi", ctti::no_attributes, static_cast<void (OverloadedStruct::*)()>(&OverloadedStruct::process),
        static_cast<void (OverloadedStruct::*)(int)>(&OverloadedStruct::process),
        static_cast<void (OverloadedStruct::*)(int, int)>(&OverloadedStruct::process)>();

    CHECK(ctti::get_overload_count<multi_symbol>() == 3);
  }
}
