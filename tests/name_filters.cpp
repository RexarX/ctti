#include "catch.hpp"

#include <ctti/detail/name_filters.hpp>

#include <string_view>

using namespace ctti::detail;

TEST_CASE("name_filters") {
  constexpr std::string_view foo_full = "foo::foo::foo::foo";
  constexpr std::string_view foobar = "foo::bar";

  SECTION("find_ith") {
    REQUIRE(find_ith(foo_full, "foo", 0) == 0);
    REQUIRE(find_ith(foo_full, "foo", 1) == std::string_view("foo::").length());
    REQUIRE(find_ith(foo_full, "foo", 2) == std::string_view("foo::foo::").length());

    REQUIRE(find_ith(foo_full, "::", 0) == std::string_view{"foo"}.length());
    REQUIRE(find_ith(foo_full, "::", 1) == std::string_view("foo::foo").length());
    REQUIRE(find_ith(foo_full, "::", 2) == std::string_view("foo::foo::foo").length());

    REQUIRE(find_ith(foobar, "foo", 1) == std::string_view::npos);
    REQUIRE(find_ith(foobar, "foo", 2) == std::string_view::npos);
    REQUIRE(find_ith(foobar, "foo", 3) == std::string_view::npos);

    REQUIRE(find_ith(foobar, "::", 1) == std::string_view::npos);
  }
}
