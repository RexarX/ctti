# CTTI - Compile Time Type Introspection

A modern C++23 header-only library for compile-time type introspection and reflection.

## Features

- **Pure C++23**: Uses concepts, constexpr everywhere, and latest language features
- **Compile-time only**: All operations happen at compile time with zero runtime overhead
- **Clean API**: Simple, intuitive interface following modern C++ conventions
- **Type safe**: Leverages C++23 type system and concepts for maximum safety
- **Header-only**: No compilation required, just include and use
- **Zero dependencies**: Uses only standard library

## Quick Start

```cpp
#include <ctti/ctti.hpp>
#include <print>

int main() {
  // Get type names
  std::print("int: {}\n", ctti::name_of<int>());  // "int"
  std::print("vector: {}\n", ctti::name_of<std::vector<std::string>>());

  // Type identification
  constexpr auto int_id = ctti::type_id_of<int>();
  constexpr auto string_id = ctti::type_id_of<std::string>();

  std::print("int: {} (hash: {})\n", int_id.name(), int_id.hash());

  // Enum values
  enum class Status { kActive, kInactive };
  std::print("Status::kActive: {}\n", ctti::name_of<Status, Status::kActive>());

  // Hash literals
  using namespace ctti;
  constexpr auto hash = "hello"_sh;
  std::print("Hash: {}\n", hash);

  return 0;
}
```

---

## Core API

### Type Names

```cpp
ctti::name_of<Type>()            // Get type name
ctti::name_of<Type, Value>()     // Get enum/constant value name
ctti::qualified_name_of<Type>()  // Get qualified name with namespaces
```

### Type Identification

```cpp
ctti::type_id_of<T>()               // Get type ID from type
ctti::type_id_of(object)            // Get type ID from object
ctti::type_index_of<T>()            // Get type index (hash-only)

auto id = ctti::type_id_of<int>();
id.name()                           // Get type name
id.hash()                           // Get type hash
```

### Symbol-Based Reflection

```cpp
// Define symbols
CTTI_DEFINE_SYMBOL(value);
CTTI_DEFINE_SYMBOL(name);

struct MyStruct {
  int value = 42;
  std::string name = "test";
};

// Check membership
ctti_symbols::value::is_owner_of<MyStruct>()           // true

// Access members
ctti::get_member_value<ctti_symbols::value>(obj)      // Get member value
ctti::set_member_value<ctti_symbols::value>(obj, 10)  // Set member value
```

### Hash Literals

```cpp
using namespace ctti;

constexpr auto hash = "hello"_sh;  // Compile-time string hash

switch (ctti::detail::Fnv1aHash(input)) {
  case "start"_sh:
    // Handle start
    break;
  case "stop"_sh:
    // Handle stop
    break;
}
```

## Advanced Features

### Qualified Names

```cpp
const auto name = ctti::qualified_name_of<std::vector<int>>();
std::print("Simple: {}\n", name.get_name());                    // Simple name
std::print("Full: {}\n", name.get_full_name());                 // Full qualified name
std::print("Qualifier: {}\n", name.get_qualifier(0));           // First qualifier (e.g., "std")
```

### Symbol Metadata

```cpp
std::print("Symbol name: {}\n", ctti_symbols::value::symbol);            // Symbol name
std::print("Symbol hash: {}\n", ctti_symbols::value::hash);              // Symbol hash
std::print("Member name: {}\n", ctti_symbols::value::member_name<T>());
```

### Constructor Info

```cpp
auto info = ctti::get_constructor_info<MyClass>();
info.is_default_constructible()                     // Check if default constructible
info.can_construct<int, std::string>()              // Check if constructible with args
auto obj = info.construct(42, "test");              // Construct with args
auto ptr = info.make_unique(42, "test");            // Create unique_ptr
```

### Enum Utilities

```cpp
enum class Color { kRed, kGreen, kBlue };

auto info = ctti::get_enum_info<Color>();
info.is_scoped()                           // true for scoped enums
info.name_of_value<Color::kRed>()          // "kRed"
info.underlying_value<Color::kRed>()       // Underlying value

auto list = ctti::make_enum_list<Color, Color::kRed, Color::kGreen>();
list.contains<Color::kRed>()  // true
```

### Template Info

```cpp
auto info = ctti::get_template_info<std::vector<int>>();
info.is_template_instantiation                           // true
info.parameter_count                                     // Number of template params
using first_param = decltype(info)::parameter<0>;          // First parameter type
```

### Object Mapping

```cpp
struct Source { int value; std::string name; };
struct Sink { int value; std::string name; };

Source src{42, "test"};
Sink dst;

ctti::map<ctti_symbols::value, ctti_symbols::value>(src, dst);
ctti::map<ctti_symbols::name, ctti_symbols::name>(src, dst);
```

### Tie Operations

```cpp
struct Point { double x(0.0), y(0.0); };
Point p{3.0, 4.0};

double x_val(0.0), y_val(0.0);
auto tied = ctti::tie<ctti_symbols::x, ctti_symbols::y>(x_val, y_val);
tied = p;  // Extract values
```

---

## Installation

### Header-only

Simply copy the `include/ctti` directory to your project and include the headers.

### CMake

```cmake
find_package(ctti REQUIRED)
target_link_libraries(your_target PRIVATE ctti::ctti)
```

### CMake FetchContent

```cmake
include(FetchContent)

FetchContent_Declare(
    ctti
    GIT_REPOSITORY https://github.com/RexarX/ctti
    GIT_TAG master  # or specific version tag like "v1.0.0"
)

FetchContent_MakeAvailable(ctti)

target_link_libraries(your_target PRIVATE ctti::ctti)
```

### CPM.cmake

```cmake
include(cmake/CPM.cmake)

CPMAddPackage(
    NAME ctti
    GITHUB_REPOSITORY "https://github.com/RexarX/ctti"
    VERSION 1.0.0
    GIT_TAG master
)

target_link_libraries(your_target PRIVATE ctti::ctti)
```

---

## Requirements

- C++23 compatible compiler (GCC 12+, Clang 15+, MSVC 19.35+)
- Standard library with C++23 features

---

## Design Principles

1. **Compile-time everything**: All operations must work at compile time
2. **Modern C++23**: Uses latest language features for better safety and performance
3. **Clean interface**: snake_case for user functions, PascalCase for concepts
4. **Minimal macros**: Only where absolutely necessary
5. **Type safety**: Leverages concepts and strong typing
6. **Zero runtime cost**: Everything resolved at compile time

---

## Examples

See `examples/main.cpp` for comprehensive usage examples.

---

## Compiler Support

- **GCC 12+**: Full support
- **Clang 15+**: Full support
- **MSVC 19.35+**: Full support

---

## License

MIT License - see LICENSE file for details.
