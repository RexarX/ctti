#include "catch.hpp"

#include <ctti/type_id.hpp>

#include <map>
#include <string>

TEST_CASE("type_id") {
  SECTION("basic usage") {
    const auto int_id = ctti::type_id<int>();
    const auto int_id2 = ctti::type_id<int>();
    const auto float_id = ctti::type_id<float>();

    REQUIRE(int_id == int_id2);
    REQUIRE(int_id != float_id);
    REQUIRE(int_id.name() == "int");
    REQUIRE(float_id.name() == "float");
  }

  SECTION("with containers") {
    const auto vector_int_id = ctti::type_id<std::vector<int>>();
    const auto vector_float_id = ctti::type_id<std::vector<float>>();

    REQUIRE(vector_int_id != vector_float_id);
    REQUIRE(vector_int_id.name().find("vector") != std::string_view::npos);
    REQUIRE(vector_int_id.name().find("int") != std::string_view::npos);
  }

  SECTION("using as map key") {
    std::map<ctti::type_index, std::string> type_names;

    type_names[ctti::unnamed_type_id<int>()] = "integer";
    type_names[ctti::unnamed_type_id<float>()] = "float";
    type_names[ctti::unnamed_type_id<std::string>()] = "string";

    REQUIRE(type_names[ctti::unnamed_type_id<int>()] == "integer");
    REQUIRE(type_names[ctti::unnamed_type_id<float>()] == "float");
    REQUIRE(type_names[ctti::unnamed_type_id<std::string>()] == "string");
  }

  SECTION("unnamed_type_id equality") {
    const auto int_id = ctti::unnamed_type_id<int>();
    const auto int_id2 = ctti::unnamed_type_id<int>();
    const auto float_id = ctti::unnamed_type_id<float>();

    REQUIRE(int_id == int_id2);
    REQUIRE(int_id != float_id);
  }
}
