#include "doctest.h"

#include <ctti/constructor.hpp>

#include <concepts>
#include <string>

namespace {

struct DefaultConstructible {
  DefaultConstructible() = default;
};

struct NonDefaultConstructible {
  explicit NonDefaultConstructible(int) {}
};

struct CopyConstructible {
  CopyConstructible() = default;
  CopyConstructible(const CopyConstructible&) = default;
};

struct NonCopyConstructible {
  NonCopyConstructible() = default;
  NonCopyConstructible(const NonCopyConstructible&) = delete;
  NonCopyConstructible(NonCopyConstructible&&) = default;
};

struct AggregateType {
  int a = 0;
  double b = 0.0;
  std::string c;
};

struct NonAggregateType {
  int a = 0;
  double b = 0.0;

private:
  std::string c;
};

}  // namespace

TEST_SUITE("constructor") {
  TEST_CASE("constructor_concepts") {
    CHECK(ctti::default_constructible<DefaultConstructible>);
    CHECK_FALSE(ctti::default_constructible<NonDefaultConstructible>);

    CHECK(ctti::copy_constructible<CopyConstructible>);
    CHECK_FALSE(ctti::copy_constructible<NonCopyConstructible>);

    CHECK(ctti::move_constructible<NonCopyConstructible>);

    CHECK(ctti::constructible<NonDefaultConstructible, int>);
    CHECK_FALSE(ctti::constructible<NonDefaultConstructible>);

    CHECK(ctti::aggregate_type<AggregateType>);
    CHECK_FALSE(ctti::aggregate_type<NonAggregateType>);
  }

  TEST_CASE("constructor_info_basic") {
    auto info = ctti::get_constructor_info<DefaultConstructible>();

    CHECK(info.is_default_constructible());
    CHECK(info.is_copy_constructible());
    CHECK(info.is_move_constructible());
    CHECK(info.can_construct<>());
    CHECK(info.can_construct_nothrow<>());
  }

  TEST_CASE("constructor_info_non_default") {
    auto info = ctti::get_constructor_info<NonDefaultConstructible>();

    CHECK_FALSE(info.is_default_constructible());
    CHECK(info.can_construct<int>());
    CHECK_FALSE(info.can_construct<>());
  }

  TEST_CASE("constructor_info_aggregate") {
    auto info = ctti::get_constructor_info<AggregateType>();

    CHECK(info.is_aggregate());
    CHECK(info.can_construct<int, double, std::string>());
    CHECK(info.can_construct<>());
  }

  TEST_CASE("construction_methods") {
    auto info = ctti::get_constructor_info<AggregateType>();

    auto obj1 = info.construct();
    auto obj2 = info.construct(1, 2.5, std::string("test"));

    CHECK_EQ(obj2.a, 1);
    CHECK_EQ(obj2.b, doctest::Approx(2.5));
    CHECK_EQ(obj2.c, "test");

    auto ptr1 = info.make_unique(3, 4.5, std::string("unique"));
    auto ptr2 = info.make_shared(5, 6.5, std::string("shared"));

    CHECK_EQ(ptr1->a, 3);
    CHECK_EQ(ptr2->a, 5);
  }

  TEST_CASE("constructor_signature") {
    using sig = ctti::constructor_signature<AggregateType, int, double, std::string>;

    CHECK_EQ(sig::arity, 3);
    CHECK(std::same_as<sig::arg_type<0>, int>);
    CHECK(std::same_as<sig::arg_type<1>, double>);
    CHECK(std::same_as<sig::arg_type<2>, std::string>);
    CHECK(sig::is_valid);
  }

  TEST_CASE("nothrow_construction") {
    auto info = ctti::get_constructor_info<int>();

    CHECK(info.can_construct_nothrow<>());
    CHECK(info.can_construct_nothrow<int>());

    static_assert(ctti::nothrow_constructible<int>);
    static_assert(ctti::nothrow_constructible<int, int>);
  }
}
