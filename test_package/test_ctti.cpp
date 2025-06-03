#include <iostream>

#include <ctti/ctti.hpp>

struct TestStruct {
  int value = 0;
  std::string name;
};

CTTI_DEFINE_SYMBOL(value);
CTTI_DEFINE_SYMBOL(name);

int main() {
  std::cout << "Testing CTTI library\n";
  std::cout << "Type name: " << ctti::name_of<TestStruct>() << '\n';

  TestStruct obj{42, "test"};

  if constexpr (ctti_symbols::value::is_owner_of<TestStruct>()) {
    auto member = ctti_symbols::value::get_member<TestStruct>();
    std::cout << "Value: " << obj.*member << '\n';
  }

  return 0;
}
