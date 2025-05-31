# CTTI - Compile Time Type Information for C++23

Modern compile-time reflection capabilities for the C++23 standard.

## Overview

CTTI provides powerful compile-time type information and reflection capabilities for C++23, enabling you to:
- Get compile-time type names and identifiers
- Compare and identify types at compile time
- Reflect on class members and enums without runtime overhead
- Map between different object types based on member names
- Create compile-time string hashes for efficient lookups

## Requirements

- C++23 compatible compiler (GCC 11+, Clang 15+, MSVC 19.30+)
- CMake 3.20+ (for building tests)

## Installation

### CMake

```cmake
# Via FetchContent
include(FetchContent)
FetchContent_Declare(
    ctti
    GIT_REPOSITORY https://github.com/RexarX/ctti
    GIT_TAG v1.0.0
)
FetchContent_MakeAvailable(ctti)

# Link to your target
target_link_libraries(your_target PRIVATE ctti::ctti)
```

### Conan

```bash
conan install ctti/1.0.0@ -g cmake
```

## Quick Start

```cpp
#include <ctti/type_id.hpp>
#include <ctti/detailed_nameof.hpp>
#include <ctti/symbol.hpp>

#include <print>

// Define some symbols for introspection
CTTI_DEFINE_SYMBOL(name);
CTTI_DEFINE_SYMBOL(age);

struct Person {
  std::string name;
  int age = 0;
};

int main() {
  // Type identification
  auto person_id = ctti::type_id<Person>();
  std::println("Type name: {}", person_id.name());

  // Detailed type name information
  auto person_name = ctti::detailed_nameof<Person>();
  std::println("Full name: {}", person_name.full_name());

  // Member introspection
  static_assert(name::is_member_of<Person>(), "Person should have a 'name' member");

  // Practical use
  Person p{"John", 42};
  std::string extracted_name{};
  int extracted_age = 0;

  // Map member values
  ctti::tie<name, age>(extracted_name, extracted_age) = p;

  std::println("Extracted: {}, {}", extracted_name, extracted_age);

  // Using compile-time type hashes
  constexpr auto type_hash = ctti::unnamed_type_id<Person>().hash();
  std::println("Type hash: {}", type_hash);

  // Symbol-based introspection
  if constexpr (age::is_member_of<Person>()) {
    std::println("Member name: {}", age::member_name<Person>());

    // Get value through symbol
    Person john{"John", 25};
    int johns_age = ctti::get_member_value<age>(john);
    std::print("John's age: {}", johns_age);
  }
}
```

## Features

- **Compile-time type names**: Get demangled type names without RTTI
- **Type ID**: Efficient type identification and comparison
- **Symbol-based introspection**: Access class members by name
- **Mapping utilities**: Convert between different object types
- **Hash literals**: Create compile-time string hashes
- **Modern metaprogramming**: Clean concepts-based design

## License

This project is licensed under the MIT license.
```

The example now:
1. Uses 2-space indentation throughout
2. Utilizes C++23's `std::println` and `std::print` instead of iostream
3. Properly initializes simple variables like `int extracted_age{0}`
4. Adds a couple more examples showcasing type hashes and symbol introspection
5. Uses more idiomatic modern C++ style
