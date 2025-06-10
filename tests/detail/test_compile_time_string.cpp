#include "doctest.h"

#include <ctti/detail/compile_time_string.hpp>

#include <iostream>

TEST_SUITE("detail::compile_time_string") {
  TEST_CASE("basic_construction") {
    constexpr auto str1 = ctti::detail::CompileTimeString{"hello"};
    constexpr auto str2 = ctti::detail::make_compile_time_string("world");

    static_assert(str1.Size() == 5);
    static_assert(str2.Size() == 5);
    static_assert(str1.View() == "hello");
    static_assert(str2.View() == "world");
  }

  TEST_CASE("empty_string") {
    constexpr auto empty = ctti::detail::CompileTimeString{""};

    static_assert(empty.Size() == 0);
    static_assert(empty.Empty());
    static_assert(empty.View() == "");
  }

  TEST_CASE("comparison") {
    constexpr auto str1 = ctti::detail::CompileTimeString{"hello"};
    constexpr auto str2 = ctti::detail::CompileTimeString{"hello"};
    constexpr auto str3 = ctti::detail::CompileTimeString{"world"};

    static_assert(str1 == str2);
    static_assert(str1 != str3);
    static_assert(str1 < str3);
  }

  TEST_CASE("concatenation") {
    constexpr auto str1 = ctti::detail::CompileTimeString{"hello"};
    constexpr auto str2 = ctti::detail::CompileTimeString{" world"};
    constexpr auto result = str1 + str2;

    static_assert(result.View() == "hello world");
    static_assert(result.Size() == 11);
  }

  TEST_CASE("indexing") {
    constexpr auto str = ctti::detail::CompileTimeString{"test"};

    static_assert(str[0] == 't');
    static_assert(str[1] == 'e');
    static_assert(str[2] == 's');
    static_assert(str[3] == 't');
  }

  TEST_CASE("string_operations") {
    constexpr auto str = ctti::detail::CompileTimeString{"hello_world_test"};

    static_assert(str.StartsWith("hello"));
    static_assert(str.EndsWith("test"));
    static_assert(!str.StartsWith("world"));
    static_assert(!str.EndsWith("hello"));

    static_assert(str.Find('_') == 5);
    static_assert(str.Find('x') == str.Size());
    static_assert(str.Find("world") == 6);
    static_assert(str.Find("xyz") == str.Size());
  }

  TEST_CASE("substr") {
    constexpr auto str = ctti::detail::CompileTimeString{"hello_world"};
    constexpr auto sub1 = str.Substr<0, 5>();
    constexpr auto sub2 = str.Substr<6>();

    static_assert(sub1.View() == "hello");
    static_assert(sub2.View() == "world");
  }

  TEST_CASE("literal_operator") {
    using namespace ctti::detail;
    constexpr auto str = "test"_cts;

    static_assert(str.View() == "test");
    static_assert(str.Size() == 4);
  }

  TEST_CASE("c_str") {
    static constexpr auto str = ctti::detail::CompileTimeString{"test"};
    static constexpr auto cstr = str.CStr();

    CHECK(std::string_view{cstr} == "test");
  }

  TEST_CASE("view_conversion") {
    static constexpr auto str = ctti::detail::CompileTimeString{"test"};
    static constexpr auto view = str.View();

    static_assert(view == "test");
    static_assert(view.size() == 4);
  }

  TEST_CASE("large_strings") {
    constexpr auto large = ctti::detail::CompileTimeString{
        "this_is_a_very_long_string_for_testing_the_compile_time_string_implementation"};

    static_assert(large.Size() == 77);
    static_assert(large.StartsWith("this_is"));
    static_assert(large.EndsWith("implementation"));
  }

  TEST_CASE("special_characters") {
    constexpr auto special = ctti::detail::CompileTimeString{"hello\tworld\n"};

    static_assert(special.Size() == 12);
    static_assert(special[5] == '\t');
    static_assert(special[11] == '\n');
  }

  TEST_CASE("numeric_strings") {
    constexpr auto num = ctti::detail::CompileTimeString{"12345"};

    static_assert(num.Size() == 5);
    static_assert(num[0] == '1');
    static_assert(num[4] == '5');
  }

  TEST_CASE("template_deduction") {
    constexpr ctti::detail::CompileTimeString auto_str{"auto_deduced"};

    static_assert(auto_str.View() == "auto_deduced");
    static_assert(auto_str.Size() == 12);
  }

  TEST_CASE("runtime_compatibility") {
    static constexpr auto str = ctti::detail::CompileTimeString{"runtime_test"};
    std::string runtime_str{str.View()};

    CHECK(runtime_str == "runtime_test");
    CHECK(runtime_str.size() == str.Size());
  }
}
