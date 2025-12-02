#include "doctest.h"

#include <ctti/detail/meta.hpp>

#include <concepts>
#include <string>
#include <tuple>
#include <type_traits>

namespace {

template <typename T>
struct is_floating_point_predicate : std::bool_constant<std::is_floating_point_v<T>> {};

template <typename T>
struct is_integral_predicate : std::bool_constant<std::is_integral_v<T>> {};

}  // namespace

TEST_SUITE("detail::meta") {
  TEST_CASE("basic_type_aliases") {
    CHECK(std::same_as<ctti::detail::DecayType<const int&>, int>);
    CHECK(std::same_as<ctti::detail::DecayType<volatile double&&>, double>);

    CHECK(std::same_as<ctti::detail::VoidType<int, double, char>, void>);
    CHECK(std::same_as<ctti::detail::VoidType<>, void>);

    CHECK(std::same_as<ctti::detail::Identity<int>::type, int>);
    CHECK(std::same_as<ctti::detail::TypeOf<ctti::detail::Identity<double>>, double>);
  }

  TEST_CASE("integral_constants") {
    CHECK_EQ(ctti::detail::UInt8<42>::value, 42);
    CHECK_EQ(ctti::detail::UInt16<1000>::value, 1000);
    CHECK_EQ(ctti::detail::UInt32<100000>::value, 100000);
    CHECK_EQ(ctti::detail::UInt64<1000000000000ULL>::value, 1000000000000ULL);

    CHECK_EQ(ctti::detail::Int8<-42>::value, -42);
    CHECK_EQ(ctti::detail::Int16<-1000>::value, -1000);
    CHECK_EQ(ctti::detail::Int32<-100000>::value, -100000);
    CHECK_EQ(ctti::detail::Int64<-1000000000000LL>::value, -1000000000000LL);

    CHECK_EQ(ctti::detail::SizeType<123>::value, 123);
    CHECK_EQ(ctti::detail::BoolType<true>::value, true);
    CHECK_EQ(ctti::detail::BoolType<false>::value, false);
    CHECK_EQ(ctti::detail::CharType<'A'>::value, 'A');
  }

  TEST_CASE("integral_constant_concept") {
    CHECK(ctti::detail::IntegralConstantType<ctti::detail::UInt32<42>>);
    CHECK(ctti::detail::IntegralConstantType<std::integral_constant<int, 123>>);
    CHECK(ctti::detail::IntegralConstantType<std::true_type>);
    CHECK(ctti::detail::IntegralConstantType<std::false_type>);
    CHECK_FALSE(ctti::detail::IntegralConstantType<int>);
    CHECK_FALSE(ctti::detail::IntegralConstantType<std::string>);
  }

  TEST_CASE("get_value") {
    constexpr auto val1 = ctti::detail::GetValue<ctti::detail::UInt32<42>>();
    constexpr auto val2 = ctti::detail::GetValue<ctti::detail::BoolType<true>>();
    constexpr auto val3 = ctti::detail::GetValue<ctti::detail::CharType<'X'>>();

    CHECK_EQ(val1, 42);
    CHECK_EQ(val2, true);
    CHECK_EQ(val3, 'X');
  }

  TEST_CASE("type_list_basic") {
    using list = ctti::detail::TypeList<int, double, std::string>;

    CHECK_EQ(list::kSize, 3);
    CHECK(std::same_as<list::At<0>, int>);
    CHECK(std::same_as<list::At<1>, double>);
    CHECK(std::same_as<list::At<2>, std::string>);
  }

  TEST_CASE("type_list_predicates") {
    using list = ctti::detail::TypeList<int, double, float>;

    CHECK_EQ(list::kSize, 3);

    // Test individual type properties
    CHECK(std::is_floating_point_v<double>);
    CHECK(std::is_floating_point_v<float>);
    CHECK(std::is_integral_v<int>);
    CHECK_FALSE(std::is_floating_point_v<int>);
    CHECK_FALSE(std::is_integral_v<double>);
  }

  TEST_CASE("type_list_for_each") {
    using list = ctti::detail::TypeList<int, double, char>;

    int count = 0;
    list::ForEach([&count](auto identity) { ++count; });

    CHECK_EQ(count, 3);
  }

  TEST_CASE("empty_type_list") {
    using empty_list = ctti::detail::TypeList<>;

    CHECK_EQ(empty_list::kSize, 0);

    int count = 0;
    empty_list::ForEach([&count](auto) { ++count; });
    CHECK_EQ(count, 0);
  }

  TEST_CASE("pack_get_type") {
    CHECK(std::same_as<ctti::detail::PackGetType<0, int, double, char>, int>);
    CHECK(std::same_as<ctti::detail::PackGetType<1, int, double, char>, double>);
    CHECK(std::same_as<ctti::detail::PackGetType<2, int, double, char>, char>);
  }

  TEST_CASE("list_size") {
    using list1 = ctti::detail::TypeList<int, double>;
    using list2 = ctti::detail::TypeList<>;
    using list3 = std::tuple<int, double, char>;

    CHECK_EQ(ctti::detail::ListSize<list1>::value, 2);
    CHECK_EQ(ctti::detail::ListSize<list2>::value, 0);
    CHECK_EQ(ctti::detail::ListSize<list3>::value, 3);
  }

  TEST_CASE("pair") {
    using pair = ctti::detail::Pair<int, std::string>;

    CHECK(std::same_as<ctti::detail::KeyType<pair>, int>);
    CHECK(std::same_as<ctti::detail::ValueType<pair>, std::string>);

    constexpr auto created_pair = pair::Make(42, std::string("test"));
    CHECK(std::same_as<decltype(created_pair), const pair>);
  }

  TEST_CASE("inherit") {
    struct A {
      int a = 0;
    };
    struct B {
      double b = 0.0;
    };
    struct C {
      char c = 'a';
    };

    using inherited = ctti::detail::Inherit<A, B, C>;

    inherited obj;
    obj.a = 1;
    obj.b = 2.0;
    obj.c = 'x';

    CHECK_EQ(obj.a, 1);
    CHECK_EQ(obj.b, 2.0);
    CHECK_EQ(obj.c, 'x');
  }

  TEST_CASE("inherit_from_type_list") {
    struct A {
      int a = 0;
    };
    struct B {
      double b = 0.0;
    };

    using list = ctti::detail::TypeList<A, B>;
    using inherited = ctti::detail::Inherit<list>;

    inherited obj;
    obj.a = 42;
    obj.b = 3.14;

    CHECK_EQ(obj.a, 42);
    CHECK_EQ(obj.b, doctest::Approx(3.14));
  }

  TEST_CASE("cat") {
    using list1 = ctti::detail::TypeList<int, double>;
    using list2 = ctti::detail::TypeList<char, bool>;
    using concatenated = ctti::detail::CatType<list1, list2>;

    CHECK_EQ(concatenated::kSize, 4);
    CHECK(std::same_as<concatenated::At<0>, int>);
    CHECK(std::same_as<concatenated::At<1>, double>);
    CHECK(std::same_as<concatenated::At<2>, char>);
    CHECK(std::same_as<concatenated::At<3>, bool>);
  }

  TEST_CASE("contains") {
    using list = ctti::detail::TypeList<int, double, std::string>;

    CHECK(ctti::detail::kContains<int, list>);
    CHECK(ctti::detail::kContains<double, list>);
    CHECK(ctti::detail::kContains<std::string, list>);
    CHECK_FALSE(ctti::detail::kContains<char, list>);
    CHECK_FALSE(ctti::detail::kContains<float, list>);
  }

  TEST_CASE("index_types") {
    using idx0 = ctti::detail::IndexType<0>;
    using idx1 = ctti::detail::IndexType<1>;
    using idx2 = ctti::detail::IndexType<2>;

    CHECK_EQ(idx0::value, 0);
    CHECK_EQ(idx1::value, 1);
    CHECK_EQ(idx2::value, 2);
  }

  TEST_CASE("integer_sequence") {
    using seq = ctti::detail::IntegerSequence<int, 1, 2, 3>;

    CHECK_EQ(seq::kSize, 3);
    CHECK(std::same_as<seq::At<0>, std::integral_constant<int, 1>>);
    CHECK(std::same_as<seq::At<1>, std::integral_constant<int, 2>>);
    CHECK(std::same_as<seq::At<2>, std::integral_constant<int, 3>>);
  }

  TEST_CASE("index_sequence") {
    using seq = ctti::detail::IndexSequence<0, 1, 2>;

    CHECK_EQ(seq::kSize, 3);
    CHECK(std::same_as<seq::At<0>, ctti::detail::IndexType<0>>);
    CHECK(std::same_as<seq::At<1>, ctti::detail::IndexType<1>>);
    CHECK(std::same_as<seq::At<2>, ctti::detail::IndexType<2>>);
  }

  TEST_CASE("string_type") {
    using str = ctti::detail::String<'h', 'i'>;

    CHECK_EQ(str::kSize, 2);
    CHECK(std::same_as<str::At<0>, std::integral_constant<char, 'h'>>);
    CHECK(std::same_as<str::At<1>, std::integral_constant<char, 'i'>>);
  }

  TEST_CASE("all_true_concept") {
    CHECK(ctti::detail::AllTrue<std::true_type, std::true_type, std::true_type>);
    CHECK_FALSE(ctti::detail::AllTrue<std::true_type, std::false_type, std::true_type>);
    CHECK_FALSE(ctti::detail::AllTrue<std::false_type, std::false_type, std::false_type>);
    CHECK(ctti::detail::AllTrue<>);  // Empty pack is true
  }
}
