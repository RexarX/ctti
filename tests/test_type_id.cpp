#include "doctest.h"

#include <ctti/type_id.hpp>

#include <iostream>
#include <string>
#include <unordered_map>

struct CustomStruct {};
enum class CustomEnum { A, B };

TEST_SUITE("type_id") {
  TEST_CASE("basic_type_id") {
    auto int_id = ctti::type_id_of<int>();
    auto string_id = ctti::type_id_of<std::string>();
    auto custom_id = ctti::type_id_of<CustomStruct>();

    CHECK(int_id.name() == "int");
    CHECK(string_id.name() == "std::string");
    CHECK(custom_id.name().find("CustomStruct") != std::string_view::npos);

    CHECK(int_id.hash() != 0);
    CHECK(string_id.hash() != int_id.hash());
    CHECK(custom_id.hash() != int_id.hash());
  }

  TEST_CASE("type_id_from_object") {
    int val = 42;
    std::string str = "test";
    CustomStruct custom;

    auto int_id = ctti::type_id_of(val);
    auto string_id = ctti::type_id_of(str);
    auto custom_id = ctti::type_id_of(custom);

    CHECK(int_id.name() == "int");
    CHECK(string_id.name() == "std::string");
    CHECK(custom_id.name().find("CustomStruct") != std::string_view::npos);
  }

  TEST_CASE("type_id_equality") {
    auto id1 = ctti::type_id_of<int>();
    auto id2 = ctti::type_id_of<int>();
    auto id3 = ctti::type_id_of<double>();

    CHECK(id1 == id2);
    CHECK_FALSE(id1 == id3);
    CHECK(id1 != id3);
  }

  TEST_CASE("type_id_ordering") {
    auto int_id = ctti::type_id_of<int>();
    auto double_id = ctti::type_id_of<double>();

    // Ordering should be consistent
    auto cmp1 = int_id <=> double_id;
    auto cmp2 = double_id <=> int_id;
    auto cmp3 = int_id <=> int_id;

    CHECK(cmp3 == std::strong_ordering::equal);
    CHECK((cmp1 == std::strong_ordering::less) == (cmp2 == std::strong_ordering::greater));
  }

  TEST_CASE("type_index") {
    auto int_index = ctti::type_index_of<int>();
    auto string_index = ctti::type_index_of<std::string>();

    CHECK(int_index.hash() != 0);
    CHECK(string_index.hash() != int_index.hash());

    CHECK(int_index == int_index);
    CHECK_FALSE(int_index == string_index);
  }

  TEST_CASE("type_index_from_object") {
    int val = 42;
    std::string str = "test";

    auto int_index = ctti::type_index_of(val);
    auto string_index = ctti::type_index_of(str);

    CHECK(int_index != string_index);
    CHECK(int_index == ctti::type_index_of<int>());
    CHECK(string_index == ctti::type_index_of<std::string>());
  }

  TEST_CASE("type_index_from_type_id") {
    auto type_id = ctti::type_id_of<int>();
    auto type_index = ctti::type_index{type_id};
    auto direct_index = ctti::type_index_of<int>();

    CHECK(type_index == direct_index);
    CHECK(type_index.hash() == type_id.hash());
  }

  TEST_CASE("id_from_name") {
    auto index1 = ctti::id_from_name("int");
    auto index2 = ctti::id_from_name("double");
    auto index3 = ctti::id_from_name("int");

    CHECK(index1 == index3);
    CHECK_FALSE(index1 == index2);
    CHECK(index1.hash() != 0);
  }

  TEST_CASE("type_id_in_container") {
    std::unordered_map<ctti::type_id, std::string> type_map;

    type_map[ctti::type_id_of<int>()] = "integer";
    type_map[ctti::type_id_of<double>()] = "floating";
    type_map[ctti::type_id_of<std::string>()] = "string";

    CHECK(type_map.size() == 3);
    CHECK(type_map[ctti::type_id_of<int>()] == "integer");
    CHECK(type_map[ctti::type_id_of<double>()] == "floating");
    CHECK(type_map[ctti::type_id_of<std::string>()] == "string");
  }

  TEST_CASE("type_index_in_container") {
    std::unordered_map<ctti::type_index, std::string> index_map;

    index_map[ctti::type_index_of<int>()] = "integer";
    index_map[ctti::type_index_of<double>()] = "floating";
    index_map[ctti::type_index_of<CustomStruct>()] = "custom";

    CHECK(index_map.size() == 3);
    CHECK(index_map[ctti::type_index_of<int>()] == "integer");
    CHECK(index_map[ctti::type_index_of<double>()] == "floating");
    CHECK(index_map[ctti::type_index_of<CustomStruct>()] == "custom");
  }

  TEST_CASE("enum_type_id") {
    auto enum_id = ctti::type_id_of<CustomEnum>();
    auto enum_index = ctti::type_index_of<CustomEnum>();

    CHECK(enum_id.name().find("CustomEnum") != std::string_view::npos);
    CHECK(enum_index.hash() != 0);
    CHECK(enum_index.hash() == enum_id.hash());
  }

  TEST_CASE("cv_qualified_types") {
    auto const_id = ctti::type_id_of<const int>();
    auto volatile_id = ctti::type_id_of<volatile int>();
    auto plain_id = ctti::type_id_of<int>();

    CHECK(const_id != plain_id);
    CHECK(volatile_id != plain_id);
    CHECK(const_id != volatile_id);
  }

  TEST_CASE("pointer_types") {
    auto ptr_id = ctti::type_id_of<int*>();
    auto ref_id = ctti::type_id_of<int&>();
    auto value_id = ctti::type_id_of<int>();

    CHECK(ptr_id != value_id);
    CHECK(ref_id != value_id);
    CHECK(ptr_id != ref_id);
  }
}
