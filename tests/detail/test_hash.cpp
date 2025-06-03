#include "doctest.h"

#include <ctti/detail/hash.hpp>

#include <array>
#include <iostream>
#include <string_view>

TEST_SUITE("detail::hash") {
  TEST_CASE("basic_fnv1a_hash") {
    constexpr auto hash1 = ctti::detail::Fnv1aHash("test");
    constexpr auto hash2 = ctti::detail::Fnv1aHash("hello");
    constexpr auto hash3 = ctti::detail::Fnv1aHash("test");

    CHECK(hash1 != 0);
    CHECK(hash2 != 0);
    CHECK(hash1 != hash2);
    CHECK(hash1 == hash3);
  }

  TEST_CASE("empty_string_hash") {
    constexpr auto empty_hash = ctti::detail::Fnv1aHash("");
    CHECK(empty_hash == ctti::detail::kFnvBasis);
  }

  TEST_CASE("single_character_hash") {
    constexpr auto hash_a = ctti::detail::Fnv1aHash("a");
    constexpr auto hash_b = ctti::detail::Fnv1aHash("b");

    CHECK(hash_a != hash_b);
    CHECK(hash_a != ctti::detail::kFnvBasis);
    CHECK(hash_b != ctti::detail::kFnvBasis);
  }

  TEST_CASE("hash_with_custom_basis") {
    constexpr auto custom_basis = 12345ull;
    constexpr auto hash1 = ctti::detail::Fnv1aHash("test", custom_basis);
    constexpr auto hash2 = ctti::detail::Fnv1aHash("test", ctti::detail::kFnvBasis);

    CHECK(hash1 != hash2);
    CHECK(hash1 != 0);
    CHECK(hash2 != 0);
  }

  TEST_CASE("recursive_hash_function") {
    constexpr char test_str[] = "hello";
    constexpr auto recursive_hash = ctti::detail::Fnv1aHash(5, test_str);
    constexpr auto string_view_hash = ctti::detail::Fnv1aHash(std::string_view(test_str));

    CHECK(recursive_hash == string_view_hash);
  }

  TEST_CASE("array_hash") {
    constexpr char test_array[] = "test_string";
    constexpr auto array_hash = ctti::detail::Fnv1aHash(test_array);
    constexpr auto string_view_hash = ctti::detail::Fnv1aHash(std::string_view("test_string"));

    CHECK(array_hash == string_view_hash);
  }

  TEST_CASE("zero_length_recursive") {
    constexpr char test_str[] = "hello";
    constexpr auto zero_length_hash = ctti::detail::Fnv1aHash(0, test_str);

    CHECK(zero_length_hash == ctti::detail::kFnvBasis);
  }

  TEST_CASE("different_lengths_same_prefix") {
    constexpr auto hash1 = ctti::detail::Fnv1aHash("test");
    constexpr auto hash2 = ctti::detail::Fnv1aHash("testing");
    constexpr auto hash3 = ctti::detail::Fnv1aHash("te");

    CHECK(hash1 != hash2);
    CHECK(hash1 != hash3);
    CHECK(hash2 != hash3);
  }

  TEST_CASE("case_sensitivity") {
    constexpr auto lower = ctti::detail::Fnv1aHash("hello");
    constexpr auto upper = ctti::detail::Fnv1aHash("HELLO");
    constexpr auto mixed = ctti::detail::Fnv1aHash("Hello");

    CHECK(lower != upper);
    CHECK(lower != mixed);
    CHECK(upper != mixed);
  }

  TEST_CASE("special_characters") {
    constexpr auto space_hash = ctti::detail::Fnv1aHash("hello world");
    constexpr auto tab_hash = ctti::detail::Fnv1aHash("hello\tworld");
    constexpr auto newline_hash = ctti::detail::Fnv1aHash("hello\nworld");
    constexpr auto symbol_hash = ctti::detail::Fnv1aHash("hello@world.com");

    CHECK(space_hash != tab_hash);
    CHECK(tab_hash != newline_hash);
    CHECK(newline_hash != symbol_hash);
    CHECK(space_hash != symbol_hash);
  }

  TEST_CASE("numeric_strings") {
    constexpr auto num1 = ctti::detail::Fnv1aHash("123");
    constexpr auto num2 = ctti::detail::Fnv1aHash("456");
    constexpr auto num3 = ctti::detail::Fnv1aHash("0");
    constexpr auto negative = ctti::detail::Fnv1aHash("-123");

    CHECK(num1 != num2);
    CHECK(num2 != num3);
    CHECK(num1 != negative);
    CHECK(num3 != negative);
  }

  TEST_CASE("long_strings") {
    constexpr auto long1 =
        ctti::detail::Fnv1aHash("this_is_a_very_long_string_for_testing_the_hash_function_with_many_characters");
    constexpr auto long2 = ctti::detail::Fnv1aHash(
        "this_is_a_very_long_string_for_testing_the_hash_function_with_many_character");  // One char less
    constexpr auto long3 = ctti::detail::Fnv1aHash(
        "this_is_a_very_long_string_for_testing_the_hash_function_with_many_characterX");  // Different last char

    CHECK(long1 != long2);
    CHECK(long1 != long3);
    CHECK(long2 != long3);
    CHECK(long1 != 0);
    CHECK(long2 != 0);
    CHECK(long3 != 0);
  }

  TEST_CASE("fnv_constants") {
    CHECK(ctti::detail::kFnvBasis == 14695981039346656037ull);
    CHECK(ctti::detail::kFnvPrime == 1099511628211ull);
  }

  TEST_CASE("hash_type_consistency") {
    constexpr auto hash = ctti::detail::Fnv1aHash("test");
    static_assert(std::same_as<decltype(hash), const ctti::detail::HashType>);
    static_assert(std::same_as<ctti::detail::HashType, std::uint64_t>);
  }

  TEST_CASE("compile_time_evaluation") {
    constexpr auto hash = ctti::detail::Fnv1aHash("compile_time_test");
    static_assert(hash != 0);
    static_assert(hash == ctti::detail::Fnv1aHash("compile_time_test"));

    // Verify it can be used in template parameters - use a small modulo to avoid large array
    constexpr auto array_size = (hash % 100) + 1;  // Ensure non-zero size
    std::array<int, array_size> arr;
    CHECK(arr.size() == array_size);
  }

  TEST_CASE("boundary_conditions") {
    // Test with null character in middle
    constexpr char null_in_middle[] = {'a', '\0', 'b', '\0'};
    constexpr auto hash_with_null = ctti::detail::Fnv1aHash(std::string_view(null_in_middle, 3));
    constexpr auto hash_without_null = ctti::detail::Fnv1aHash("a");

    CHECK(hash_with_null != hash_without_null);

    // Test with only null character
    constexpr auto null_only = ctti::detail::Fnv1aHash(std::string_view("\0", 1));
    CHECK(null_only != ctti::detail::kFnvBasis);
  }
}
