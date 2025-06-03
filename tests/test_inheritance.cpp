#include "doctest.h"

#include <ctti/inheritance.hpp>

#include <iostream>

struct Base {
  virtual ~Base() = default;
  virtual void virtual_func() {}
};

struct Derived : public Base {
  void virtual_func() override {}
};

struct PrivateDerived : private Base {};

struct MultipleBase1 {
  virtual ~MultipleBase1() = default;
};

struct MultipleBase2 {
  virtual ~MultipleBase2() = default;
};

struct MultipleDerived : public MultipleBase1, public MultipleBase2 {};

struct AbstractBase {
  virtual ~AbstractBase() = default;
  virtual void pure_virtual() = 0;
};

struct ConcreteImpl : public AbstractBase {
  void pure_virtual() override {}
};

struct FinalClass final {};

TEST_SUITE("inheritance") {
  TEST_CASE("inheritance_concepts") {
    CHECK(ctti::derived_from<Derived, Base>);
    CHECK_FALSE(ctti::derived_from<Base, Derived>);
    CHECK_FALSE(ctti::derived_from<int, Base>);

    CHECK(ctti::publicly_derived_from<Derived, Base>);
    CHECK_FALSE(ctti::publicly_derived_from<PrivateDerived, Base>);

    CHECK(ctti::polymorphic<Base>);
    CHECK(ctti::polymorphic<Derived>);
    CHECK_FALSE(ctti::polymorphic<int>);

    CHECK(ctti::abstract<AbstractBase>);
    CHECK_FALSE(ctti::abstract<ConcreteImpl>);
    CHECK_FALSE(ctti::abstract<Base>);

    CHECK(ctti::final_type<FinalClass>);
    CHECK_FALSE(ctti::final_type<Base>);

    CHECK(ctti::has_virtual_destructor<Base>);
    CHECK(ctti::has_virtual_destructor<Derived>);
    CHECK_FALSE(ctti::has_virtual_destructor<int>);
  }

  TEST_CASE("inheritance_info") {
    auto info = ctti::get_inheritance_info<Derived, Base>();

    CHECK(info.is_derived);
    CHECK(info.is_public_derived);
    CHECK_FALSE(info.is_virtual_base);

    CHECK(std::same_as<decltype(info)::derived_type, Derived>);
    CHECK(std::same_as<decltype(info)::base_type, Base>);

    auto derived_name = info.derived_name();
    auto base_name = info.base_name();

    CHECK(derived_name.find("Derived") != std::string_view::npos);
    CHECK(base_name.find("Base") != std::string_view::npos);
  }

  TEST_CASE("base_list") {
    using bases = ctti::base_list<MultipleDerived, MultipleBase1, MultipleBase2>;

    CHECK(bases::count == 2);
    CHECK(std::same_as<bases::base<0>, MultipleBase1>);
    CHECK(std::same_as<bases::base<1>, MultipleBase2>);

    CHECK(bases::has_base<MultipleBase1>());
    CHECK(bases::has_base<MultipleBase2>());
    CHECK_FALSE(bases::has_base<Base>());

    int count = 0;
    bases::for_each_base([&count](auto identity) { ++count; });
    CHECK(count == 2);
  }

  TEST_CASE("polymorphism_info") {
    auto info = ctti::get_polymorphism_info<Base>();

    CHECK(info.is_polymorphic);
    CHECK_FALSE(info.is_abstract);
    CHECK_FALSE(info.is_final);
    CHECK(info.has_virtual_destructor);

    auto abstract_info = ctti::get_polymorphism_info<AbstractBase>();
    CHECK(abstract_info.is_abstract);

    auto final_info = ctti::get_polymorphism_info<FinalClass>();
    CHECK(final_info.is_final);
  }

  TEST_CASE("utility_functions") {
    CHECK(ctti::is_derived_from<Derived, Base>());
    CHECK_FALSE(ctti::is_derived_from<Base, Derived>());

    CHECK(ctti::is_publicly_derived_from<Derived, Base>());
    CHECK_FALSE(ctti::is_publicly_derived_from<PrivateDerived, Base>());

    CHECK(ctti::is_polymorphic<Base>());
    CHECK_FALSE(ctti::is_polymorphic<int>());

    CHECK(ctti::is_abstract<AbstractBase>());
    CHECK_FALSE(ctti::is_abstract<Base>());

    CHECK(ctti::is_final<FinalClass>());
    CHECK_FALSE(ctti::is_final<Base>());
  }

  TEST_CASE("safe_casting") {
    Derived derived;
    Base* base_ptr = &derived;

    auto* derived_ptr = ctti::safe_cast<Derived>(base_ptr);
    CHECK(derived_ptr == &derived);

    const Derived const_derived;
    const Base* const_base_ptr = &const_derived;

    auto* const_derived_ptr = ctti::safe_cast<const Derived>(const_base_ptr);
    CHECK(const_derived_ptr == &const_derived);
  }

  TEST_CASE("dynamic_casting") {
    Derived derived;
    Base* base_ptr = &derived;

    auto* dynamic_derived = ctti::dynamic_cast_safe<Derived>(base_ptr);
    CHECK(dynamic_derived == &derived);

    auto* invalid_cast = ctti::dynamic_cast_safe<MultipleDerived>(base_ptr);
    CHECK(invalid_cast == nullptr);
  }
}
