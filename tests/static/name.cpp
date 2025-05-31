#include "static_test.hpp"

#include <ctti/name.hpp>

#include <string_view>

constexpr ctti::name_t hello_world{"hello::world"};
EXPECT_EQ(hello_world.name(), "world");
EXPECT_EQ(hello_world.full_name(), "hello::world");

constexpr ctti::name_t scoped_hello_world{"::hello::world"};
EXPECT_EQ(scoped_hello_world.name(), "world");
EXPECT_EQ(scoped_hello_world.full_name(), "::hello::world");

constexpr ctti::name_t world{"world"};
EXPECT_EQ(world.name(), "world");

constexpr ctti::name_t foo_bar_quux_foobar_foobarquux{"foo::bar::quux::foobar::foobarquux"};
EXPECT_EQ(foo_bar_quux_foobar_foobarquux.qualifier(0), "foo");
EXPECT_EQ(foo_bar_quux_foobar_foobarquux.qualifier(1), "bar");
EXPECT_EQ(foo_bar_quux_foobar_foobarquux.qualifier(2), "quux");
EXPECT_EQ(foo_bar_quux_foobar_foobarquux.qualifier(3), "foobar");
EXPECT_EQ(foo_bar_quux_foobar_foobarquux.name(), "foobarquux");
