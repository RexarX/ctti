#include "doctest.h"

#include <ctti/attributes.hpp>
#include <ctti/reflection.hpp>
#include <ctti/symbol.hpp>

#include <iostream>
#include <string>

struct ReflectedStruct {
  int value = 42;
  std::string name = "test";
  double price = 99.99;

  void set_value(int v) { value = v; }
  int get_value() const { return value; }
};

template <>
struct ctti::meta<ReflectedStruct> {
  using type = ReflectedStruct;

  static constexpr auto reflection = ctti::make_reflection(
      ctti::member<"value", &ReflectedStruct::value>(), ctti::member<"name", &ReflectedStruct::name>(),
      ctti::member<"price", &ReflectedStruct::price>(), ctti::member<"set_value", &ReflectedStruct::set_value>(),
      ctti::member<"get_value", &ReflectedStruct::get_value>());
};

struct NonReflectedStruct {
  int x = 0;
  int y = 0;
};

TEST_SUITE("reflection") {
  TEST_CASE("reflectable_concept") {
    CHECK(ctti::reflectable<ReflectedStruct>);
    CHECK_FALSE(ctti::reflectable<NonReflectedStruct>);
  }

  TEST_CASE("get_reflection") {
    constexpr auto reflection = ctti::get_reflection<ReflectedStruct>();
    CHECK(reflection.size == 5);
  }

  TEST_CASE("symbol_count") {
    CHECK(ctti::symbol_count<ReflectedStruct>() == 5);
  }

  TEST_CASE("get_symbol_names") {
    auto names = ctti::get_symbol_names<ReflectedStruct>();
    CHECK(names.size() == 5);

    bool found_value = false;
    bool found_name = false;
    for (const auto& name : names) {
      if (name == "value") found_value = true;
      if (name == "name") found_name = true;
    }
    CHECK(found_value);
    CHECK(found_name);
  }

  TEST_CASE("has_symbol") {
    CHECK(ctti::has_symbol<"value", ReflectedStruct>());
    CHECK(ctti::has_symbol<"name", ReflectedStruct>());
    CHECK(ctti::has_symbol<"price", ReflectedStruct>());
    CHECK_FALSE(ctti::has_symbol<"nonexistent", ReflectedStruct>());
  }

  TEST_CASE("get_symbol") {
    constexpr auto value_symbol = ctti::get_symbol<ReflectedStruct, "value">();
    CHECK(value_symbol.name == "value");
    CHECK(value_symbol.is_owner_of<ReflectedStruct>());
  }

  TEST_CASE("for_each_symbol") {
    int count = 0;
    ctti::for_each_symbol<ReflectedStruct>([&count](auto symbol) {
      ++count;
      CHECK(!symbol.name.empty());
    });
    CHECK(count == 5);
  }

  TEST_CASE("member_definition") {
    constexpr auto value_def = ctti::member<"value", &ReflectedStruct::value>();
    constexpr auto attributed_def = ctti::member<"name", &ReflectedStruct::name>(ctti::deprecated{});

    CHECK(value_def.name == "value");
    CHECK(attributed_def.name == "name");
  }

  TEST_CASE("overloaded_member_definition") {
    struct TestStruct {
      void func() {}
      void func(int) {}
      void func(double) {}
    };

    constexpr auto overloaded_def =
        ctti::overloaded_member<"func", static_cast<void (TestStruct::*)()>(&TestStruct::func),
                                static_cast<void (TestStruct::*)(int)>(&TestStruct::func),
                                static_cast<void (TestStruct::*)(double)>(&TestStruct::func)>();

    CHECK(overloaded_def.name == "func");
  }

  TEST_CASE("attributed_member") {
    using since_attr = ctti::since<1>;
    constexpr auto attributed_def = ctti::member<"value", &ReflectedStruct::value>(since_attr{}, ctti::description{});

    CHECK(attributed_def.name == "value");
  }

  TEST_CASE("reflection_meta_information") {
    constexpr auto reflection = ctti::get_reflection<ReflectedStruct>();
    using reflection_type = decltype(reflection);

    CHECK(std::same_as<reflection_type::type, ReflectedStruct>);
    static_assert(reflection_type::size == 5);
  }

  TEST_CASE("symbol_access_through_reflection") {
    ReflectedStruct obj;
    obj.value = 123;
    obj.name = "reflected";

    constexpr auto value_symbol = ctti::get_symbol<ReflectedStruct, "value">();
    constexpr auto name_symbol = ctti::get_symbol<ReflectedStruct, "name">();

    // Access through symbol
    auto value_member = value_symbol.get_member<ReflectedStruct>();
    auto name_member = name_symbol.get_member<ReflectedStruct>();

    CHECK(obj.*value_member == 123);
    CHECK(obj.*name_member == "reflected");
  }
}
