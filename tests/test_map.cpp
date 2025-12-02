#include "doctest.h"

#include <ctti/map.hpp>
#include <ctti/symbol.hpp>

#include <string>

namespace {

struct Source {
  int value = 42;
  std::string name = "source";
  double price = 99.99;
};

struct Sink {
  int value = 0;
  std::string name = "sink";
  bool active = false;
};

}  // namespace

TEST_SUITE("map") {
  TEST_CASE("basic_mapping") {
    Source src;
    Sink dst;

    constexpr auto value_src_symbol = ctti::make_simple_symbol<"value", &Source::value>();
    constexpr auto value_dst_symbol = ctti::make_simple_symbol<"value", &Sink::value>();
    constexpr auto name_src_symbol = ctti::make_simple_symbol<"name", &Source::name>();
    constexpr auto name_dst_symbol = ctti::make_simple_symbol<"name", &Sink::name>();

    ctti::map<value_src_symbol, value_dst_symbol>(src, dst);
    ctti::map<name_src_symbol, name_dst_symbol>(src, dst);

    CHECK_EQ(dst.value, 42);
    CHECK_EQ(dst.name, "source");
    CHECK_FALSE(dst.active);  // Should remain unchanged
  }

  TEST_CASE("custom_mapping_function") {
    Source src;
    Sink dst;

    constexpr auto value_src_symbol = ctti::make_simple_symbol<"value", &Source::value>();
    constexpr auto value_dst_symbol = ctti::make_simple_symbol<"value", &Sink::value>();

    auto custom_mapper = [](const Source& source, auto src_symbol, Sink& sink, auto dst_symbol) {
      if constexpr (src_symbol.template is_owner_of<Source>() && dst_symbol.template is_owner_of<Sink>()) {
        auto src_member = src_symbol.template get_member<Source>();
        auto dst_member = dst_symbol.template get_member<Sink>();
        sink.*dst_member = source.*src_member + 1000;
      }
    };

    ctti::map<value_src_symbol, value_dst_symbol>(src, dst, custom_mapper);
    CHECK_EQ(dst.value, 1042);  // 42 + 1000
  }

  TEST_CASE("symbol_mapping_class") {
    Source src;
    Sink dst;

    constexpr auto value_src_symbol = ctti::make_simple_symbol<"value", &Source::value>();
    constexpr auto value_dst_symbol = ctti::make_simple_symbol<"value", &Sink::value>();

    auto mapping = ctti::make_mapping<value_src_symbol, value_dst_symbol>();
    mapping(src, dst);

    CHECK_EQ(dst.value, 42);
  }

  TEST_CASE("multiple_mappings") {
    Source src;
    Sink dst;

    constexpr auto value_src_symbol = ctti::make_simple_symbol<"value", &Source::value>();
    constexpr auto value_dst_symbol = ctti::make_simple_symbol<"value", &Sink::value>();
    constexpr auto name_src_symbol = ctti::make_simple_symbol<"name", &Source::name>();
    constexpr auto name_dst_symbol = ctti::make_simple_symbol<"name", &Sink::name>();
    constexpr auto price_src_symbol = ctti::make_simple_symbol<"price", &Source::price>();
    constexpr auto active_dst_symbol = ctti::make_simple_symbol<"active", &Sink::active>();

    auto value_mapping = ctti::make_mapping<value_src_symbol, value_dst_symbol>();
    auto name_mapping = ctti::make_mapping<name_src_symbol, name_dst_symbol>();
    auto custom_mapping = ctti::make_mapping<price_src_symbol, active_dst_symbol>(
        [](const Source& source, auto src_sym, Sink& sink, auto dst_sym) {
          auto price_member = src_sym.template get_member<Source>();
          auto active_member = dst_sym.template get_member<Sink>();
          sink.*active_member = source.*price_member > 50.0;
        });

    ctti::map(src, dst, value_mapping, name_mapping, custom_mapping);

    CHECK_EQ(dst.value, 42);
    CHECK_EQ(dst.name, "source");
    CHECK(dst.active);  // price > 50.0
  }
}
