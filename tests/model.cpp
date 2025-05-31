#include "catch.hpp"

#include <ctti/model.hpp>
#include <ctti/nameof.hpp>

CTTI_DEFINE_SYMBOL(i);
CTTI_DEFINE_SYMBOL(b);

struct Foo {
  int i = 0;
  bool b = false;

  using ctti_model = ctti::model<::i, ::b>;
};

struct Bar {
  int i = 0;
  bool b = false;
};

ctti::model<i, b> ctti_model(ctti::type_tag<Bar>);

TEST_CASE("model") {
  REQUIRE(ctti::nameof<ctti::get_model<Foo>>() == ctti::nameof<ctti::model<i, b>>());
  REQUIRE(ctti::nameof<ctti::get_model<Bar>>() == ctti::nameof<ctti::model<i, b>>());
}
