#include "doctest.h"

#include <ctti/hash.hpp>

#include <array>
#include <iostream>
#include <string_view>

using namespace ctti::hash_literals;

TEST_SUITE("hash") {
  TEST_CASE("basic_fnv1a_hash") {
    constexpr auto hash1 = ctti::fnv1a_hash("test");
    constexpr auto hash2 = ctti::fnv1a_hash("hello");
    constexpr auto hash3 = ctti::fnv1a_hash("test");

    CHECK(hash1 != 0);
    CHECK(hash2 != 0);
    CHECK(hash1 != hash2);
    CHECK(hash1 == hash3);
  }

  TEST_CASE("empty_string_hash") {
    constexpr auto empty_hash = ctti::fnv1a_hash("");
    CHECK(empty_hash != 0);  // FNV-1a has a non-zero basis
  }

  TEST_CASE("single_character_hash") {
    constexpr auto hash_a = ctti::fnv1a_hash("a");
    constexpr auto hash_b = ctti::fnv1a_hash("b");

    CHECK(hash_a != hash_b);
    CHECK(hash_a != 0);
    CHECK(hash_b != 0);
  }

  TEST_CASE("case_sensitivity") {
    constexpr auto lower = ctti::fnv1a_hash("hello");
    constexpr auto upper = ctti::fnv1a_hash("HELLO");
    constexpr auto mixed = ctti::fnv1a_hash("Hello");

    CHECK(lower != upper);
    CHECK(lower != mixed);
    CHECK(upper != mixed);
  }

  TEST_CASE("hash_literal") {
    constexpr auto hash1 = "test"_sh;
    constexpr auto hash2 = "hello"_sh;
    constexpr auto hash3 = "test"_sh;

    CHECK(hash1 != 0);
    CHECK(hash2 != 0);
    CHECK(hash1 != hash2);
    CHECK(hash1 == hash3);
  }

  TEST_CASE("hash_literal_consistency") {
    constexpr auto literal_hash = "test_string"_sh;
    constexpr auto function_hash = ctti::fnv1a_hash("test_string");

    CHECK(literal_hash == function_hash);
  }

  TEST_CASE("compile_time_evaluation") {
    constexpr auto hash = ctti::fnv1a_hash("compile_time_test");
    static_assert(hash != 0);
    static_assert(hash == ctti::fnv1a_hash("compile_time_test"));

    CHECK(hash != 0);
  }

  TEST_CASE("switch_case_usage") {
    constexpr auto process_command = [](std::string_view cmd) -> std::string_view {
      switch (ctti::fnv1a_hash(cmd)) {
        case "start"_sh:
          return "Starting...";
        case "stop"_sh:
          return "Stopping...";
        case "reset"_sh:
          return "Resetting...";
        default:
          return "Unknown command";
      }
    };

    CHECK(process_command("start") == "Starting...");
    CHECK(process_command("stop") == "Stopping...");
    CHECK(process_command("reset") == "Resetting...");
    CHECK(process_command("unknown") == "Unknown command");
  }

  TEST_CASE("long_strings") {
    constexpr auto long1 = ctti::fnv1a_hash("this_is_a_very_long_string_for_testing");
    constexpr auto long2 = ctti::fnv1a_hash("this_is_a_very_long_string_for_testin");   // One char less
    constexpr auto long3 = ctti::fnv1a_hash("this_is_a_very_long_string_for_testinX");  // Different last char

    CHECK(long1 != long2);
    CHECK(long1 != long3);
    CHECK(long2 != long3);
    CHECK(long1 != 0);
  }

  TEST_CASE("special_characters") {
    constexpr auto special1 = "hello@world.com"_sh;
    constexpr auto special2 = "path/to/file.txt"_sh;
    constexpr auto special3 = "namespace::class::member"_sh;

    CHECK(special1 != 0);
    CHECK(special2 != 0);
    CHECK(special3 != 0);
    CHECK(special1 != special2);
    CHECK(special2 != special3);
    CHECK(special1 != special3);
  }

  TEST_CASE("hash_type_consistency") {
    constexpr auto hash = ctti::fnv1a_hash("test");
    static_assert(std::same_as<decltype(hash), const std::uint64_t>);
  }
}
