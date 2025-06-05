#include "doctest.h"

#include <ctti/detail/compile_time_string.hpp>

#include <iostream>

TEST_SUITE("detail::compile_time_string") {
  TEST_CASE("basic_construction") {
    constexpr auto str1 = ctti::detail::CompileTimeString{"hello"};
    constexpr auto str2 = ctti::detail::make_compile_time_string("world");

    static_assert(str1.size() == 5);
    static_assert(str2.size() == 5);
    static_assert(str1.view() == "hello");
    static_assert(str2.view() == "world");
  }

  TEST_CASE("empty_string") {
    constexpr auto empty = ctti::detail::CompileTimeString{""};

    static_assert(empty.size() == 0);
    static_assert(empty.empty());
    static_assert(empty.view() == "");
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

    static_assert(result.view() == "hello world");
    static_assert(result.size() == 11);
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

    static_assert(str.starts_with("hello"));
    static_assert(str.ends_with("test"));
    static_assert(!str.starts_with("world"));
    static_assert(!str.ends_with("hello"));

    static_assert(str.find('_') == 5);
    static_assert(str.find('x') == str.size());
    static_assert(str.find("world") == 6);
    static_assert(str.find("xyz") == str.size());
  }

  TEST_CASE("substr") {
    constexpr auto str = ctti::detail::CompileTimeString{"hello_world"};
    constexpr auto sub1 = str.substr<0, 5>();
    constexpr auto sub2 = str.substr<6>();

    static_assert(sub1.view() == "hello");
    static_assert(sub2.view() == "world");
  }

  TEST_CASE("literal_operator") {
    using namespace ctti::detail;
    constexpr auto str = "test"_cts;

    static_assert(str.view() == "test");
    static_assert(str.size() == 4);
  }

  TEST_CASE("c_str") {
    static constexpr auto str = ctti::detail::CompileTimeString{"test"};
    static constexpr auto cstr = str.c_str();

    CHECK(std::string_view{cstr} == "test");
  }

  TEST_CASE("view_conversion") {
    static constexpr auto str = ctti::detail::CompileTimeString{"test"};
    static constexpr auto view = str.view();

    static_assert(view == "test");
    static_assert(view.size() == 4);
  }

  TEST_CASE("large_strings") {
    constexpr auto large = ctti::detail::CompileTimeString{
        "this_is_a_very_long_string_for_testing_the_compile_time_string_implementation"};

    static_assert(large.size() == 77);
    static_assert(large.starts_with("this_is"));
    static_assert(large.ends_with("implementation"));
  }

  TEST_CASE("special_characters") {
    constexpr auto special = ctti::detail::CompileTimeString{"hello\tworld\n"};

    static_assert(special.size() == 12);
    static_assert(special[5] == '\t');
    static_assert(special[11] == '\n');
  }

  TEST_CASE("numeric_strings") {
    constexpr auto num = ctti::detail::CompileTimeString{"12345"};

    static_assert(num.size() == 5);
    static_assert(num[0] == '1');
    static_assert(num[4] == '5');
  }

  TEST_CASE("template_deduction") {
    constexpr ctti::detail::CompileTimeString auto_str{"auto_deduced"};

    static_assert(auto_str.view() == "auto_deduced");
    static_assert(auto_str.size() == 12);
  }

  TEST_CASE("runtime_compatibility") {
    static constexpr auto str = ctti::detail::CompileTimeString{"runtime_test"};
    std::string runtime_str{str.view()};

    CHECK(runtime_str == "runtime_test");
    CHECK(runtime_str.size() == str.size());
  }
}
