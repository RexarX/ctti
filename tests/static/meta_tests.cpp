#include "static_test.hpp"

#include <ctti/detail/meta.hpp>

using namespace ctti::meta;

// Test type_t
using int_identity = identity<int>;
EXPECT_TRUE((std::is_same_v<type_t<int_identity>, int>));

// Test list
using int_float_list = list<int, float>;
EXPECT_TRUE((std::is_same_v<pack_get_t<0, int, float>, int>));
EXPECT_TRUE((std::is_same_v<pack_get_t<1, int, float>, float>));
EXPECT_TRUE((std::is_same_v<head_t<int_float_list>, int>));
EXPECT_TRUE((std::is_same_v<get_t<0, int_float_list>, int>));

// Test logical operations
EXPECT_TRUE((value<and_t<true_, true_>>() == true));
EXPECT_TRUE((value<and_t<true_, false_>>() == false));
EXPECT_TRUE((value<or_t<true_, false_>>() == true));
EXPECT_TRUE((value<or_t<false_, false_>>() == false));
EXPECT_TRUE((value<not_t<false_>>() == true));
EXPECT_TRUE((value<not_t<true_>>() == false));

// Test arithmetic
EXPECT_TRUE((value<add_t<ctti::meta::int32<5>, ctti::meta::int32<3>>>() == 8));
EXPECT_TRUE((value<subtract_t<ctti::meta::int32<5>, ctti::meta::int32<3>>>() == 2));
EXPECT_TRUE((value<multiply_t<ctti::meta::int32<5>, ctti::meta::int32<3>>>() == 15));

// Test cat_t
using list1 = list<int, float>;
using list2 = list<char, double>;
using concatenated = cat_t<list1, list2>;
EXPECT_TRUE((std::is_same_v<concatenated, list<int, float, char, double>>));

// Test filter_t
struct is_integral {
  template <typename T>
  struct apply {
    using type = bool_<std::is_integral_v<T>>;
  };
};

using mixed_list = list<int, float, char, double>;
using filtered_list = filter_t<is_integral, mixed_list>;
EXPECT_TRUE((std::is_same_v<filtered_list, list<int, char>>));

// Test sort_t
struct size_compare {
  template <typename A, typename B>
  struct apply {
    using type = bool_<(sizeof(A) < sizeof(B))>;
  };
};

using unsorted = list<double, char, int, long long>;
using sorted = sort_t<unsorted, size_compare>;
// This assumes typical size relationships
EXPECT_TRUE((std::is_same_v<get_t<0, sorted>, char>));
