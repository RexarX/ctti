#include <ctti/ctti.hpp>

#include <print>
#include <string>
#include <vector>

CTTI_DEFINE_SYMBOL(value);
CTTI_DEFINE_SYMBOL(name);
CTTI_DEFINE_SYMBOL(x);
CTTI_DEFINE_SYMBOL(y);
CTTI_DEFINE_SYMBOL(calculate);

struct Point {
  double x = 0.0;
  double y = 0.0;

  double calculate() const { return x * x + y * y; }
};

struct Named {
  std::string name;
  int value = 0;
};

enum class Status { kPending, kActive, kInactive };

void DemonstrateTypeNames() {
  std::print("=== Type Names ===\n");

  std::print("int: {}\n", ctti::name_of<int>());
  std::print("std::string: {}\n", ctti::name_of<std::string>());
  std::print("std::vector<int>: {}\n", ctti::name_of<std::vector<int>>());
  std::print("Point: {}\n", ctti::name_of<Point>());
  std::print("Named: {}\n", ctti::name_of<Named>());

  const auto qualified = ctti::qualified_name_of<std::vector<std::string>>();
  std::print("Qualified full name: {}\n", qualified.get_full_name());
  std::print("Qualified simple name: {}\n", qualified.get_name());

  std::print("\n");
}

void DemonstrateTypeIds() {
  std::print("=== Type IDs ===\n");

  constexpr auto int_id = ctti::type_id_of<int>();
  constexpr auto point_id = ctti::type_id_of<Point>();

  std::print("int type ID: {} (hash: {})\n", int_id.name(), int_id.hash());
  std::print("Point type ID: {} (hash: {})\n", point_id.name(), point_id.hash());

  Point p{3.0, 4.0};
  std::string s = "hello";

  const auto p_id = ctti::type_id_of(p);
  const auto s_id = ctti::type_id_of(s);

  std::print("Point instance type: {}\n", p_id.name());
  std::print("String instance type: {}\n", s_id.name());

  constexpr auto index_int = ctti::type_index_of<int>();
  constexpr auto index_point = ctti::type_index_of<Point>();

  std::print("int index hash: {}\n", index_int.hash());
  std::print("Point index hash: {}\n", index_point.hash());

  static_assert(ctti::type_id_of<int>() != ctti::type_id_of<char>());
  std::print("✓ Type IDs correctly distinguish different types\n");

  std::print("\n");
}

void DemonstrateEnumValues() {
  std::print("=== Enum Value Names ===\n");

  constexpr auto pending_name = ctti::name_of<Status, Status::kPending>();
  constexpr auto active_name = ctti::name_of<Status, Status::kActive>();
  constexpr auto inactive_name = ctti::name_of<Status, Status::kInactive>();

  std::print("Status::kPending: {}\n", pending_name);
  std::print("Status::kActive: {}\n", active_name);
  std::print("Status::kInactive: {}\n", inactive_name);

  auto info = ctti::get_enum_info<Status>();
  std::print("Status is scoped: {}\n", info.is_scoped());
  std::print("Status name: {}\n", info.name());

  std::print("\n");
}

void DemonstrateSymbolAccess() {
  std::print("=== Symbol-Based Member Access ===\n");

  Point p{3.0, 4.0};
  Named n{"example", 42};

  std::print("x is member of Point: {}\n", ctti_symbols::x::is_owner_of<Point>());
  std::print("y is member of Point: {}\n", ctti_symbols::y::is_owner_of<Point>());
  std::print("name is member of Named: {}\n", ctti_symbols::name::is_owner_of<Named>());
  std::print("value is member of Named: {}\n", ctti_symbols::value::is_owner_of<Named>());

  const auto x_val = ctti::get_member_value<ctti_symbols::x>(p);
  const auto y_val = ctti::get_member_value<ctti_symbols::y>(p);
  const auto calculated = ctti::get_member_value<ctti_symbols::calculate>(p);

  std::print("Point x: {}\n", x_val);
  std::print("Point y: {}\n", y_val);
  std::print("Point calculated: {}\n", calculated);

  const auto name_val = ctti::get_member_value<ctti_symbols::name>(n);
  const auto value_val = ctti::get_member_value<ctti_symbols::value>(n);

  std::print("Named name: {}\n", name_val);
  std::print("Named value: {}\n", value_val);

  std::print("\n");
}

void DemonstrateConstructorInfo() {
  std::print("=== Constructor Info ===\n");

  auto point_info = ctti::get_constructor_info<Point>();
  auto named_info = ctti::get_constructor_info<Named>();

  std::print("Point is default constructible: {}\n", point_info.is_default_constructible());
  std::print("Point can construct with (double, double): {}\n", point_info.can_construct<double, double>());

  auto point_obj = point_info.construct(1.5, 2.5);
  std::print("Constructed Point: ({}, {})\n", point_obj.x, point_obj.y);

  auto named_ptr = named_info.make_unique();
  std::print("Named unique_ptr created successfully\n");

  std::print("\n");
}

void DemonstrateHashLiterals() {
  std::print("=== Hash Literals ===\n");

  using namespace ctti::hash_literals;

  constexpr auto hello_hash = "hello"_sh;
  constexpr auto world_hash = "world"_sh;

  std::print("'hello' hash: {}\n", hello_hash);
  std::print("'world' hash: {}\n", world_hash);

  constexpr auto process_command = [](std::string_view cmd) -> std::string_view {
    switch (ctti::fnv1a_hash(cmd)) {
      case "start"_sh:
        return "Starting...";
      case "stop"_sh:
        return "Stopping...";
      case "reset"_sh:
        return "Resetting...";
      default:
        return "Unknown command";
    }
  };

  std::print("Command 'start': {}\n", process_command("start"));
  std::print("Command 'unknown': {}\n", process_command("unknown"));

  std::print("\n");
}

void DemonstrateMapping() {
  std::print("=== Object Mapping ===\n");

  struct Source {
    int value = 42;
    std::string name = "source";
  };

  struct Sink {
    int value = 0;
    std::string name = "sink";
  };

  Source src;
  Sink dst;

  ctti::map<ctti_symbols::value, ctti_symbols::value>(src, dst);
  ctti::map<ctti_symbols::name, ctti_symbols::name>(src, dst);

  std::print("Mapped value: {}\n", dst.value);
  std::print("Mapped name: {}\n", dst.name);

  std::print("\n");
}

void DemonstrateTie() {
  std::print("=== Tie Operations ===\n");

  Point p{3.14, 2.71};

  double x_val = 0.0;
  double y_val = 0.0;

  auto tied = ctti::tie<ctti_symbols::x, ctti_symbols::y>(x_val, y_val);
  tied = p;

  std::print("Tied x: {}\n", x_val);
  std::print("Tied y: {}\n", y_val);

  std::print("\n");
}

int main() {
  DemonstrateTypeNames();
  DemonstrateTypeIds();
  DemonstrateEnumValues();
  DemonstrateSymbolAccess();
  DemonstrateConstructorInfo();
  DemonstrateHashLiterals();
  DemonstrateMapping();
  DemonstrateTie();

  return 0;
}
