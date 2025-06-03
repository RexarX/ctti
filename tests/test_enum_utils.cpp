#include "doctest.h"

#include <ctti/enum_utils.hpp>

#include <array>
#include <iostream>

enum class ScopedEnum { First = 1, Second = 2, Third = 3 };
enum UnscopedEnum { A = 10, B = 20, C = 30 };

TEST_SUITE("enum_utils") {
  TEST_CASE("enum_concepts") {
    CHECK(ctti::scoped_enum<ScopedEnum>);
    CHECK_FALSE(ctti::scoped_enum<UnscopedEnum>);
    CHECK_FALSE(ctti::scoped_enum<int>);

    CHECK(ctti::unscoped_enum<UnscopedEnum>);
    CHECK_FALSE(ctti::unscoped_enum<ScopedEnum>);
    CHECK_FALSE(ctti::unscoped_enum<int>);
  }

  TEST_CASE("enum_info_basic") {
    auto info = ctti::get_enum_info<ScopedEnum>();

    CHECK(info.is_scoped());
    CHECK(std::same_as<decltype(info)::enum_type, ScopedEnum>);
    CHECK(std::same_as<decltype(info)::underlying_type, int>);

    auto name = info.name();
    CHECK(name.find("ScopedEnum") != std::string_view::npos);
  }

  TEST_CASE("enum_value_names") {
    auto info = ctti::get_enum_info<ScopedEnum>();

    auto first_name = info.name_of_value<ScopedEnum::First>();
    auto second_name = info.name_of_value<ScopedEnum::Second>();

    CHECK(first_name.find("First") != std::string_view::npos);
    CHECK(second_name.find("Second") != std::string_view::npos);
  }

  TEST_CASE("enum_underlying_values") {
    auto info = ctti::get_enum_info<ScopedEnum>();

    CHECK(info.underlying_value<ScopedEnum::First>() == 1);
    CHECK(info.underlying_value<ScopedEnum::Second>() == 2);
    CHECK(info.underlying_value<ScopedEnum::Third>() == 3);
  }

  TEST_CASE("enum_value_list") {
    auto list = ctti::make_enum_list<ScopedEnum, ScopedEnum::First, ScopedEnum::Second, ScopedEnum::Third>();

    CHECK(list.count == 3);
    CHECK(list.at<0>() == ScopedEnum::First);
    CHECK(list.at<1>() == ScopedEnum::Second);
    CHECK(list.at<2>() == ScopedEnum::Third);

    CHECK(list.contains<ScopedEnum::First>());
    CHECK(list.contains<ScopedEnum::Second>());
    CHECK_FALSE(list.contains<static_cast<ScopedEnum>(99)>());

    auto names = list.names();
    CHECK(names.size() == 3);
  }

  TEST_CASE("enum_utility_functions") {
    CHECK(ctti::enum_name<ScopedEnum, ScopedEnum::First>().find("First") != std::string_view::npos);
    CHECK(ctti::enum_type_name<ScopedEnum>().find("ScopedEnum") != std::string_view::npos);
    CHECK(ctti::enum_underlying_value<ScopedEnum, ScopedEnum::Second>() == 2);

    CHECK(ctti::enum_equal<ScopedEnum, ScopedEnum::First, ScopedEnum::First>());
    CHECK_FALSE(ctti::enum_equal<ScopedEnum, ScopedEnum::First, ScopedEnum::Second>());
    CHECK(ctti::enum_less<ScopedEnum, ScopedEnum::First, ScopedEnum::Second>());
  }

  TEST_CASE("unscoped_enum") {
    auto info = ctti::get_enum_info<UnscopedEnum>();

    CHECK_FALSE(info.is_scoped());
    CHECK(info.underlying_value<UnscopedEnum::A>() == 10);
    CHECK(info.underlying_value<UnscopedEnum::B>() == 20);
  }

  TEST_CASE("from_underlying") {
    auto info = ctti::get_enum_info<ScopedEnum>();

    auto opt1 = info.from_underlying(1);
    auto opt2 = info.from_underlying(2);

    CHECK(opt1.has_value());
    CHECK(opt1.value() == ScopedEnum::First);
    CHECK(opt2.has_value());
    CHECK(opt2.value() == ScopedEnum::Second);
  }

  TEST_CASE("enum_list_for_each") {
    auto list = ctti::make_enum_list<ScopedEnum, ScopedEnum::First, ScopedEnum::Second>();

    int count = 0;
    list.for_each([&count](auto index, ScopedEnum value) {
      ++count;
      CHECK((value == ScopedEnum::First || value == ScopedEnum::Second));
    });

    CHECK(count == 2);
  }
}
