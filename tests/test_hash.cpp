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

  TEST_CASE("different_lengths_same_prefix") {
    constexpr auto hash1 = ctti::fnv1a_hash("test");
    constexpr auto hash2 = ctti::fnv1a_hash("testing");
    constexpr auto hash3 = ctti::fnv1a_hash("te");

    CHECK(hash1 != hash2);
    CHECK(hash1 != hash3);
    CHECK(hash2 != hash3);
  }

  TEST_CASE("case_sensitivity") {
    constexpr auto lower = ctti::fnv1a_hash("hello");
    constexpr auto upper = ctti::fnv1a_hash("HELLO");
    constexpr auto mixed = ctti::fnv1a_hash("Hello");

    CHECK(lower != upper);
    CHECK(lower != mixed);
    CHECK(upper != mixed);
  }

  TEST_CASE("special_characters") {
    constexpr auto space_hash = ctti::fnv1a_hash("hello world");
    constexpr auto tab_hash = ctti::fnv1a_hash("hello\tworld");
    constexpr auto newline_hash = ctti::fnv1a_hash("hello\nworld");
    constexpr auto symbol_hash = ctti::fnv1a_hash("hello@world.com");

    CHECK(space_hash != tab_hash);
    CHECK(tab_hash != newline_hash);
    CHECK(newline_hash != symbol_hash);
    CHECK(space_hash != symbol_hash);
  }

  TEST_CASE("numeric_strings") {
    constexpr auto num1 = ctti::fnv1a_hash("123");
    constexpr auto num2 = ctti::fnv1a_hash("456");
    constexpr auto num3 = ctti::fnv1a_hash("0");
    constexpr auto negative = ctti::fnv1a_hash("-123");

    CHECK(num1 != num2);
    CHECK(num2 != num3);
    CHECK(num1 != negative);
    CHECK(num3 != negative);
  }

  TEST_CASE("long_strings") {
    constexpr auto long1 =
        ctti::fnv1a_hash("this_is_a_very_long_string_for_testing_the_hash_function_with_many_characters");
    constexpr auto long2 = ctti::fnv1a_hash(
        "this_is_a_very_long_string_for_testing_the_hash_function_with_many_character");  // One char less
    constexpr auto long3 = ctti::fnv1a_hash(
        "this_is_a_very_long_string_for_testing_the_hash_function_with_many_characterX");  // Different last char

    CHECK(long1 != long2);
    CHECK(long1 != long3);
    CHECK(long2 != long3);
    CHECK(long1 != 0);
    CHECK(long2 != 0);
    CHECK(long3 != 0);
  }

  TEST_CASE("compile_time_evaluation") {
    constexpr auto hash = ctti::fnv1a_hash("compile_time_test");
    static_assert(hash != 0);
    static_assert(hash == ctti::fnv1a_hash("compile_time_test"));

    // Verify it can be used in template parameters - use a small modulo to avoid large array
    constexpr auto array_size = (hash % 100) + 1;  // Ensure non-zero size
    std::array<int, array_size> arr;
    CHECK(arr.size() == array_size);
  }

  TEST_CASE("boundary_conditions") {
    // Test with null character in middle
    constexpr char null_in_middle[] = {'a', '\0', 'b', '\0'};
    constexpr auto hash_with_null = ctti::fnv1a_hash(std::string_view(null_in_middle, std::size(null_in_middle)));
    constexpr auto hash_without_null = ctti::fnv1a_hash("a");

    CHECK(hash_with_null != hash_without_null);

    // Test with only null character
    constexpr auto null_only = ctti::fnv1a_hash(std::string_view("\0", 1));
    CHECK(null_only != 0);
  }

  // Hash literal tests
  TEST_CASE("basic_hash_literal") {
    constexpr auto hash1 = "test"_sh;
    constexpr auto hash2 = "hello"_sh;
    constexpr auto hash3 = "test"_sh;

    CHECK(hash1 != 0);
    CHECK(hash2 != 0);
    CHECK(hash1 != hash2);
    CHECK(hash1 == hash3);
  }

  TEST_CASE("hash_literal_empty_string") {
    constexpr auto empty_hash = ""_sh;
    CHECK(empty_hash != 0);  // FNV-1a has a non-zero basis
  }

  TEST_CASE("hash_literal_different_strings") {
    constexpr auto hash_a = "a"_sh;
    constexpr auto hash_b = "b"_sh;
    constexpr auto hash_ab = "ab"_sh;
    constexpr auto hash_ba = "ba"_sh;

    CHECK(hash_a != hash_b);
    CHECK(hash_ab != hash_ba);
    CHECK(hash_a != hash_ab);
    CHECK(hash_b != hash_ab);
  }

  TEST_CASE("hash_literal_case_sensitivity") {
    constexpr auto lower = "hello"_sh;
    constexpr auto upper = "HELLO"_sh;
    constexpr auto mixed = "Hello"_sh;

    CHECK(lower != upper);
    CHECK(lower != mixed);
    CHECK(upper != mixed);
  }

  TEST_CASE("hash_literal_long_string") {
    constexpr auto long_hash = "this_is_a_very_long_string_for_testing_hash_function"_sh;
    CHECK(long_hash != 0);
  }

  TEST_CASE("hash_literal_special_characters") {
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

  TEST_CASE("hash_literal_numeric_strings") {
    constexpr auto num1 = "123"_sh;
    constexpr auto num2 = "456"_sh;
    constexpr auto num3 = "123456"_sh;

    CHECK(num1 != num2);
    CHECK(num1 != num3);
    CHECK(num2 != num3);
  }

  TEST_CASE("hash_literal_whitespace_sensitivity") {
    constexpr auto no_space = "helloworld"_sh;
    constexpr auto with_space = "hello world"_sh;
    constexpr auto with_tab = "hello\tworld"_sh;
    constexpr auto with_newline = "hello\nworld"_sh;

    CHECK(no_space != with_space);
    CHECK(with_space != with_tab);
    CHECK(with_tab != with_newline);
    CHECK(no_space != with_newline);
  }

  TEST_CASE("hash_literal_compile_time_evaluation") {
    constexpr auto hash = "compile_time"_sh;
    static_assert(hash != 0);
    static_assert(hash == "compile_time"_sh);

    // Use in template parameters
    std::array<int, hash % 100> arr;  // Should compile
    CHECK(arr.size() == hash % 100);
  }

  TEST_CASE("hash_literal_consistency_with_function") {
    constexpr auto literal_hash = "test_string"_sh;
    constexpr auto function_hash = ctti::fnv1a_hash("test_string");

    CHECK(literal_hash == function_hash);
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

  TEST_CASE("hash_type_consistency") {
    constexpr auto hash = ctti::fnv1a_hash("test");
    static_assert(std::same_as<decltype(hash), const std::uint64_t>);
  }
}
