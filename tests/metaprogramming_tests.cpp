#include "catch.hpp"

#include <ctti/detail/meta.hpp>
#include <ctti/type_id.hpp>

#include <string>
#include <type_traits>

using namespace ctti::meta;

struct to_pointer {
  template <typename T>
  struct apply {
    using type = T*;
  };
};

struct is_integral_pred {
  template <typename T>
  struct apply {
    using type = bool_<std::is_integral_v<T>>;
  };
};

struct sum_sizes {
  template <typename Acc, typename T>
  struct apply {
    using type = ctti::meta::usize<value<Acc>() + sizeof(T)>;
  };
};

TEST_CASE("meta programming utilities") {
  SECTION("list_size") {
    using empty_list = list<>;
    using one_item_list = list<int>;
    using multiple_items_list = list<int, float, char>;

    REQUIRE(list_size<empty_list>() == 0);
    REQUIRE(list_size<one_item_list>() == 1);
    REQUIRE(list_size<multiple_items_list>() == 3);
  }

  SECTION("fmap") {
    using input = list<int, float, char>;
    using expected = list<int*, float*, char*>;
    using result = fmap_t<to_pointer, input>;

    // Use the "typeid" approach to verify type equality in runtime tests
    REQUIRE(ctti::type_id<result>() == ctti::type_id<expected>());
  }

  SECTION("filter") {
    using input = list<int, float, char, double, bool>;
    using expected = list<int, char, bool>;
    using result = filter_t<is_integral_pred, input>;

    REQUIRE(ctti::type_id<result>() == ctti::type_id<expected>());
  }

  SECTION("foldl") {
    using input = list<char, int, double>;  // 1 + 4 + 8 = 13 (assuming typical sizes)
    constexpr std::size_t result = value<foldl_t<sum_sizes, ctti::meta::usize<0>, input>>();

    REQUIRE(result == (sizeof(char) + sizeof(int) + sizeof(double)));
  }

  SECTION("split operations") {
    using input = list<char, int, float, double, bool>;

    using split_result = split<2, input>;
    using before = typename split_result::before;
    using head = typename split_result::head;
    using after = typename split_result::after;

    REQUIRE(list_size<before>() == 2);
    REQUIRE(ctti::type_id<head>() == ctti::type_id<float>());
    REQUIRE(list_size<after>() == 2);
  }
}
