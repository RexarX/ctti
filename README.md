# CTTI - Compile Time Type Information

A modern C++23 header-only library for compile-time type information and reflection.

<details>
<summary><strong>📋 Table of Contents</strong></summary>

- [Features](#features)
- [Quick Start](#quick-start)
- [Core API](#core-api)
  - [Type Names](#type-names)
  - [Type Identification](#type-identification)
  - [Reflection System](#reflection-system)
  - [Symbol System](#symbol-system)
  - [Hash Literals](#hash-literals)
- [Advanced Features](#advanced-features)
  - [Attributes System](#attributes-system)
  - [Method Overloads](#method-overloads)
  - [Constructor Info](#constructor-info)
  - [Enum Utilities](#enum-utilities)
  - [Template Info](#template-info)
  - [Object Mapping](#object-mapping)
  - [Tie Operations](#tie-operations)
  - [Inheritance Info](#inheritance-info)
  - [Static Values](#static-values)
  - [Reflection Iteration](#reflection-iteration)
- [Installation](#installation)
- [Requirements](#requirements)
- [Design Principles](#design-principles)
- [Concepts and Type Safety](#concepts-and-type-safety)
- [Examples](#examples)
- [Compiler Support](#compiler-support)
- [Thread Safety](#thread-safety)
- [License](#license)

</details>

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
  enum class Status { Active, Inactive };
  std::print("Status::Active: {}\n", ctti::name_of<Status, Status::Active>());

  // Hash literals
  using namespace ctti::hash_literals;
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
ctti::type_id_of<T>()     // Get type ID from type
ctti::type_id_of(object)  // Get type ID from object
ctti::type_index_of<T>()  // Get type index (hash-only)

auto id = ctti::type_id_of<int>();
id.name()  // Get type name
id.hash()  // Get type hash
```

### Reflection System

Define reflectable types with meta specialization:

```cpp
struct Point {
  double x = 0.0;
  double y = 0.0;

  double calculate() const { return x * x + y * y; }
  void set_coordinates(double new_x, double new_y) { x = new_x; y = new_y; }
};

template <>
struct ctti::meta<Point> {
  using type = Point;

  static constexpr auto reflection = ctti::make_reflection(
    ctti::member<"x", &Point::x>(),
    ctti::member<"y", &Point::y>(ctti::read_only{}),
    ctti::member<"calculate", &Point::calculate>(),
    ctti::member<"set_coordinates", &Point::set_coordinates>()
  );
};

// Use reflection
Point p{3.0, 4.0};
auto x_val = ctti::get_member_value<"x">(p);  // Get member value
ctti::set_member_value<"x">(p, 5.0);          // Set member value

constexpr auto calc_symbol = ctti::get_reflected_symbol<"calculate", Point>();
auto result = ctti::call_symbol<calc_symbol>(p);  // Call method
```

### Symbol System

Create symbols for member access:

```cpp
constexpr auto value_symbol = ctti::make_simple_symbol<"value", &MyStruct::value>();
constexpr auto name_symbol = ctti::make_simple_symbol<"name", &MyStruct::name>();

// Check ownership and access
value_symbol.is_owner_of<MyStruct>()  // true
auto member_ptr = value_symbol.get_member<MyStruct>();

// Direct symbol operations
auto val = ctti::get_symbol_value<value_symbol>(obj);
ctti::set_symbol_value<value_symbol>(obj, new_value);
```

### Hash Literals

```cpp
using namespace ctti::hash_literals;

constexpr auto hash = "hello"_sh;  // Compile-time string hash
switch (ctti::fnv1a_hash(input)) {
  case "start"_sh:
    // Handle start
    break;
  case "stop"_sh:
    // Handle stop
    break;
}
```

## Advanced Features

### Attributes System

```cpp
struct Person {
  std::string name;
  int age = 0;
  std::string email;
};

template <>
struct ctti::meta<Person> {
  using type = Person;

  static constexpr auto reflection = ctti::make_reflection(
    ctti::member<"name", &Person::name>(ctti::validated{}),
    ctti::member<"age", &Person::age>(ctti::since<1>{}),
    ctti::member<"email", &Person::email>(ctti::deprecated{}, ctti::description{})
  );
};

// Check attributes
constexpr auto name_symbol = ctti::get_reflected_symbol<"name", Person>();
constexpr bool is_validated = ctti::has_tag<name_symbol, ctti::validated_tag>();
constexpr auto age_symbol = ctti::get_reflected_symbol<"age", Person>();
constexpr bool has_version = ctti::has_attribute_value<age_symbol, 1>();
```

### Method Overloads

```cpp
struct Calculator {
  int add(int a, int b) { return a + b; }
  double add(double a, double b) { return a + b; }
  std::string add(const std::string& a, const std::string& b) { return a + b; }
};

template <>
struct ctti::meta<Calculator> {
  using type = Calculator;

  static constexpr auto reflection = ctti::make_reflection(
    ctti::overloaded_member<"add",
      static_cast<int (Calculator::*)(int, int)>(&Calculator::add),
      static_cast<double (Calculator::*)(double, double)>(&Calculator::add),
      static_cast<std::string (Calculator::*)(const std::string&, const std::string&)>(&Calculator::add)
    >()
  );
};

// Call overloads
Calculator calc;
constexpr auto add_symbol = ctti::get_reflected_symbol<"add", Calculator>();
int result1 = ctti::call_symbol<add_symbol>(calc, 5, 3);
double result2 = ctti::call_symbol<add_symbol>(calc, 5.5, 3.2);
std::string result3 = ctti::call_symbol<add_symbol>(calc, std::string("Hello"), std::string(" World"));

// Overload queries
auto query = ctti::query_overloads<add_symbol, Calculator>();
bool can_call_with_int = query.has<int, int>();
bool can_call_with_string = query.has<const std::string&, const std::string&>();

// Overload wrapper
auto wrapper = ctti::get_overload_wrapper<add_symbol>(calc);
wrapper(10, 20);    // Calls appropriate overload
wrapper(1.5, 2.5);  // Calls appropriate overload
```

### Constructor Info

```cpp
auto info = ctti::get_constructor_info<MyClass>();
info.is_default_constructible()              // Check if default constructible
info.can_construct<int, std::string>()       // Check if constructible with args
auto obj = info.construct(42, "test");       // Construct with args
auto ptr = info.make_unique(42, "test");     // Create unique_ptr
auto shared = info.make_shared(42, "test");  // Create shared_ptr
```

### Enum Utilities

```cpp
enum class Color { Red, Green, Blue };

auto info = ctti::get_enum_info<Color>();
info.is_scoped()                     // true for scoped enums
info.name_of_value<Color::Red>()     // "Red"
info.underlying_value<Color::Red>()  // Underlying value

auto list = ctti::make_enum_list<Color, Color::Red, Color::Green>();
list.contains<Color::Red>()  // true
list.count                   // 2

// Iterate over enum values
list.for_each([](auto index, Color value) {
  std::print("Enum value: {}\n", ctti::enum_name<Color, value>());
});
```

### Template Info

```cpp
auto info = ctti::get_template_info<std::vector<int>>();
info.is_template_instantiation  // true
info.parameter_count            // Number of template params
info.type_parameter_count       // Number of type params
info.value_parameter_count      // Number of value params

// For type templates
if constexpr (ctti::variadic_type_template<std::vector<int>>) {
  using first_param = decltype(info)::type_parameter<0>;  // int
  auto param_names = info.type_parameter_names();         // ["int"]

  info.for_each_type_parameter([](auto tag) {
    std::print("Type param: {}\n", ctti::name_of<decltype(tag)::type>());
  });
}

// For value templates
template <int N> struct Array {};
auto array_info = ctti::get_template_info<Array<5>>();
if constexpr (ctti::variadic_value_template<Array<5>>) {
  constexpr auto value = array_info.value_parameter<0>();  // 5
}

// For mixed templates
template <typename T, int Size> struct FixedArray {};
auto mixed_info = ctti::get_template_info<FixedArray<double, 10>>();
if constexpr (ctti::mixed_variadic_template<FixedArray<double, 10>>) {
  using type_param = decltype(mixed_info)::type_parameter;  // double
  constexpr auto size_param = mixed_info.value_parameter;   // 10
}
```

### Object Mapping

Map between objects using symbols:

```cpp
struct Source { int value = 0; std::string name; };
struct Sink { int value = 0; std::string name; };

Source src{42, "test"};
Sink dst;

constexpr auto src_value_symbol = ctti::make_simple_symbol<"value", &Source::value>();
constexpr auto dst_value_symbol = ctti::make_simple_symbol<"value", &Sink::value>();
constexpr auto src_name_symbol = ctti::make_simple_symbol<"name", &Source::name>();
constexpr auto dst_name_symbol = ctti::make_simple_symbol<"name", &Sink::name>();

// Direct mapping
ctti::map<src_value_symbol, dst_value_symbol>(src, dst);
ctti::map<src_name_symbol, dst_name_symbol>(src, dst);

// Custom mapping function
auto custom_mapping = ctti::make_mapping<src_value_symbol, dst_value_symbol>(
  [](const Source& source, auto src_sym, Sink& sink, auto dst_sym) {
    auto src_member = src_sym.get_member<Source>();
    auto dst_member = dst_sym.get_member<Sink>();
    sink.*dst_member = source.*src_member + 1000;
  }
);

custom_mapping(src, dst);

// Multiple mappings
auto value_mapping = ctti::make_mapping<src_value_symbol, dst_value_symbol>();
auto name_mapping = ctti::make_mapping<src_name_symbol, dst_name_symbol>();
ctti::map(src, dst, value_mapping, name_mapping);
```

### Tie Operations

Extract values from objects using symbols:

```cpp
struct Point { double x = 0.0, y = 0.0; };
Point p{3.0, 4.0};

double x_val = 0.0, y_val = 0.0;
constexpr auto x_symbol = ctti::make_simple_symbol<"x", &Point::x>();
constexpr auto y_symbol = ctti::make_simple_symbol<"y", &Point::y>();

auto tied = ctti::tie<x_symbol, y_symbol>(x_val, y_val);
tied = p;  // Extract values: x_val = 3.0, y_val = 4.0
```

### Inheritance Info

```cpp
struct Base { virtual ~Base() = default; };
struct Derived : public Base {};

auto info = ctti::get_inheritance_info<Derived, Base>();
info.is_derived         // true
info.is_public_derived  // true

auto poly_info = ctti::get_polymorphism_info<Base>();
poly_info.is_polymorphic          // true
poly_info.has_virtual_destructor  // true

// Check inheritance at compile time
constexpr bool derives = ctti::is_derived_from<Derived, Base>();
constexpr bool publicly_derives = ctti::is_publicly_derived_from<Derived, Base>();
constexpr bool is_polymorphic = ctti::is_polymorphic<Base>();

// Safe casting
Derived derived;
Base* base_ptr = &derived;
auto* derived_ptr = ctti::safe_cast<Derived>(base_ptr);
auto* dynamic_ptr = ctti::dynamic_cast_safe<Derived>(base_ptr);
```

### Static Values

```cpp
constexpr auto val = ctti::static_value<int, 42>{};
val.get()              // 42
static_cast<int>(val)  // 42
val == 42              // true

auto dynamic_val = ctti::make_static_value<123>();
constexpr auto from_value = ctti::make_static_value<42>();
```

### Reflection Iteration

```cpp
// Iterate over all symbols
ctti::for_each_symbol<Point>([](auto symbol) {
  std::print("Symbol: {}\n", ctti::symbol_name<symbol>());
  std::print("Hash: {}\n", ctti::symbol_hash<symbol>());
  std::print("Overload count: {}\n", ctti::overload_count<symbol>());
});

// Get symbol names
constexpr auto names = ctti::get_symbol_names<Point>();
constexpr auto symbol_count = ctti::symbol_count<Point>();

// Check if symbol exists
constexpr bool has_x = ctti::has_symbol<"x", Point>();
constexpr bool has_invalid = ctti::has_symbol<"invalid", Point>();
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
    GITHUB_REPOSITORY "RexarX/ctti"
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
3. **Clean interface**: snake_case for user functions
4. **Minimal macros**: Eliminated in favor of template parameters and concepts
5. **Type safety**: Leverages concepts and strong typing throughout
6. **Zero runtime cost**: Everything resolved at compile time

---

## Concepts and Type Safety

The library uses C++23 concepts extensively for type safety:

```cpp
// Type concepts
ctti::reflectable<T>             // Type has reflection info
ctti::template_instantiation<T>  // Type is template instantiation
ctti::scoped_enum<T>             // Type is scoped enum
ctti::unscoped_enum<T>           // Type is unscoped enum
ctti::polymorphic<T>             // Type is polymorphic
ctti::abstract<T>                // Type is abstract
ctti::final_type<T>              // Type is final

// Template concepts
ctti::variadic_type_template<T>   // Template with type parameters
ctti::variadic_value_template<T>  // Template with value parameters
ctti::mixed_variadic_template<T>  // Template with mixed parameters
ctti::template_specialization<T>  // Type is template specialization

// Constructor concepts
ctti::constructible<T, Args...>          // Type constructible with args
ctti::nothrow_constructible<T, Args...>  // Type nothrow constructible
ctti::aggregate_type<T>                  // Type is aggregate
ctti::default_constructible<T>           // Type is default constructible
ctti::copy_constructible<T>              // Type is copy constructible
ctti::move_constructible<T>              // Type is move constructible

// Inheritance concepts
ctti::derived_from<Derived, Base>           // Derived inherits from Base
ctti::publicly_derived_from<Derived, Base>  // Public inheritance
ctti::has_virtual_destructor<T>             // Type has virtual destructor

// Other concepts
ctti::has_custom_name_of<T>      // Type provides custom name
ctti::integral_constant_type<T>  // Type is integral constant
```

---

## Examples

See `examples/main.cpp` for comprehensive usage examples including:

- Basic type information
- Complete reflection setup
- Method overload handling with clean API
- Template parameter analysis
- Object mapping and transformation
- Enum value processing
- Symbol-based operations
- Compile-time tie operations
- Attribute system usage

---

## Compiler Support

- **GCC 13+**: Full support
- **Clang 16+**: Full support
- **MSVC 19.35+**: Full support

All major compilers with C++23 support are compatible.

---

## Thread Safety

Since all operations are compile-time, thread safety is not a concern. The library generates no runtime code that could have thread safety issues.

---

## License

MIT License - see LICENSE file for details.

---

[↑ Back to Top](#ctti---compile-time-type-information)
