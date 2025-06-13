#include <ctti/ctti.hpp>

#include <array>
#include <memory>
#include <optional>
#include <print>
#include <string>
#include <vector>

struct Point {
  double x = 0.0;
  double y = 0.0;

  double calculate() const { return x * x + y * y; }
  void set_coordinates(double new_x, double new_y) {
    x = new_x;
    y = new_y;
  }
};

template <>
struct ctti::meta<Point> {
  using type = Point;

  static constexpr auto reflection = ctti::make_reflection(
      ctti::member<"x", &Point::x>(), ctti::member<"y", &Point::y>(ctti::read_only{}),
      ctti::member<"calculate", &Point::calculate>(), ctti::member<"set_coordinates", &Point::set_coordinates>());
};

struct Calculator {
  int add(int a, int b) { return a + b; }
  double add(double a, double b) { return a + b; }
  std::string add(const std::string& a, const std::string& b) { return a + b; }
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

struct Named {
  std::string name;
  int value = 0;
};

template <>
struct ctti::meta<Named> {
  using type = Named;

  static constexpr auto reflection =
      ctti::make_reflection(ctti::member<"name", &Named::name>(ctti::validated{}),
                            ctti::member<"value", &Named::value>(ctti::attribute_value<1>{}));
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

void demonstrate_symbol_access() {
  std::print("=== Symbol-Based Member Access ===\n");

  Point p{3.0, 4.0};
  Named n{"example", 42};

  const auto x_val = ctti::get_symbol_value<"x">(p);
  const auto y_val = ctti::get_symbol_value<"y">(p);
  constexpr auto calculate_symbol = ctti::get_reflected_symbol<"calculate", Point>();
  const auto calculated = calculate_symbol.call(p);

  std::print("Point x: {}\n", x_val);
  std::print("Point y: {}\n", y_val);
  std::print("Point calculated: {}\n", calculated);

  const auto name_val = ctti::get_symbol_value<"name">(n);
  const auto value_val = ctti::get_symbol_value<"value">(n);

  std::print("Named name: {}\n", name_val);
  std::print("Named value: {}\n", value_val);

  std::print("\n");
}

void demonstrate_overloads() {
  std::print("=== Method Overloads ===\n");

  Calculator calc;

  constexpr auto add_symbol = ctti::get_reflected_symbol<"add", Calculator>();

  int result1 = add_symbol.call(calc, 5, 3);
  double result2 = add_symbol.call(calc, 5.5, 3.2);
  std::string result3 = add_symbol.call(calc, std::string("Hello"), std::string(" World"));

  std::print("add(5, 3) = {}\n", result1);
  std::print("add(5.5, 3.2) = {}\n", result2);
  std::print("add(\"Hello\", \" World\") = {}\n", result3);

  std::print("Can call with int(int, int): {}\n", add_symbol.has_overload<int(int, int)>());
  std::print("Can call with string(const string&, const string&): {}\n",
             add_symbol.has_overload<std::string(const std::string&, const std::string&)>());
  std::print("Can call with double(double, double): {}\n", add_symbol.has_overload<double(double, double)>());

  std::print("\n");
}

void demonstrate_attributes() {
  std::print("=== Attributes ===\n");

  constexpr auto y_symbol = ctti::get_reflected_symbol<"y", Point>();
  constexpr auto name_symbol = ctti::get_reflected_symbol<"name", Named>();
  constexpr auto value_symbol = ctti::get_reflected_symbol<"value", Named>();

  constexpr bool y_is_readonly = y_symbol.has_attribute<ctti::read_only>();
  constexpr bool name_is_validated = name_symbol.has_attribute<ctti::validated>();
  constexpr bool value_has_version = value_symbol.has_attribute_value<1>();

  std::print("Point.y is read-only: {}\n", y_is_readonly);
  std::print("Named.name is validated: {}\n", name_is_validated);
  std::print("Named.value has version 1: {}\n", value_has_version);

  std::print("\n");
}

void demonstrate_reflection_iteration() {
  std::print("=== Reflection Iteration ===\n");

  std::print("Point symbols:\n");
  ctti::for_each_symbol<Point>([](auto symbol) { std::print("  - {}\n", symbol.name); });

  std::print("Named symbols:\n");
  ctti::for_each_symbol<Named>([](auto symbol) { std::print("  - {}\n", symbol.name); });

  constexpr auto point_symbol_names = ctti::get_symbol_names<Point>();
  std::print("Point has {} symbols\n", point_symbol_names.size());

  std::print("\n");
}

void demonstrate_mapping() {
  std::print("=== Object Mapping ===\n");

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

  std::print("After direct mapping:\n");
  std::print("  dst.value: {}\n", dst.value);
  std::print("  dst.name: {}\n", dst.name);
  std::print("  dst.active: {}\n", dst.active);

  auto custom_mapping = ctti::make_mapping<price_src_symbol, active_dst_symbol>(
      [](const Source& source, auto src_sym, Sink& sink, auto dst_sym) {
        auto price_member = src_sym.get_member<Source>();
        auto active_member = dst_sym.get_member<Sink>();
        sink.*active_member = source.*price_member > 50.0;
      });

  custom_mapping(src, dst);

  std::print("After custom mapping:\n");
  std::print("  dst.active: {} (price {} > 50.0)\n", dst.active, src.price);

  std::print("\n");
}

void demonstrate_tie() {
  std::print("=== Tie Functionality ===\n");

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

  std::print("Tied values:\n");
  std::print("  val: {}\n", val);
  std::print("  nm: {}\n", nm);
  std::print("  wt: {}\n", wt);

  std::print("\n");
}

void demonstrate_symbol_utilities() {
  std::print("=== Symbol Utilities ===\n");

  constexpr auto value_symbol = ctti::make_simple_symbol<"value", &Point::x>();
  constexpr auto calc_symbol = ctti::make_simple_symbol<"calculate", &Point::calculate>();

  std::print("Symbol names:\n");
  std::print("  value_symbol: {}\n", value_symbol.name);
  std::print("  calc_symbol: {}\n", calc_symbol.name);

  std::print("Symbol hashes:\n");
  std::print("  value_symbol: {}\n", value_symbol.hash);
  std::print("  calc_symbol: {}\n", calc_symbol.hash);

  std::print("Symbol overload counts:\n");
  std::print("  value_symbol: {}\n", value_symbol.overload_count);
  std::print("  calc_symbol: {}\n", calc_symbol.overload_count);

  std::print("Symbol ownership:\n");
  std::print("  value_symbol owns Point: {}\n", value_symbol.is_owner_of<Point>());
  std::print("  value_symbol owns int: {}\n", value_symbol.is_owner_of<int>());

  Point p{5.0, 12.0};
  auto val = value_symbol.get_value(p);
  std::print("Retrieved value using symbol: {}\n", val);

  value_symbol.set_value(p, 7.0);
  std::print("After setting value to 7.0: {}\n", p.x);

  std::print("Symbol method overload checking:\n");
  std::print("  calc_symbol has double() const: {}\n", calc_symbol.has_overload<double() const>());
  std::print("  calc_symbol has int(): {}\n", calc_symbol.has_overload<int()>());

  std::print("\n");
}

void demonstrate_type_names() {
  std::print("=== Type Names ===\n");

  std::print("int: {}\n", ctti::name_of<int>());
  std::print("Point: {}\n", ctti::name_of<Point>());
  std::print("Named: {}\n", ctti::name_of<Named>());
  std::print("Calculator: {}\n", ctti::name_of<Calculator>());

  std::print("std::vector<int>: {}\n", ctti::name_of<std::vector<int>>());
  std::print("std::optional<std::string>: {}\n", ctti::name_of<std::optional<std::string>>());
  std::print("Container<int>: {}\n", ctti::name_of<Container<int>>());
  std::print("FixedArray<double, 5>: {}\n", ctti::name_of<FixedArray<double, 5>>());
  std::print("ValueWrapper<42>: {}\n", ctti::name_of<ValueWrapper<42>>());
  std::print("DefaultedContainer<int, 100>: {}\n", ctti::name_of<DefaultedContainer<int, 100>>());

  std::print("\n");
}

void demonstrate_template_info() {
  std::print("=== Template Information ===\n");

  using VectorInt = std::vector<int>;
  using ArrayDouble5 = FixedArray<double, 5>;
  using ValueInt42 = ValueWrapper<42>;
  using DefaultedIntContainer = DefaultedContainer<int, 100>;

  constexpr bool vector_is_template = ctti::is_template_instantiation<VectorInt>();
  constexpr bool array_is_template = ctti::is_template_instantiation<ArrayDouble5>();
  constexpr bool value_is_template = ctti::is_template_instantiation<ValueInt42>();
  constexpr bool defaulted_is_template = ctti::is_template_instantiation<DefaultedIntContainer>();
  constexpr bool int_is_template = ctti::is_template_instantiation<int>();

  std::print("std::vector<int> is template: {}\n", vector_is_template);
  std::print("FixedArray<double, 5> is template: {}\n", array_is_template);
  std::print("ValueWrapper<42> is template: {}\n", value_is_template);
  std::print("DefaultedContainer<int, 100> is template: {}\n", defaulted_is_template);
  std::print("int is template: {}\n", int_is_template);

  constexpr auto vector_info = ctti::get_template_info<VectorInt>();
  constexpr auto array_info = ctti::get_template_info<ArrayDouble5>();
  constexpr auto value_info = ctti::get_template_info<ValueInt42>();
  constexpr auto defaulted_info = ctti::get_template_info<DefaultedIntContainer>();

  std::print("Vector parameter count: {}\n", vector_info.parameter_count);
  std::print("Array parameter count: {}\n", array_info.parameter_count);
  std::print("Value parameter count: {}\n", value_info.parameter_count);
  std::print("Defaulted parameter count: {}\n", defaulted_info.parameter_count);

  std::print("Vector type parameter count: {}\n", vector_info.type_parameter_count);
  std::print("Array type parameter count: {}\n", array_info.type_parameter_count);
  std::print("Value type parameter count: {}\n", value_info.type_parameter_count);
  std::print("Defaulted type parameter count: {}\n", defaulted_info.type_parameter_count);

  std::print("Vector value parameter count: {}\n", vector_info.value_parameter_count);
  std::print("Array value parameter count: {}\n", array_info.value_parameter_count);
  std::print("Value value parameter count: {}\n", value_info.value_parameter_count);
  std::print("Defaulted value parameter count: {}\n", defaulted_info.value_parameter_count);

  std::print("\n");
}

void demonstrate_template_parameters() {
  std::print("=== Template Parameters ===\n");

  using VectorInt = std::vector<int>;
  using ArrayDouble5 = FixedArray<double, 5>;

  if constexpr (ctti::variadic_type_template<VectorInt>) {
    constexpr auto vector_info = ctti::get_template_info<VectorInt>();
    std::print("Vector type parameters:\n");

    if constexpr (vector_info.type_parameter_count > 0) {
      constexpr auto param_names = vector_info.type_parameter_names();
      for (std::size_t i = 0; i < param_names.size(); ++i) {
        std::print("  [{}]: {}\n", i, param_names[i]);
      }
    }
  }

  constexpr auto array_info = ctti::get_template_info<ArrayDouble5>();
  std::print("Array parameter count: {}\n", array_info.parameter_count);
  std::print("Array type parameter count: {}\n", array_info.type_parameter_count);
  std::print("Array value parameter count: {}\n", array_info.value_parameter_count);

  std::print("\n");
}

void demonstrate_enum_utilities() {
  std::print("=== Enum Utilities ===\n");

  std::print("Color enum name: {}\n", ctti::enum_type_name<Color>());
  std::print("Status enum name: {}\n", ctti::enum_type_name<Status>());

  std::print("Color::Red name: {}\n", ctti::enum_name<Color, Color::Red>());
  std::print("Color::Green name: {}\n", ctti::enum_name<Color, Color::Green>());
  std::print("Color::Blue name: {}\n", ctti::enum_name<Color, Color::Blue>());

  std::print("Status::Active name: {}\n", ctti::enum_name<Status, Status::Active>());
  std::print("Status::Inactive name: {}\n", ctti::enum_name<Status, Status::Inactive>());
  std::print("Status::Pending name: {}\n", ctti::enum_name<Status, Status::Pending>());

  constexpr auto color_info = ctti::get_enum_info<Color>();
  constexpr auto status_info = ctti::get_enum_info<Status>();

  std::print("Color is scoped: {}\n", color_info.is_scoped());
  std::print("Status is scoped: {}\n", status_info.is_scoped());

  std::print("Color::Red underlying: {}\n", ctti::enum_underlying_value<Color, Color::Red>());
  std::print("Status::Active underlying: {}\n", ctti::enum_underlying_value<Status, Status::Active>());

  std::print("\n");
}

void demonstrate_constructor_info() {
  std::print("=== Constructor Information ===\n");

  constexpr auto point_ctor = ctti::get_constructor_info<Point>();
  constexpr auto vector_ctor = ctti::get_constructor_info<std::vector<int>>();

  std::print("Point default constructible: {}\n", point_ctor.is_default_constructible());
  std::print("Point copy constructible: {}\n", point_ctor.is_copy_constructible());
  std::print("Point move constructible: {}\n", point_ctor.is_move_constructible());
  std::print("Point is aggregate: {}\n", point_ctor.is_aggregate());

  std::print("Vector default constructible: {}\n", vector_ctor.is_default_constructible());
  std::print("Vector copy constructible: {}\n", vector_ctor.is_copy_constructible());
  std::print("Vector move constructible: {}\n", vector_ctor.is_move_constructible());
  std::print("Vector is aggregate: {}\n", vector_ctor.is_aggregate());

  std::print("Point can construct with (double, double): {}\n", point_ctor.can_construct<double, double>());
  std::print("Vector can construct with (size_t): {}\n", vector_ctor.can_construct<std::size_t>());

  if constexpr (point_ctor.can_construct<double, double>()) {
    auto constructed_point = point_ctor.construct(1.5, 2.5);
    std::print("Constructed point: ({}, {})\n", constructed_point.x, constructed_point.y);
  }

  std::print("\n");
}

void demonstrate_type_ids() {
  std::print("=== Type IDs ===\n");

  auto int_id = ctti::type_id_of<int>();
  auto point_id = ctti::type_id_of<Point>();

  std::print("int type ID: {} (hash: {})\n", int_id.name(), int_id.hash());
  std::print("Point type ID: {} (hash: {})\n", point_id.name(), point_id.hash());

  auto vector_id = ctti::type_id_of<std::vector<int>>();
  auto array_id = ctti::type_id_of<FixedArray<double, 5>>();
  auto value_id = ctti::type_id_of<ValueWrapper<42>>();

  std::print("vector<int> type ID: {} (hash: {})\n", vector_id.name(), vector_id.hash());
  std::print("FixedArray<double, 5> type ID: {} (hash: {})\n", array_id.name(), array_id.hash());
  std::print("ValueWrapper<42> type ID: {} (hash: {})\n", value_id.name(), value_id.hash());

  auto int_index1 = ctti::type_index_of<int>();
  auto int_index2 = ctti::type_index_of<int>();
  auto double_index = ctti::type_index_of<double>();

  std::print("int index == int index: {}\n", int_index1 == int_index2);
  std::print("int index == double index: {}\n", int_index1 == double_index);

  std::print("\n");
}

void demonstrate_hash_literals() {
  std::print("=== Hash Literals ===\n");

  using namespace ctti::hash_literals;

  constexpr auto hello_hash = "hello"_sh;
  constexpr auto world_hash = "world"_sh;
  constexpr auto template_hash = "template"_sh;

  std::print("'hello' hash: {}\n", hello_hash);
  std::print("'world' hash: {}\n", world_hash);
  std::print("'template' hash: {}\n", template_hash);

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

  std::print("Command 'start': {}\n", process_command("start"));
  std::print("Command 'stop': {}\n", process_command("stop"));
  std::print("Command 'template': {}\n", process_command("template"));
  std::print("Command 'unknown': {}\n", process_command("unknown"));

  std::print("\n");
}

void demonstrate_inheritance() {
  std::print("=== Inheritance Information ===\n");

  constexpr bool string_derives_from_basic_string = ctti::is_derived_from<std::string, std::basic_string<char>>();
  constexpr bool point_is_polymorphic = ctti::is_polymorphic<Point>();
  constexpr bool point_is_abstract = ctti::is_abstract<Point>();
  constexpr bool point_is_final = ctti::is_final<Point>();

  std::print("std::string derives from std::basic_string<char>: {}\n", string_derives_from_basic_string);
  std::print("Point is polymorphic: {}\n", point_is_polymorphic);
  std::print("Point is abstract: {}\n", point_is_abstract);
  std::print("Point is final: {}\n", point_is_final);

  constexpr auto point_poly_info = ctti::get_polymorphism_info<Point>();
  std::print("Point polymorphism info:\n");
  std::print("  - is_polymorphic: {}\n", point_poly_info.is_polymorphic);
  std::print("  - is_abstract: {}\n", point_poly_info.is_abstract);
  std::print("  - is_final: {}\n", point_poly_info.is_final);
  std::print("  - has_virtual_destructor: {}\n", point_poly_info.has_virtual_destructor);

  std::print("\n");
}

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
