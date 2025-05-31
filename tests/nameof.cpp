#include "catch.hpp"

#include <ctti/nameof.hpp>

using namespace ctti;

namespace foo {

struct Foo {};

namespace bar {

struct Bar {};
}  // namespace bar

inline namespace inline_quux {
struct Quux {};

}  // namespace inline_quux

}  // namespace foo

namespace bar {

struct Bar {};

constexpr std::string_view ctti_nameof(ctti::type_tag<Bar>) {
  return "Bar";  // without namespace
}

enum class Enum { A, B, C };

constexpr std::string_view ctti_nameof(CTTI_STATIC_VALUE(Enum::C)) {
  return "Enum::Si";
}

}  // namespace bar

class MyClass {
public:
  class InnerClass {};
};

struct MyStruct {
  struct InnerStruct {};
};

enum ClassicEnum { A, B, C };
enum class EnumClass { A, B, C };

TEST_CASE("nameof") {
  SECTION("basic types") {
    REQUIRE(nameof<int>() == "int");
    REQUIRE(nameof<bool>() == "bool");
    REQUIRE(nameof<char>() == "char");
    REQUIRE(nameof<void>() == "void");
  }

  SECTION("class types") {
    REQUIRE(nameof<MyClass>() == "MyClass");
    REQUIRE(nameof<MyClass::InnerClass>() == "MyClass::InnerClass");
  }

  SECTION("struct types") {
    REQUIRE(nameof<MyStruct>() == "MyStruct");
    REQUIRE(nameof<MyStruct::InnerStruct>() == "MyStruct::InnerStruct");
  }

  SECTION("enum types") {
    REQUIRE(nameof<ClassicEnum>() == "ClassicEnum");
    REQUIRE(nameof<EnumClass>() == "EnumClass");
  }

  SECTION("with namespaces") {
    REQUIRE(nameof<foo::Foo>() == "foo::Foo");
    REQUIRE(nameof<foo::bar::Bar>() == "foo::bar::Bar");

    SECTION("inline namespaces") {
      REQUIRE(nameof<foo::inline_quux::Quux>() == "foo::inline_quux::Quux");
      REQUIRE(nameof<foo::Quux>() == "foo::inline_quux::Quux");
    }

    SECTION("std") {
      REQUIRE(nameof<std::string>() == "std::string");
    }
  }

  SECTION("with custom ctti_nameof()") {
    SECTION("intrusive customize") {
      struct Foo {
        static constexpr const char* ctti_nameof() { return "Foobar"; }
      };

      REQUIRE(nameof<Foo>() == "Foobar");
    }

    SECTION("non-intrusive customize") {
      REQUIRE(nameof<bar::Bar>() == "Bar");
      REQUIRE(nameof<CTTI_STATIC_VALUE(bar::Enum::C)>() == "Enum::Si");
    }
  }

#ifdef CTTI_HAS_VARIABLE_TEMPLATES
  SECTION("variable templates") {
    REQUIRE(ctti::nameof_v<int> == nameof<int>());
    REQUIRE(ctti::nameof_value_v<int, 42> == nameof<int, 42>());
  }
#endif  // CTTI_HAS_VARIABLE_TEMPLATES
}

TEST_CASE("nameof.enums") {
  SECTION("enum values") {
    // Get the actual values
    const auto classic_enum_name = nameof<CTTI_STATIC_VALUE(ClassicEnum::A)>();
    const auto enum_class_name = nameof<CTTI_STATIC_VALUE(EnumClass::A)>();

    INFO("ClassicEnum::A resolved to: '" << classic_enum_name << "'");
    INFO("EnumClass::A resolved to: '" << enum_class_name << "'");

    // Accept either the fully qualified name or just the value name
    bool classic_ok = (classic_enum_name == "ClassicEnum::A" || classic_enum_name == "A");
    bool enum_class_ok = (enum_class_name == "EnumClass::A" || enum_class_name == "A");

    REQUIRE(classic_ok);
    REQUIRE(enum_class_ok);
  }
}
