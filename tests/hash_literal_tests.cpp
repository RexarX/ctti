#include "catch.hpp"

#include <ctti/hash_literal.hpp>

#ifdef CTTI_HASH_LITERAL_NAMESPACE
using namespace CTTI_HASH_LITERAL_NAMESPACE;
#endif

TEST_CASE("hash_literal") {
  SECTION("basic strings") {
    constexpr auto hello_hash = "hello"_sh;
    constexpr auto hello2_hash = "hello"_sh;
    constexpr auto world_hash = "world"_sh;

    REQUIRE(hello_hash == hello2_hash);
    REQUIRE(hello_hash != world_hash);
  }

  SECTION("different lengths") {
    constexpr auto short_hash = "abc"_sh;
    constexpr auto long_hash = "abcdefghijklmnopqrstuvwxyz"_sh;

    REQUIRE(short_hash != long_hash);
  }

  SECTION("case sensitivity") {
    constexpr auto upper_hash = "ABC"_sh;
    constexpr auto lower_hash = "abc"_sh;

    REQUIRE(upper_hash != lower_hash);
  }

  SECTION("empty string") {
    constexpr auto empty_hash = ""_sh;
    constexpr auto space_hash = " "_sh;

    REQUIRE(empty_hash != space_hash);
  }
}
