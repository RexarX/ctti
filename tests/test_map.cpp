#include "doctest.h"

#include <ctti/map.hpp>
#include <ctti/symbol.hpp>

#include <iostream>
#include <string>

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

CTTI_DEFINE_SYMBOL(value);
CTTI_DEFINE_SYMBOL(name);
CTTI_DEFINE_SYMBOL(price);
CTTI_DEFINE_SYMBOL(active);

TEST_SUITE("map") {
  TEST_CASE("basic_mapping") {
    Source src;
    Sink dst;

    ctti::map<ctti_symbols::value, ctti_symbols::value>(src, dst);
    ctti::map<ctti_symbols::name, ctti_symbols::name>(src, dst);

    CHECK(dst.value == 42);
    CHECK(dst.name == "source");
    CHECK_FALSE(dst.active);  // Should remain unchanged
  }

  TEST_CASE("default_mapping_function") {
    Source src;
    Sink dst;

    ctti::default_symbol_mapping_function mapper;
    ctti::map<ctti_symbols::value, ctti_symbols::value>(src, dst, mapper);

    CHECK(dst.value == 42);
  }

  TEST_CASE("custom_mapping_function") {
    Source src;
    Sink dst;

    auto custom_mapper = [](const Source& source, ctti_symbols::value src_symbol, Sink& sink, ctti_symbols::value dst_symbol) {
      if constexpr (src_symbol.template is_owner_of<Source>() && dst_symbol.template is_owner_of<Sink>()) {
        ctti::set_member_value<ctti_symbols::value>(sink, ctti::get_member_value<ctti_symbols::value>(source) + 1000);
      }
    };

    ctti::map<ctti_symbols::value, ctti_symbols::value>(src, dst, custom_mapper);
    CHECK(dst.value == 1042);  // 42 + 1000
  }

  TEST_CASE("symbol_mapping_class") {
    Source src;
    Sink dst;

    auto mapping = ctti::make_mapping<ctti_symbols::value, ctti_symbols::value>();
    mapping(src, dst);

    CHECK(dst.value == 42);
  }

  TEST_CASE("custom_symbol_mapping") {
    Source src;
    Sink dst;

    auto custom_function = [](const Source& source, ctti_symbols::value, Sink& sink, ctti_symbols::active) { sink.active = (source.value > 0); };

    auto mapping = ctti::make_mapping<ctti_symbols::value, ctti_symbols::active>(custom_function);
    mapping(src, dst);

    CHECK(dst.active);
  }

  TEST_CASE("multiple_mappings") {
    Source src;
    Sink dst;

    auto value_mapping = ctti::make_mapping<ctti_symbols::value, ctti_symbols::value>();
    auto name_mapping = ctti::make_mapping<ctti_symbols::name, ctti_symbols::name>();
    auto custom_mapping = ctti::make_mapping<ctti_symbols::price, ctti_symbols::active>(
        [](const Source& source, ctti_symbols::price, Sink& sink, ctti_symbols::active) { sink.active = (source.price > 50.0); });

    ctti::map(src, dst, value_mapping, name_mapping, custom_mapping);

    CHECK(dst.value == 42);
    CHECK(dst.name == "source");
    CHECK(dst.active);  // price > 50.0
  }

  TEST_CASE("mapping_with_type_conversion") {
    struct SourceInt {
      int value = 42;
    };
    struct SinkDouble {
      double value = 0.0;
    };

    SourceInt src;
    SinkDouble dst;

    ctti::map<ctti_symbols::value, ctti_symbols::value>(src, dst);
    CHECK(dst.value == 42.0);
  }

  TEST_CASE("conditional_mapping") {
    Source src;
    Sink dst;

    auto conditional_mapper = [](const Source& source, ctti_symbols::value src_symbol, Sink& sink, ctti_symbols::value dst_symbol) {
      if constexpr (src_symbol.template is_owner_of<Source>() && dst_symbol.template is_owner_of<Sink>()) {
        auto src_value = ctti::get_member_value<ctti_symbols::value>(source);
        if (src_value > 40) {
          ctti::set_member_value<ctti_symbols::value>(sink, src_value);
        }
      }
    };

    ctti::map<ctti_symbols::value, ctti_symbols::value>(src, dst, conditional_mapper);
    CHECK(dst.value == 42);

    src.value = 30;
    dst.value = 0;
    ctti::map<ctti_symbols::value, ctti_symbols::value>(src, dst, conditional_mapper);
    CHECK(dst.value == 0);  // Condition not met, should remain 0
  }
}
