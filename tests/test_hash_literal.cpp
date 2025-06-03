#include "doctest.h"

#include <ctti/hash_literal.hpp>

#include <iostream>

TEST_SUITE("hash_literal") {
  TEST_CASE("basic_hash_literal") {
    constexpr auto hash1 = "test"_sh;
    constexpr auto hash2 = "hello"_sh;
    constexpr auto hash3 = "test"_sh;

    CHECK(hash1 != 0);
    CHECK(hash2 != 0);
    CHECK(hash1 != hash2);
    CHECK(hash1 == hash3);
  }

  TEST_CASE("empty_string_hash") {
    constexpr auto empty_hash = ""_sh;
    CHECK(empty_hash != 0);  // FNV-1a has a non-zero basis
  }

  TEST_CASE("different_strings_different_hashes") {
    constexpr auto hash_a = "a"_sh;
    constexpr auto hash_b = "b"_sh;
    constexpr auto hash_ab = "ab"_sh;
    constexpr auto hash_ba = "ba"_sh;

    CHECK(hash_a != hash_b);
    CHECK(hash_ab != hash_ba);
    CHECK(hash_a != hash_ab);
    CHECK(hash_b != hash_ab);
  }

  TEST_CASE("case_sensitivity") {
    constexpr auto lower = "hello"_sh;
    constexpr auto upper = "HELLO"_sh;
    constexpr auto mixed = "Hello"_sh;

    CHECK(lower != upper);
    CHECK(lower != mixed);
    CHECK(upper != mixed);
  }

  TEST_CASE("long_string_hash") {
    constexpr auto long_hash = "this_is_a_very_long_string_for_testing_hash_function"_sh;
    CHECK(long_hash != 0);
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

  TEST_CASE("numeric_strings") {
    constexpr auto num1 = "123"_sh;
    constexpr auto num2 = "456"_sh;
    constexpr auto num3 = "123456"_sh;

    CHECK(num1 != num2);
    CHECK(num1 != num3);
    CHECK(num2 != num3);
  }

  TEST_CASE("whitespace_sensitivity") {
    constexpr auto no_space = "helloworld"_sh;
    constexpr auto with_space = "hello world"_sh;
    constexpr auto with_tab = "hello\tworld"_sh;
    constexpr auto with_newline = "hello\nworld"_sh;

    CHECK(no_space != with_space);
    CHECK(with_space != with_tab);
    CHECK(with_tab != with_newline);
    CHECK(no_space != with_newline);
  }

  TEST_CASE("compile_time_evaluation") {
    constexpr auto hash = "compile_time"_sh;
    static_assert(hash != 0);
    static_assert(hash == "compile_time"_sh);

    // Use in template parameters
    std::array<int, hash % 100> arr;  // Should compile
    CHECK(arr.size() == hash % 100);
  }

  TEST_CASE("consistency_with_manual_hash") {
    constexpr auto literal_hash = "test_string"_sh;
    constexpr auto manual_hash = ctti::detail::Fnv1aHash("test_string");

    CHECK(literal_hash == manual_hash);
  }
}
