#include "catch.hpp"

#include <ctti/model.hpp>
#include <ctti/tie.hpp>

#include <string>
#include <vector>

CTTI_DEFINE_SYMBOL(name);
CTTI_DEFINE_SYMBOL(age);
CTTI_DEFINE_SYMBOL(addresses);
CTTI_DEFINE_SYMBOL(city);
CTTI_DEFINE_SYMBOL(street);
CTTI_DEFINE_SYMBOL(zip);
CTTI_DEFINE_SYMBOL(department);
CTTI_DEFINE_SYMBOL(salary);
CTTI_DEFINE_SYMBOL(manager);

struct Address {
  std::string city;
  std::string street;
  std::string zip;

  using ctti_model = ctti::model<::city, ::street, ::zip>;
};

struct Employee {
  std::string name;
  int age = 0;
  std::vector<Address> addresses;
  std::string department;
  double salary = 0.0;
  bool manager = false;

  using ctti_model = ctti::model<::name, ::age, ::addresses, ::department, ::salary, ::manager>;
};

TEST_CASE("complex model and tie") {
  Employee employee;
  employee.name = "John Smith";
  employee.age = 42;
  employee.addresses = {{"New York", "Broadway", "10001"}, {"Boston", "Main St", "02108"}};
  employee.department = "Engineering";
  employee.salary = 120000.0;
  employee.manager = true;

  SECTION("tie with employee") {
    std::string name;
    int age = 0;
    std::vector<Address> addresses;
    std::string department;
    double salary = 0.0;
    bool manager = false;

    ctti::tie<::name, ::age, ::addresses, ::department, ::salary, ::manager>(name, age, addresses, department, salary,
                                                                             manager) = employee;

    REQUIRE(name == "John Smith");
    REQUIRE(age == 42);
    REQUIRE(addresses.size() == 2);
    REQUIRE(addresses[0].city == "New York");
    REQUIRE(addresses[1].street == "Main St");
    REQUIRE(department == "Engineering");
    REQUIRE(salary == 120000.0);
    REQUIRE(manager == true);
  }

  SECTION("partial tie") {
    std::string name;
    double salary = 0.0;

    ctti::tie<::name, ::salary>(name, salary) = employee;

    REQUIRE(name == "John Smith");
    REQUIRE(salary == 120000.0);
  }

  SECTION("address tie") {
    std::string city;
    std::string street;
    std::string zip;

    ctti::tie<::city, ::street, ::zip>(city, street, zip) = employee.addresses[0];

    REQUIRE(city == "New York");
    REQUIRE(street == "Broadway");
    REQUIRE(zip == "10001");
  }
}
