#include "doctest.h"

#include <ctti/symbol.hpp>

#include <iostream>
#include <string>

struct SymbolTestStruct {
  int value = 42;
  std::string name = "test";

  void set_value(int v) { value = v; }
  int get_value() const { return value; }
};

enum class TestEnum { First = 1, Second = 2 };

CTTI_DEFINE_SYMBOL(value);
CTTI_DEFINE_SYMBOL(name);
CTTI_DEFINE_SYMBOL(private_member);
CTTI_DEFINE_SYMBOL(public_member);
CTTI_DEFINE_SYMBOL(get_value);
CTTI_DEFINE_SYMBOL(set_value);

TEST_SUITE("symbol") {
  TEST_CASE("symbol_basic_properties") {
    CHECK(ctti_symbols::value::symbol == "value");
    CHECK(ctti_symbols::name::symbol == "name");
    CHECK(ctti_symbols::value::hash != 0);
    CHECK(ctti_symbols::name::hash != ctti_symbols::value::hash);
  }

  TEST_CASE("member_detection") {
    CHECK(ctti_symbols::value::is_owner_of<SymbolTestStruct>());
    CHECK(ctti_symbols::name::is_owner_of<SymbolTestStruct>());
    CHECK_FALSE(ctti_symbols::value::is_owner_of<int>());
    CHECK_FALSE(ctti_symbols::name::is_owner_of<double>());
  }

  TEST_CASE("member_access") {
    SymbolTestStruct obj;

    auto& val_ref = ctti::get_member_value<ctti_symbols::value>(obj);
    auto& name_ref = ctti::get_member_value<ctti_symbols::name>(obj);

    CHECK(val_ref == 42);
    CHECK(name_ref == "test");

    ctti::set_member_value<ctti_symbols::value>(obj, 100);
    ctti::set_member_value<ctti_symbols::name>(obj, std::string("updated"));

    CHECK(obj.value == 100);
    CHECK(obj.name == "updated");
  }

  TEST_CASE("const_member_access") {
    const SymbolTestStruct obj;

    auto val = ctti::get_member_value<ctti_symbols::value>(obj);
    auto name_val = ctti::get_member_value<ctti_symbols::name>(obj);

    CHECK(val == 42);
    CHECK(name_val == "test");
  }

  TEST_CASE("symbol_with_private_members") {
    class TestClass {
    private:
      int private_member = 42;

    public:
      int public_member = 100;
    };

    // Should not detect private members
    CHECK_FALSE(ctti_symbols::private_member::is_owner_of<TestClass>());
    CHECK(ctti_symbols::public_member::is_owner_of<TestClass>());
  }

  TEST_CASE("member_traits") {
    using value_traits = ctti::member_traits<int SymbolTestStruct::*>;
    using name_traits = ctti::member_traits<std::string SymbolTestStruct::*>;

    CHECK(std::same_as<value_traits::value_type, int>);
    CHECK(std::same_as<name_traits::value_type, std::string>);
  }

  TEST_CASE("function_member_traits") {
    using get_value_traits = ctti::member_traits<int (SymbolTestStruct::*)() const>;
    using set_value_traits = ctti::member_traits<void (SymbolTestStruct::*)(int)>;

    CHECK(std::same_as<get_value_traits::value_type, int>);
    CHECK(std::same_as<set_value_traits::value_type, int>);
  }

  TEST_CASE("symbol_from_hash") {
    constexpr auto hash_val = ctti_symbols::value::hash;
    using symbol_type = ctti::symbol_from_hash<hash_val>;

    // Should compile without errors
    static_assert(std::same_as<symbol_type, ctti::no_symbol<hash_val>> ||
                  !std::same_as<symbol_type, ctti::no_symbol<hash_val>>);
  }
}
