#include <ctti/ctti.hpp>

#include <iostream>
#include <string>

struct TestStruct {
  int value = 0;
  std::string name;

  static constexpr std::string_view ctti_name_of() noexcept { return "TestStruct"; }
};

template <>
struct ctti::meta<TestStruct> {
  using type = TestStruct;

  static constexpr auto reflection =
      ctti::make_reflection(ctti::member<"value", &TestStruct::value>(), ctti::member<"name", &TestStruct::name>());
};

int main() {
  std::cout << "Testing CTTI library\n";
  std::cout << "Type name: " << ctti::name_of<TestStruct>() << '\n';

  TestStruct obj(42, "test");

  constexpr auto reflection = ctti::get_reflection<TestStruct>();
  std::cout << "Symbol count: " << reflection.size << '\n';

  constexpr auto value_symbol = ctti::get_symbol<TestStruct, "value">();
  if constexpr (value_symbol.is_owner_of<TestStruct>()) {
    std::cout << "Value: " << value_symbol.get_value(obj) << '\n';
  }

  constexpr auto name_symbol = ctti::get_symbol<TestStruct, "name">();
  if constexpr (name_symbol.is_owner_of<TestStruct>()) {
    std::cout << "Name: " << name_symbol.get_value(obj) << '\n';
  }

  constexpr auto symbol_names = reflection.get_symbol_names();
  std::cout << "Symbol names: ";
  for (const auto& name : symbol_names) {
    std::cout << name << ' ';
  }
  std::cout << '\n';

  return 0;
}
