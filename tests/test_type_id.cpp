#include "doctest.h"

#include <ctti/type_id.hpp>

#include <string>
#include <unordered_map>

namespace {

struct CustomStruct {};
enum class CustomEnum { A, B };

}  // namespace

TEST_SUITE("type_id") {
  TEST_CASE("basic_type_id") {
    auto int_id = ctti::type_id_of<int>();
    auto string_id = ctti::type_id_of<std::string>();
    auto custom_id = ctti::type_id_of<CustomStruct>();

    CHECK_EQ(int_id.name(), "int");
    CHECK_EQ(string_id.name(), "std::string");
    CHECK_EQ(custom_id.name(), "CustomStruct");

    CHECK_NE(int_id.hash(), 0);
    CHECK_NE(string_id.hash(), int_id.hash());
    CHECK_NE(custom_id.hash(), int_id.hash());
  }

  TEST_CASE("type_id_from_object") {
    int val = 42;
    std::string str = "test";
    CustomStruct custom;

    auto int_id = ctti::type_id_of(val);
    auto string_id = ctti::type_id_of(str);
    auto custom_id = ctti::type_id_of(custom);

    CHECK_EQ(int_id.name(), "int");
    CHECK_EQ(string_id.name(), "std::string");
    CHECK_EQ(custom_id.name(), "CustomStruct");
  }

  TEST_CASE("type_id_equality") {
    auto id1 = ctti::type_id_of<int>();
    auto id2 = ctti::type_id_of<int>();
    auto id3 = ctti::type_id_of<double>();

    CHECK_EQ(id1, id2);
    CHECK_NE(id1, id3);
  }

  TEST_CASE("type_id_ordering") {
    auto int_id = ctti::type_id_of<int>();
    auto double_id = ctti::type_id_of<double>();

    // Ordering should be consistent
    auto cmp1 = int_id <=> double_id;
    auto cmp2 = double_id <=> int_id;
    auto cmp3 = int_id <=> int_id;

    CHECK_EQ(cmp3, std::strong_ordering::equal);
    CHECK_EQ((cmp1 == std::strong_ordering::less), (cmp2 == std::strong_ordering::greater));
  }

  TEST_CASE("type_index") {
    auto int_index = ctti::type_index_of<int>();
    auto string_index = ctti::type_index_of<std::string>();

    CHECK_NE(int_index.hash(), 0);
    CHECK_NE(string_index.hash(), int_index.hash());

    CHECK_EQ(int_index, int_index);
    CHECK_NE(int_index, string_index);
  }

  TEST_CASE("type_index_from_object") {
    int val = 42;
    std::string str = "test";

    auto int_index = ctti::type_index_of(val);
    auto string_index = ctti::type_index_of(str);

    CHECK_NE(int_index, string_index);
    CHECK_EQ(int_index, ctti::type_index_of<int>());
    CHECK_EQ(string_index, ctti::type_index_of<std::string>());
  }

  TEST_CASE("type_index_from_type_id") {
    auto type_id = ctti::type_id_of<int>();
    auto type_index = ctti::type_index{type_id};
    auto direct_index = ctti::type_index_of<int>();

    CHECK_EQ(type_index, direct_index);
    CHECK_EQ(type_index.hash(), type_id.hash());
  }

  TEST_CASE("id_from_name") {
    auto index1 = ctti::id_from_name("int");
    auto index2 = ctti::id_from_name("double");
    auto index3 = ctti::id_from_name("int");

    CHECK_EQ(index1, index3);
    CHECK_NE(index1, index2);
    CHECK_NE(index1.hash(), 0);
  }

  TEST_CASE("type_id_in_container") {
    std::unordered_map<ctti::type_id, std::string> type_map;

    type_map[ctti::type_id_of<int>()] = "integer";
    type_map[ctti::type_id_of<double>()] = "floating";
    type_map[ctti::type_id_of<std::string>()] = "string";

    CHECK_EQ(type_map.size(), 3);
    CHECK_EQ(type_map[ctti::type_id_of<int>()], "integer");
    CHECK_EQ(type_map[ctti::type_id_of<double>()], "floating");
    CHECK_EQ(type_map[ctti::type_id_of<std::string>()], "string");
  }

  TEST_CASE("type_index_in_container") {
    std::unordered_map<ctti::type_index, std::string> index_map;

    index_map[ctti::type_index_of<int>()] = "integer";
    index_map[ctti::type_index_of<double>()] = "floating";
    index_map[ctti::type_index_of<CustomStruct>()] = "custom";

    CHECK_EQ(index_map.size(), 3);
    CHECK_EQ(index_map[ctti::type_index_of<int>()], "integer");
    CHECK_EQ(index_map[ctti::type_index_of<double>()], "floating");
    CHECK_EQ(index_map[ctti::type_index_of<CustomStruct>()], "custom");
  }

  TEST_CASE("enum_type_id") {
    auto enum_id = ctti::type_id_of<CustomEnum>();
    auto enum_index = ctti::type_index_of<CustomEnum>();

    CHECK_EQ(enum_id.name(), "CustomEnum");
    CHECK_NE(enum_index.hash(), 0);
    CHECK_EQ(enum_index.hash(), enum_id.hash());
  }

  TEST_CASE("cv_qualified_types") {
    auto const_id = ctti::type_id_of<const int>();
    auto volatile_id = ctti::type_id_of<volatile int>();
    auto plain_id = ctti::type_id_of<int>();

    CHECK_NE(const_id, plain_id);
    CHECK_NE(volatile_id, plain_id);
    CHECK_NE(const_id, volatile_id);
  }

  TEST_CASE("pointer_types") {
    auto ptr_id = ctti::type_id_of<int*>();
    auto ref_id = ctti::type_id_of<int&>();
    auto value_id = ctti::type_id_of<int>();

    CHECK_NE(ptr_id, value_id);
    CHECK_NE(ref_id, value_id);
    CHECK_NE(ptr_id, ref_id);
  }

  TEST_CASE("type_id_names") {
    CHECK_EQ(ctti::type_id_of<int*>().name(), "int*");
    CHECK_EQ(ctti::type_id_of<int&>().name(), "int&");
    CHECK_EQ(ctti::type_id_of<const int>().name(), "const int");
    CHECK_EQ(ctti::type_id_of<volatile int>().name(), "volatile int");
  }
}
