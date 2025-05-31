#include "static_test.hpp"

#include <ctti/model.hpp>

CTTI_DEFINE_SYMBOL(i);
CTTI_DEFINE_SYMBOL(b);

struct Foo {
  int i = 0;
  bool b = false;

  using ctti_model = ctti::model<::i, ::b>;
};

EXPECT_TRUE(std::is_same<ctti::get_model<Foo>, ctti::model<i, b>>());
