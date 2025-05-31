#include "catch.hpp"

#include <ctti/tie.hpp>

CTTI_DEFINE_SYMBOL(a);
CTTI_DEFINE_SYMBOL(b);
CTTI_DEFINE_SYMBOL(c);

struct Struct {
  int a = 0;
  std::string b;
  bool c = false;
};

TEST_CASE("tie") {
  int var_a = 0;
  std::string var_b;
  bool var_c = false;
  Struct object{42, "foo", true};

  ctti::tie<a, b, c>(var_a, var_b, var_c) = object;

  REQUIRE(var_a == 42);
  REQUIRE(var_b == "foo");
  REQUIRE(var_c == true);
}
