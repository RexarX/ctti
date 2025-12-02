#include "doctest.h"

#include <ctti/enum_utils.hpp>

namespace {

enum class Color { Red = 0, Green = 1, Blue = 2 };
enum class Status { Active = 1, Inactive = 2, Pending = 3 };
enum UnscopedEnum { A = 10, B = 20, C = 30 };

// Enum for testing registered enum functionality
enum class RegisteredColor { Red = 0, Green = 1, Blue = 2 };
enum class UnregisteredEnum { X, Y, Z };

// Define enum value lists for runtime features
inline constexpr auto kColorList = ctti::make_enum_list<Color::Red, Color::Green, Color::Blue>();
inline constexpr auto kStatusList = ctti::make_enum_list<Status::Active, Status::Inactive, Status::Pending>();
inline constexpr auto kUnscopedList = ctti::make_enum_list<A, B, C>();

}  // namespace

// Register enum values using enum_values specialization
template <>
struct ctti::enum_values<RegisteredColor> {
  static constexpr auto values =
      ctti::make_enum_list<RegisteredColor::Red, RegisteredColor::Green, RegisteredColor::Blue>();
};

TEST_SUITE("enum_utils") {
  TEST_CASE("enum_concepts") {
    CHECK(ctti::scoped_enum<Color>);
    CHECK(ctti::scoped_enum<Status>);
    CHECK_FALSE(ctti::scoped_enum<UnscopedEnum>);
    CHECK_FALSE(ctti::scoped_enum<int>);

    CHECK(ctti::unscoped_enum<UnscopedEnum>);
    CHECK_FALSE(ctti::unscoped_enum<Color>);
    CHECK_FALSE(ctti::unscoped_enum<int>);
  }

  TEST_CASE("enum_info_basic") {
    auto info = ctti::get_enum_info<Color>();

    CHECK(info.is_scoped());
    CHECK(std::same_as<decltype(info)::enum_type, Color>);
    CHECK(std::same_as<decltype(info)::underlying_type, int>);

    CHECK_EQ(info.name(), "Color");
  }

  TEST_CASE("enum_value_names_compile_time") {
    auto info = ctti::get_enum_info<Color>();

    CHECK_EQ(info.name_of_value<Color::Red>(), "Red");
    CHECK_EQ(info.name_of_value<Color::Green>(), "Green");
    CHECK_EQ(info.name_of_value<Color::Blue>(), "Blue");
  }

  TEST_CASE("enum_underlying_values") {
    auto info = ctti::get_enum_info<Color>();

    CHECK_EQ(info.underlying_value<Color::Red>(), 0);
    CHECK_EQ(info.underlying_value<Color::Green>(), 1);
    CHECK_EQ(info.underlying_value<Color::Blue>(), 2);
  }

  TEST_CASE("enum_value_list_basic") {
    CHECK_EQ(kColorList.count, 3);

    const auto& values = kColorList.values();
    CHECK_EQ(values.size(), 3);
    CHECK_EQ(values[0], Color::Red);
    CHECK_EQ(values[1], Color::Green);
    CHECK_EQ(values[2], Color::Blue);
  }

  TEST_CASE("enum_value_list_names") {
    const auto& names = kColorList.names();

    CHECK_EQ(names.size(), 3);
    CHECK_EQ(names[0], "Red");
    CHECK_EQ(names[1], "Green");
    CHECK_EQ(names[2], "Blue");
  }

  TEST_CASE("enum_value_list_entries") {
    const auto& entries = kColorList.entries();

    CHECK_EQ(entries.size(), 3);

    CHECK_EQ(entries[0].first, Color::Red);
    CHECK_EQ(entries[0].second, "Red");

    CHECK_EQ(entries[1].first, Color::Green);
    CHECK_EQ(entries[1].second, "Green");

    CHECK_EQ(entries[2].first, Color::Blue);
    CHECK_EQ(entries[2].second, "Blue");
  }

  TEST_CASE("enum_value_list_at") {
    CHECK_EQ(kColorList.at<0>(), Color::Red);
    CHECK_EQ(kColorList.at<1>(), Color::Green);
    CHECK_EQ(kColorList.at<2>(), Color::Blue);

    // Runtime access
    auto val0 = kColorList.value_at(0);
    auto val1 = kColorList.value_at(1);
    auto val2 = kColorList.value_at(2);
    auto invalid = kColorList.value_at(999);

    CHECK(val0.has_value());
    CHECK(val1.has_value());
    CHECK(val2.has_value());
    CHECK_FALSE(invalid.has_value());

    CHECK_EQ(val0.value(), Color::Red);
    CHECK_EQ(val1.value(), Color::Green);
    CHECK_EQ(val2.value(), Color::Blue);
  }

  TEST_CASE("enum_value_list_contains_compile_time") {
    CHECK(kColorList.contains<Color::Red>());
    CHECK(kColorList.contains<Color::Green>());
    CHECK(kColorList.contains<Color::Blue>());
  }

  TEST_CASE("enum_value_list_contains_runtime") {
    CHECK(kColorList.contains(Color::Red));
    CHECK(kColorList.contains(Color::Green));
    CHECK(kColorList.contains(Color::Blue));
    CHECK_FALSE(kColorList.contains(static_cast<Color>(999)));
  }

  TEST_CASE("enum_value_list_name_of_runtime") {
    auto red_name = kColorList.name_of(Color::Red);
    auto green_name = kColorList.name_of(Color::Green);
    auto blue_name = kColorList.name_of(Color::Blue);
    auto invalid_name = kColorList.name_of(static_cast<Color>(999));

    CHECK(red_name.has_value());
    CHECK(green_name.has_value());
    CHECK(blue_name.has_value());
    CHECK_FALSE(invalid_name.has_value());

    CHECK_EQ(red_name.value(), "Red");
    CHECK_EQ(green_name.value(), "Green");
    CHECK_EQ(blue_name.value(), "Blue");
  }

  TEST_CASE("enum_value_list_cast") {
    auto red = kColorList.cast("Red");
    auto green = kColorList.cast("Green");
    auto blue = kColorList.cast("Blue");
    auto invalid = kColorList.cast("NotAColor");

    CHECK(red.has_value());
    CHECK(green.has_value());
    CHECK(blue.has_value());
    CHECK_FALSE(invalid.has_value());

    CHECK_EQ(red.value(), Color::Red);
    CHECK_EQ(green.value(), Color::Green);
    CHECK_EQ(blue.value(), Color::Blue);
  }

  TEST_CASE("enum_value_list_index_of") {
    auto red_idx = kColorList.index_of(Color::Red);
    auto green_idx = kColorList.index_of(Color::Green);
    auto blue_idx = kColorList.index_of(Color::Blue);
    auto invalid_idx = kColorList.index_of(static_cast<Color>(999));

    CHECK(red_idx.has_value());
    CHECK(green_idx.has_value());
    CHECK(blue_idx.has_value());
    CHECK_FALSE(invalid_idx.has_value());

    CHECK_EQ(red_idx.value(), 0);
    CHECK_EQ(green_idx.value(), 1);
    CHECK_EQ(blue_idx.value(), 2);
  }

  TEST_CASE("enum_value_list_from_underlying") {
    auto red = kColorList.from_underlying(0);
    auto green = kColorList.from_underlying(1);
    auto invalid = kColorList.from_underlying(999);

    CHECK(red.has_value());
    CHECK(green.has_value());
    CHECK_FALSE(invalid.has_value());

    CHECK_EQ(red.value(), Color::Red);
    CHECK_EQ(green.value(), Color::Green);
  }

  TEST_CASE("enum_value_list_for_each") {
    int count = 0;
    kColorList.for_each([&count](auto index, Color value) {
      ++count;
      CHECK((value == Color::Red || value == Color::Green || value == Color::Blue));
    });

    CHECK_EQ(count, 3);
  }

  TEST_CASE("make_enum_list") {
    auto list = ctti::make_enum_list<Color::Red, Color::Green, Color::Blue>();

    CHECK_EQ(list.count, 3);
    CHECK_EQ(list.at<0>(), Color::Red);
    CHECK_EQ(list.at<1>(), Color::Green);
    CHECK_EQ(list.at<2>(), Color::Blue);
  }

  TEST_CASE("enum_name_compile_time") {
    CHECK_EQ(ctti::enum_name<Color, Color::Red>(), "Red");
    CHECK_EQ(ctti::enum_name<Color, Color::Green>(), "Green");
    CHECK_EQ(ctti::enum_name<Color, Color::Blue>(), "Blue");
  }

  TEST_CASE("enum_type_name") {
    CHECK_EQ(ctti::enum_type_name<Color>(), "Color");
    CHECK_EQ(ctti::enum_type_name<Status>(), "Status");
  }

  TEST_CASE("enum_underlying_value_compile_time") {
    CHECK_EQ(ctti::enum_underlying_value<Color, Color::Red>(), 0);
    CHECK_EQ(ctti::enum_underlying_value<Color, Color::Green>(), 1);
    CHECK_EQ(ctti::enum_underlying_value<Status, Status::Active>(), 1);
  }

  TEST_CASE("enum_underlying_runtime") {
    CHECK_EQ(ctti::enum_underlying(Color::Red), 0);
    CHECK_EQ(ctti::enum_underlying(Color::Green), 1);
    CHECK_EQ(ctti::enum_underlying(Color::Blue), 2);

    CHECK_EQ(ctti::enum_underlying(Status::Active), 1);
    CHECK_EQ(ctti::enum_underlying(Status::Inactive), 2);
  }

  TEST_CASE("enum_equal_compile_time") {
    CHECK(ctti::enum_equal<Color, Color::Red, Color::Red>());
    CHECK_FALSE(ctti::enum_equal<Color, Color::Red, Color::Green>());
  }

  TEST_CASE("enum_less_compile_time") {
    CHECK(ctti::enum_less<Color, Color::Red, Color::Green>());
    CHECK(ctti::enum_less<Color, Color::Green, Color::Blue>());
    CHECK_FALSE(ctti::enum_less<Color, Color::Blue, Color::Red>());
  }

  TEST_CASE("unscoped_enum") {
    auto info = ctti::get_enum_info<UnscopedEnum>();

    CHECK_FALSE(info.is_scoped());
    CHECK_EQ(info.underlying_value<A>(), 10);
    CHECK_EQ(info.underlying_value<B>(), 20);
  }

  TEST_CASE("unscoped_enum_list_runtime") {
    auto a_name = kUnscopedList.name_of(A);
    auto b_name = kUnscopedList.name_of(B);
    auto c_name = kUnscopedList.name_of(C);

    CHECK(a_name.has_value());
    CHECK(b_name.has_value());
    CHECK(c_name.has_value());

    CHECK_EQ(a_name.value(), "A");
    CHECK_EQ(b_name.value(), "B");
    CHECK_EQ(c_name.value(), "C");
  }

  TEST_CASE("unscoped_enum_list_cast") {
    auto a = kUnscopedList.cast("A");
    auto b = kUnscopedList.cast("B");

    CHECK(a.has_value());
    CHECK(b.has_value());

    CHECK_EQ(a.value(), A);
    CHECK_EQ(b.value(), B);
  }

  TEST_CASE("status_enum_with_non_zero_start") {
    CHECK_EQ(kStatusList.count, 3);

    const auto& values = kStatusList.values();
    CHECK_EQ(values[0], Status::Active);
    CHECK_EQ(values[1], Status::Inactive);
    CHECK_EQ(values[2], Status::Pending);

    auto active_name = kStatusList.name_of(Status::Active);
    CHECK(active_name.has_value());
    CHECK_EQ(active_name.value(), "Active");

    auto active_cast = kStatusList.cast("Active");
    CHECK(active_cast.has_value());
    CHECK_EQ(active_cast.value(), Status::Active);
  }

  TEST_CASE("from_underlying_enum_info") {
    auto info = ctti::get_enum_info<Color>();

    // Note: enum_info::from_underlying doesn't validate, just casts
    auto opt = info.from_underlying(0);
    CHECK(opt.has_value());
    CHECK_EQ(opt.value(), Color::Red);
  }

  TEST_CASE("compile_time_constexpr_checks") {
    // Verify compile-time evaluation
    static_assert(ctti::enum_name<Color, Color::Red>() == "Red");
    static_assert(ctti::enum_underlying_value<Color, Color::Red>() == 0);
    static_assert(ctti::enum_equal<Color, Color::Red, Color::Red>());
    static_assert(ctti::enum_less<Color, Color::Red, Color::Green>());
    static_assert(kColorList.count == 3);
    static_assert(kColorList.contains<Color::Red>());

    // Runtime checks to confirm
    CHECK_EQ(ctti::enum_name<Color, Color::Red>(), "Red");
  }

  TEST_CASE("registered_enum_concept") {
    CHECK(ctti::registered_enum<RegisteredColor>);
    CHECK_FALSE(ctti::registered_enum<UnregisteredEnum>);
    CHECK_FALSE(ctti::registered_enum<Color>);  // Not registered via enum_values
    CHECK_FALSE(ctti::registered_enum<int>);
  }

  TEST_CASE("registered_enum_name") {
    auto red_name = ctti::enum_name(RegisteredColor::Red);
    auto green_name = ctti::enum_name(RegisteredColor::Green);
    auto blue_name = ctti::enum_name(RegisteredColor::Blue);
    auto invalid_name = ctti::enum_name(static_cast<RegisteredColor>(999));

    CHECK(red_name.has_value());
    CHECK(green_name.has_value());
    CHECK(blue_name.has_value());
    CHECK_FALSE(invalid_name.has_value());

    CHECK_EQ(red_name.value(), "Red");
    CHECK_EQ(green_name.value(), "Green");
    CHECK_EQ(blue_name.value(), "Blue");
  }

  TEST_CASE("registered_enum_cast") {
    auto red = ctti::enum_cast<RegisteredColor>("Red");
    auto green = ctti::enum_cast<RegisteredColor>("Green");
    auto blue = ctti::enum_cast<RegisteredColor>("Blue");
    auto invalid = ctti::enum_cast<RegisteredColor>("Invalid");

    CHECK(red.has_value());
    CHECK(green.has_value());
    CHECK(blue.has_value());
    CHECK_FALSE(invalid.has_value());

    CHECK_EQ(red.value(), RegisteredColor::Red);
    CHECK_EQ(green.value(), RegisteredColor::Green);
    CHECK_EQ(blue.value(), RegisteredColor::Blue);
  }

  TEST_CASE("registered_enum_contains") {
    CHECK(ctti::enum_contains(RegisteredColor::Red));
    CHECK(ctti::enum_contains(RegisteredColor::Green));
    CHECK(ctti::enum_contains(RegisteredColor::Blue));
    CHECK_FALSE(ctti::enum_contains(static_cast<RegisteredColor>(999)));
  }

  TEST_CASE("registered_enum_index") {
    auto red_idx = ctti::enum_index(RegisteredColor::Red);
    auto green_idx = ctti::enum_index(RegisteredColor::Green);
    auto blue_idx = ctti::enum_index(RegisteredColor::Blue);
    auto invalid_idx = ctti::enum_index(static_cast<RegisteredColor>(999));

    CHECK(red_idx.has_value());
    CHECK(green_idx.has_value());
    CHECK(blue_idx.has_value());
    CHECK_FALSE(invalid_idx.has_value());

    CHECK_EQ(red_idx.value(), 0);
    CHECK_EQ(green_idx.value(), 1);
    CHECK_EQ(blue_idx.value(), 2);
  }

  TEST_CASE("registered_enum_count") {
    CHECK_EQ(ctti::enum_count<RegisteredColor>(), 3);
  }

  TEST_CASE("registered_enum_entries_and_names") {
    const auto& entries = ctti::enum_entries<RegisteredColor>();
    const auto& names = ctti::enum_names<RegisteredColor>();

    CHECK_EQ(entries.size(), 3);
    CHECK_EQ(names.size(), 3);

    CHECK_EQ(entries[0], RegisteredColor::Red);
    CHECK_EQ(entries[1], RegisteredColor::Green);
    CHECK_EQ(entries[2], RegisteredColor::Blue);

    CHECK_EQ(names[0], "Red");
    CHECK_EQ(names[1], "Green");
    CHECK_EQ(names[2], "Blue");
  }

  TEST_CASE("registered_enum_from_underlying") {
    auto red = ctti::enum_from_underlying<RegisteredColor>(0);
    auto green = ctti::enum_from_underlying<RegisteredColor>(1);
    auto blue = ctti::enum_from_underlying<RegisteredColor>(2);
    auto invalid = ctti::enum_from_underlying<RegisteredColor>(999);

    CHECK(red.has_value());
    CHECK(green.has_value());
    CHECK(blue.has_value());
    CHECK_FALSE(invalid.has_value());

    CHECK_EQ(red.value(), RegisteredColor::Red);
    CHECK_EQ(green.value(), RegisteredColor::Green);
    CHECK_EQ(blue.value(), RegisteredColor::Blue);
  }

  TEST_CASE("registered_enum_for_each") {
    int count = 0;
    ctti::enum_for_each<RegisteredColor>([&count](auto index, RegisteredColor value) {
      ++count;
      CHECK((value == RegisteredColor::Red || value == RegisteredColor::Green || value == RegisteredColor::Blue));
    });

    CHECK_EQ(count, 3);
  }

  TEST_CASE("registered_enum_compile_time") {
    // Verify compile-time evaluation of registered enum functions
    static_assert(ctti::registered_enum<RegisteredColor>);
    static_assert(ctti::enum_count<RegisteredColor>() == 3);
    static_assert(ctti::enum_entries<RegisteredColor>()[0] == RegisteredColor::Red);
    static_assert(ctti::enum_names<RegisteredColor>()[0] == "Red");

    // Runtime confirmation
    CHECK_EQ(ctti::enum_count<RegisteredColor>(), 3);
  }
}
