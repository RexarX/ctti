#include <ctti/ctti.hpp>

#include <array>
#include <format>
#include <iostream>
#include <optional>
#include <string>
#include <vector>

namespace {

struct Point {
  double x = 0.0;
  double y = 0.0;

  double calculate() const { return x * x + y * y; }
  void set_coordinates(double new_x, double new_y) {
    x = new_x;
    y = new_y;
  }
};

struct Calculator {
  int add(int a, int b) { return a + b; }
  double add(double a, double b) { return a + b; }
  std::string add(const std::string& a, const std::string& b) { return a + b; }
};

struct Named {
  std::string name;
  int value = 0;
};

template <typename T>
struct Container {
  T data;
  std::size_t size = 1;

  void set_data(const T& new_data) { data = new_data; }
  const T& get_data() const { return data; }
};

template <typename T, std::size_t N>
struct FixedArray {
  std::array<T, N> data;

  constexpr std::size_t size() const { return N; }
  T& at(std::size_t index) { return data.at(index); }
  const T& at(std::size_t index) const { return data.at(index); }
};

template <auto Value>
struct ValueWrapper {
  static constexpr auto kValue = Value;

  constexpr auto get_value() const { return kValue; }
};

template <typename T, auto DefaultValue>
struct DefaultedContainer {
  T value = static_cast<T>(DefaultValue);

  void reset() { value = static_cast<T>(DefaultValue); }
  void set(const T& new_value) { value = new_value; }
};

enum class Color { Red, Green, Blue };
enum Status { Active, Inactive, Pending };

}  // namespace

template <>
struct ctti::meta<Point> {
  using type = Point;

  static constexpr auto reflection = ctti::make_reflection(
      ctti::member<"x", &Point::x>(), ctti::member<"y", &Point::y>(ctti::read_only{}),
      ctti::member<"calculate", &Point::calculate>(), ctti::member<"set_coordinates", &Point::set_coordinates>());
};

template <>
struct ctti::meta<Calculator> {
  using type = Calculator;

  static constexpr auto reflection = ctti::make_reflection(
      ctti::overloaded_member<"add", static_cast<int (Calculator::*)(int, int)>(&Calculator::add),
                              static_cast<double (Calculator::*)(double, double)>(&Calculator::add),
                              static_cast<std::string (Calculator::*)(const std::string&, const std::string&)>(
                                  &Calculator::add)>());
};

template <>
struct ctti::meta<Named> {
  using type = Named;

  static constexpr auto reflection =
      ctti::make_reflection(ctti::member<"name", &Named::name>(ctti::validated{}),
                            ctti::member<"value", &Named::value>(ctti::attribute_value<1>{}));
};

// Register Color enum values for convenient runtime operations
template <>
struct ctti::enum_values<Color> {
  static constexpr auto values = ctti::make_enum_list<Color::Red, Color::Green, Color::Blue>();
};

namespace {

template <typename... Args>
void Print(std::format_string<Args...> fmt, Args&&... args) {
  std::cout << std::format(fmt, std::forward<Args>(args)...);
}

void demonstrate_symbol_access() {
  Print("=== Symbol-Based Member Access ===\n");

  Point p{3.0, 4.0};
  Named n{"example", 42};

  const auto x_val = ctti::get_symbol_value<"x">(p);
  const auto y_val = ctti::get_symbol_value<"y">(p);
  constexpr auto calculate_symbol = ctti::get_reflected_symbol<"calculate", Point>();
  const auto calculated = calculate_symbol.call(p);

  Print("Point x: {}\n", x_val);
  Print("Point y: {}\n", y_val);
  Print("Point calculated: {}\n", calculated);

  const auto name_val = ctti::get_symbol_value<"name">(n);
  const auto value_val = ctti::get_symbol_value<"value">(n);

  Print("Named name: {}\n", name_val);
  Print("Named value: {}\n", value_val);

  Print("\n");
}

void demonstrate_overloads() {
  Print("=== Method Overloads ===\n");

  Calculator calc;

  constexpr auto add_symbol = ctti::get_reflected_symbol<"add", Calculator>();

  int result1 = add_symbol.call(calc, 5, 3);
  double result2 = add_symbol.call(calc, 5.5, 3.2);
  std::string result3 = add_symbol.call(calc, std::string("Hello"), std::string(" World"));

  Print("add(5, 3) = {}\n", result1);
  Print("add(5.5, 3.2) = {}\n", result2);
  Print("add(\"Hello\", \" World\") = {}\n", result3);

  Print("Can call with int(int, int): {}\n", add_symbol.has_overload<int(int, int)>());
  Print("Can call with string(const string&, const string&): {}\n",
        add_symbol.has_overload<std::string(const std::string&, const std::string&)>());
  Print("Can call with double(double, double): {}\n", add_symbol.has_overload<double(double, double)>());

  Print("\n");
}

void demonstrate_attributes() {
  Print("=== Attributes ===\n");

  constexpr auto y_symbol = ctti::get_reflected_symbol<"y", Point>();
  constexpr auto name_symbol = ctti::get_reflected_symbol<"name", Named>();
  constexpr auto value_symbol = ctti::get_reflected_symbol<"value", Named>();

  constexpr bool y_is_readonly = y_symbol.has_attribute<ctti::read_only>();
  constexpr bool name_is_validated = name_symbol.has_attribute<ctti::validated>();
  constexpr bool value_has_version = value_symbol.has_attribute_value<1>();

  Print("Point.y is read-only: {}\n", y_is_readonly);
  Print("Named.name is validated: {}\n", name_is_validated);
  Print("Named.value has version 1: {}\n", value_has_version);

  Print("\n");
}

void demonstrate_reflection_iteration() {
  Print("=== Reflection Iteration ===\n");

  Print("Point symbols:\n");
  ctti::for_each_symbol<Point>([](auto symbol) { Print("  - {}\n", symbol.name); });

  Print("Named symbols:\n");
  ctti::for_each_symbol<Named>([](auto symbol) { Print("  - {}\n", symbol.name); });

  constexpr auto point_symbol_names = ctti::get_symbol_names<Point>();
  Print("Point has {} symbols\n", point_symbol_names.size());

  Print("\n");
}

void demonstrate_mapping() {
  Print("=== Object Mapping ===\n");

  struct Source {
    int value = 42;
    std::string name = "source";
    double price = 99.99;
  };

  struct Sink {
    int value = 0;
    std::string name = "sink";
    bool active = false;
  };

  Source src;
  Sink dst;

  constexpr auto value_src_symbol = ctti::make_simple_symbol<"value", &Source::value>();
  constexpr auto value_dst_symbol = ctti::make_simple_symbol<"value", &Sink::value>();
  constexpr auto name_src_symbol = ctti::make_simple_symbol<"name", &Source::name>();
  constexpr auto name_dst_symbol = ctti::make_simple_symbol<"name", &Sink::name>();
  constexpr auto price_src_symbol = ctti::make_simple_symbol<"price", &Source::price>();
  constexpr auto active_dst_symbol = ctti::make_simple_symbol<"active", &Sink::active>();

  ctti::map<value_src_symbol, value_dst_symbol>(src, dst);
  ctti::map<name_src_symbol, name_dst_symbol>(src, dst);

  Print("After direct mapping:\n");
  Print("  dst.value: {}\n", dst.value);
  Print("  dst.name: {}\n", dst.name);
  Print("  dst.active: {}\n", dst.active);

  auto custom_mapping = ctti::make_mapping<price_src_symbol, active_dst_symbol>(
      [](const Source& source, auto src_sym, Sink& sink, auto dst_sym) {
        auto price_member = src_sym.template get_member<Source>();
        auto active_member = dst_sym.template get_member<Sink>();
        sink.*active_member = source.*price_member > 50.0;
      });

  custom_mapping(src, dst);

  Print("After custom mapping:\n");
  Print("  dst.active: {} (price {} > 50.0)\n", dst.active, src.price);

  Print("\n");
}

void demonstrate_tie() {
  Print("=== Tie Functionality ===\n");

  struct TieTestStruct {
    int value = 100;
    std::string name = "tied";
    double weight = 75.5;
  };

  TieTestStruct obj;

  int val = 0;
  std::string nm;
  double wt = 0.0;

  constexpr auto value_symbol = ctti::make_simple_symbol<"value", &TieTestStruct::value>();
  constexpr auto name_symbol = ctti::make_simple_symbol<"name", &TieTestStruct::name>();
  constexpr auto weight_symbol = ctti::make_simple_symbol<"weight", &TieTestStruct::weight>();

  auto tied = ctti::tie<value_symbol, name_symbol, weight_symbol>(val, nm, wt);
  tied = obj;

  Print("Tied values:\n");
  Print("  val: {}\n", val);
  Print("  nm: {}\n", nm);
  Print("  wt: {}\n", wt);

  Print("\n");
}

void demonstrate_symbol_utilities() {
  Print("=== Symbol Utilities ===\n");

  constexpr auto value_symbol = ctti::make_simple_symbol<"value", &Point::x>();
  constexpr auto calc_symbol = ctti::make_simple_symbol<"calculate", &Point::calculate>();

  Print("Symbol names:\n");
  Print("  value_symbol: {}\n", value_symbol.name);
  Print("  calc_symbol: {}\n", calc_symbol.name);

  Print("Symbol hashes:\n");
  Print("  value_symbol: {}\n", value_symbol.hash);
  Print("  calc_symbol: {}\n", calc_symbol.hash);

  Print("Symbol overload counts:\n");
  Print("  value_symbol: {}\n", value_symbol.overload_count);
  Print("  calc_symbol: {}\n", calc_symbol.overload_count);

  Print("Symbol ownership:\n");
  Print("  value_symbol owns Point: {}\n", value_symbol.is_owner_of<Point>());
  Print("  value_symbol owns int: {}\n", value_symbol.is_owner_of<int>());

  Point p{5.0, 12.0};
  auto val = value_symbol.get_value(p);
  Print("Retrieved value using symbol: {}\n", val);

  value_symbol.set_value(p, 7.0);
  Print("After setting value to 7.0: {}\n", p.x);

  Print("Symbol method overload checking:\n");
  Print("  calc_symbol has double() const: {}\n", calc_symbol.has_overload<double() const>());
  Print("  calc_symbol has int(): {}\n", calc_symbol.has_overload<int()>());

  Print("\n");
}

void demonstrate_type_names() {
  Print("=== Type Names ===\n");

  Print("int: {}\n", ctti::name_of<int>());
  Print("Point: {}\n", ctti::name_of<Point>());
  Print("Named: {}\n", ctti::name_of<Named>());
  Print("Calculator: {}\n", ctti::name_of<Calculator>());

  Print("std::vector<int>: {}\n", ctti::name_of<std::vector<int>>());
  Print("std::optional<std::string>: {}\n", ctti::name_of<std::optional<std::string>>());
  Print("Container<int>: {}\n", ctti::name_of<Container<int>>());
  Print("FixedArray<double, 5>: {}\n", ctti::name_of<FixedArray<double, 5>>());
  Print("ValueWrapper<42>: {}\n", ctti::name_of<ValueWrapper<42>>());
  Print("DefaultedContainer<int, 100>: {}\n", ctti::name_of<DefaultedContainer<int, 100>>());

  Print("\n");
}

void demonstrate_template_info() {
  Print("=== Template Information ===\n");

  using VectorInt = std::vector<int>;
  using ArrayDouble5 = FixedArray<double, 5>;
  using ValueInt42 = ValueWrapper<42>;
  using DefaultedIntContainer = DefaultedContainer<int, 100>;

  constexpr bool vector_is_template = ctti::is_template_instantiation<VectorInt>();
  constexpr bool array_is_template = ctti::is_template_instantiation<ArrayDouble5>();
  constexpr bool value_is_template = ctti::is_template_instantiation<ValueInt42>();
  constexpr bool defaulted_is_template = ctti::is_template_instantiation<DefaultedIntContainer>();
  constexpr bool int_is_template = ctti::is_template_instantiation<int>();

  Print("std::vector<int> is template: {}\n", vector_is_template);
  Print("FixedArray<double, 5> is template: {}\n", array_is_template);
  Print("ValueWrapper<42> is template: {}\n", value_is_template);
  Print("DefaultedContainer<int, 100> is template: {}\n", defaulted_is_template);
  Print("int is template: {}\n", int_is_template);

  constexpr auto vector_info = ctti::get_template_info<VectorInt>();
  constexpr auto array_info = ctti::get_template_info<ArrayDouble5>();
  constexpr auto value_info = ctti::get_template_info<ValueInt42>();
  constexpr auto defaulted_info = ctti::get_template_info<DefaultedIntContainer>();

  Print("Vector parameter count: {}\n", vector_info.parameter_count);
  Print("Array parameter count: {}\n", array_info.parameter_count);
  Print("Value parameter count: {}\n", value_info.parameter_count);
  Print("Defaulted parameter count: {}\n", defaulted_info.parameter_count);

  Print("Vector type parameter count: {}\n", vector_info.type_parameter_count);
  Print("Array type parameter count: {}\n", array_info.type_parameter_count);
  Print("Value type parameter count: {}\n", value_info.type_parameter_count);
  Print("Defaulted type parameter count: {}\n", defaulted_info.type_parameter_count);

  Print("Vector value parameter count: {}\n", vector_info.value_parameter_count);
  Print("Array value parameter count: {}\n", array_info.value_parameter_count);
  Print("Value value parameter count: {}\n", value_info.value_parameter_count);
  Print("Defaulted value parameter count: {}\n", defaulted_info.value_parameter_count);

  Print("\n");
}

void demonstrate_template_parameters() {
  Print("=== Template Parameters ===\n");

  using VectorInt = std::vector<int>;
  using ArrayDouble5 = FixedArray<double, 5>;

  if constexpr (ctti::variadic_type_template<VectorInt>) {
    constexpr auto vector_info = ctti::get_template_info<VectorInt>();
    Print("Vector type parameters:\n");

    if constexpr (vector_info.type_parameter_count > 0) {
      constexpr auto param_names = vector_info.type_parameter_names();
      for (std::size_t i = 0; i < param_names.size(); ++i) {
        Print("  [{}]: {}\n", i, param_names[i]);
      }
    }
  }

  constexpr auto array_info = ctti::get_template_info<ArrayDouble5>();
  Print("Array parameter count: {}\n", array_info.parameter_count);
  Print("Array type parameter count: {}\n", array_info.type_parameter_count);
  Print("Array value parameter count: {}\n", array_info.value_parameter_count);

  Print("\n");
}

void demonstrate_enum_utilities() {
  Print("=== Enum Utilities ===\n");

  // Compile-time enum info
  Print("Color enum name: {}\n", ctti::enum_type_name<Color>());
  Print("Status enum name: {}\n", ctti::enum_type_name<Status>());

  Print("Color::Red name (compile-time): {}\n", ctti::enum_name<Color, Color::Red>());
  Print("Color::Green name (compile-time): {}\n", ctti::enum_name<Color, Color::Green>());
  Print("Color::Blue name (compile-time): {}\n", ctti::enum_name<Color, Color::Blue>());

  constexpr auto color_info = ctti::get_enum_info<Color>();
  constexpr auto status_info = ctti::get_enum_info<Status>();

  Print("Color is scoped: {}\n", color_info.is_scoped());
  Print("Status is scoped: {}\n", status_info.is_scoped());

  Print("Color::Red underlying: {}\n", ctti::enum_underlying_value<Color, Color::Red>());
  Print("Color::Green underlying (runtime): {}\n", ctti::enum_underlying(Color::Green));

  // Color is registered via ctti::enum_values<Color> specialization
  Print("\nRegistered enum operations (using enum_values<Color>):\n");
  Print("  Color is registered: {}\n", ctti::registered_enum<Color>);
  Print("  Status is registered: {}\n", ctti::registered_enum<Status>);
  Print("  enum_count<Color>(): {}\n", ctti::enum_count<Color>());

  // Runtime name lookup using registered values
  auto red_name = ctti::enum_name(Color::Red);
  Print("  enum_name(Color::Red): {}\n", red_name.value_or("unknown"));

  // String to enum conversion
  auto green_val = ctti::enum_cast<Color>("Green");
  Print("  enum_cast<Color>(\"Green\") valid: {}\n", green_val.has_value());

  // Check if value is valid
  Print("  enum_contains(Color::Blue): {}\n", ctti::enum_contains(Color::Blue));
  Print("  enum_contains(invalid): {}\n", ctti::enum_contains(static_cast<Color>(999)));

  // Index lookup
  auto blue_idx = ctti::enum_index(Color::Blue);
  Print("  enum_index(Color::Blue): {}\n", blue_idx.value_or(999));

  // From underlying value
  auto from_int = ctti::enum_from_underlying<Color>(0);
  Print("  enum_from_underlying<Color>(0) valid: {}\n", from_int.has_value());

  // Access all entries and names
  const auto& entries = ctti::enum_entries<Color>();
  const auto& names = ctti::enum_names<Color>();
  Print("  All entries count: {}\n", entries.size());
  Print("  All names count: {}\n", names.size());

  // Iterate over enum values using registered enum
  Print("  Iterating over colors (enum_for_each):\n");
  ctti::enum_for_each<Color>([](auto index, Color value) {
    auto name = ctti::enum_name(value);
    Print("    [{}] {}\n", index.value, name.value_or("unknown"));
  });

  // === Manual enum_value_list approach (for unregistered enums) ===
  Print("\nManual enum_value_list operations (for Status):\n");
  constexpr auto status_list = ctti::make_enum_list<Status::Active, Status::Inactive, Status::Pending>();
  Print("  Status list count: {}\n", status_list.count);

  auto active_name = status_list.name_of(Status::Active);
  Print("  Status::Active name: {}\n", active_name.value_or("unknown"));

  Print("\n");
}

void demonstrate_constructor_info() {
  Print("=== Constructor Information ===\n");

  constexpr auto point_ctor = ctti::get_constructor_info<Point>();
  constexpr auto vector_ctor = ctti::get_constructor_info<std::vector<int>>();

  Print("Point default constructible: {}\n", point_ctor.is_default_constructible());
  Print("Point copy constructible: {}\n", point_ctor.is_copy_constructible());
  Print("Point move constructible: {}\n", point_ctor.is_move_constructible());
  Print("Point is aggregate: {}\n", point_ctor.is_aggregate());

  Print("Vector default constructible: {}\n", vector_ctor.is_default_constructible());
  Print("Vector copy constructible: {}\n", vector_ctor.is_copy_constructible());
  Print("Vector move constructible: {}\n", vector_ctor.is_move_constructible());
  Print("Vector is aggregate: {}\n", vector_ctor.is_aggregate());

  Print("Point can construct with (double, double): {}\n", point_ctor.can_construct<double, double>());
  Print("Vector can construct with (size_t): {}\n", vector_ctor.can_construct<std::size_t>());

  if constexpr (point_ctor.can_construct<double, double>()) {
    auto constructed_point = point_ctor.construct(1.5, 2.5);
    Print("Constructed point: ({}, {})\n", constructed_point.x, constructed_point.y);
  }

  Print("\n");
}

void demonstrate_type_ids() {
  Print("=== Type IDs ===\n");

  auto int_id = ctti::type_id_of<int>();
  auto point_id = ctti::type_id_of<Point>();

  Print("int type ID: {} (hash: {})\n", int_id.name(), int_id.hash());
  Print("Point type ID: {} (hash: {})\n", point_id.name(), point_id.hash());

  auto vector_id = ctti::type_id_of<std::vector<int>>();
  auto array_id = ctti::type_id_of<FixedArray<double, 5>>();
  auto value_id = ctti::type_id_of<ValueWrapper<42>>();

  Print("vector<int> type ID: {} (hash: {})\n", vector_id.name(), vector_id.hash());
  Print("FixedArray<double, 5> type ID: {} (hash: {})\n", array_id.name(), array_id.hash());
  Print("ValueWrapper<42> type ID: {} (hash: {})\n", value_id.name(), value_id.hash());

  auto int_index1 = ctti::type_index_of<int>();
  auto int_index2 = ctti::type_index_of<int>();
  auto double_index = ctti::type_index_of<double>();

  Print("int index == int index: {}\n", int_index1 == int_index2);
  Print("int index == double index: {}\n", int_index1 == double_index);

  Print("\n");
}

void demonstrate_hash_literals() {
  Print("=== Hash Literals ===\n");

  using namespace ctti::hash_literals;

  constexpr auto hello_hash = "hello"_sh;
  constexpr auto world_hash = "world"_sh;
  constexpr auto template_hash = "template"_sh;

  Print("'hello' hash: {}\n", hello_hash);
  Print("'world' hash: {}\n", world_hash);
  Print("'template' hash: {}\n", template_hash);

  constexpr auto process_command = [](std::string_view cmd) -> std::string_view {
    switch (ctti::fnv1a_hash(cmd)) {
      case "start"_sh:
        return "Starting...";
      case "stop"_sh:
        return "Stopping...";
      case "reset"_sh:
        return "Resetting...";
      case "template"_sh:
        return "Processing template...";
      default:
        return "Unknown command";
    }
  };

  Print("Command 'start': {}\n", process_command("start"));
  Print("Command 'stop': {}\n", process_command("stop"));
  Print("Command 'template': {}\n", process_command("template"));
  Print("Command 'unknown': {}\n", process_command("unknown"));

  Print("\n");
}

void demonstrate_inheritance() {
  Print("=== Inheritance Information ===\n");

  constexpr bool string_derives_from_basic_string = ctti::is_derived_from<std::string, std::basic_string<char>>();
  constexpr bool point_is_polymorphic = ctti::is_polymorphic<Point>();
  constexpr bool point_is_abstract = ctti::is_abstract<Point>();
  constexpr bool point_is_final = ctti::is_final<Point>();

  Print("std::string derives from std::basic_string<char>: {}\n", string_derives_from_basic_string);
  Print("Point is polymorphic: {}\n", point_is_polymorphic);
  Print("Point is abstract: {}\n", point_is_abstract);
  Print("Point is final: {}\n", point_is_final);

  constexpr auto point_poly_info = ctti::get_polymorphism_info<Point>();
  Print("Point polymorphism info:\n");
  Print("  - is_polymorphic: {}\n", point_poly_info.is_polymorphic);
  Print("  - is_abstract: {}\n", point_poly_info.is_abstract);
  Print("  - is_final: {}\n", point_poly_info.is_final);
  Print("  - has_virtual_destructor: {}\n", point_poly_info.has_virtual_destructor);

  Print("\n");
}

}  // namespace

int main() {
  demonstrate_type_names();
  demonstrate_template_info();
  demonstrate_template_parameters();
  demonstrate_constructor_info();
  demonstrate_enum_utilities();
  demonstrate_inheritance();
  demonstrate_type_ids();
  demonstrate_hash_literals();
  demonstrate_symbol_access();
  demonstrate_overloads();
  demonstrate_attributes();
  demonstrate_reflection_iteration();
  demonstrate_mapping();
  demonstrate_tie();
  demonstrate_symbol_utilities();

  return 0;
}
