#pragma once

#include <concepts>
#include <cstddef>
#include <cstdint>
#include <tuple>
#include <type_traits>

namespace ctti::detail {

template <typename T>
using DecayType = std::decay_t<T>;

template <typename... Ts>
using VoidType = std::void_t<Ts...>;

template <typename T>
struct Identity {
  using type = T;
};

template <typename Metafunction>
using TypeOf = typename Metafunction::type;

template <std::size_t I>
using SizeType = std::integral_constant<std::size_t, I>;

template <bool B>
using BoolType = std::integral_constant<bool, B>;
using TrueType = BoolType<true>;
using FalseType = BoolType<false>;

template <char C>
using CharType = std::integral_constant<char, C>;

template <typename T>
concept IntegralConstantType = requires {
  typename T::value_type;
  { T::value } -> std::convertible_to<typename T::value_type>;
};

template <typename... Ts>
struct TypeList {
  static constexpr std::size_t kSize = sizeof...(Ts);

  template <std::size_t I>
    requires(I < kSize)
  using At = std::tuple_element_t<I, std::tuple<Ts...>>;

  template <template <typename> typename Predicate>
  static constexpr bool kAllSatisfy = (Predicate<Ts>::value && ...);

  template <template <typename> typename Predicate>
  static constexpr bool kAnySatisfy = (Predicate<Ts>::value || ...);

  template <typename F>
    requires((std::invocable<const F&, Identity<Ts>> && ...) || (std::invocable<const F&, Ts> && ...))
  static constexpr void ForEach(const F& func) {
    if constexpr (requires { (func.template operator()<Ts>(), ...); }) {
      (func.template operator()<Ts>(), ...);
    } else {
      (func(Identity<Ts>{}), ...);
    }
  }
};

template <std::size_t I, typename... Ts>
using PackGetType = std::tuple_element_t<I, std::tuple<Ts...>>;

template <typename Key, typename Value>
struct Pair {
  using key = Key;
  using value = Value;

  static constexpr Pair Make(const Key& /*key*/, const Value& /*value*/) noexcept { return {}; }
};

template <typename Pair>
using KeyType = typename Pair::key;

template <typename Pair>
using ValueType = typename Pair::value;

template <typename T, typename List>
struct Contains;

template <typename T, template <typename...> class Seq, typename... Ts>
struct Contains<T, Seq<Ts...>> : std::bool_constant<(std::same_as<T, Ts> || ...)> {};

template <typename T, typename List>
constexpr bool kContains = Contains<T, List>::value;

template <typename Lhs, typename Rhs>
struct Cat;

template <template <typename...> class Seq, typename... Lhs, typename... Rhs>
struct Cat<Seq<Lhs...>, Seq<Rhs...>> {
  using type = Seq<Lhs..., Rhs...>;
};

template <typename Lhs, typename Rhs>
using CatType = TypeOf<Cat<Lhs, Rhs>>;

// Helper for concatenating multiple TypeLists
template <typename... Lists>
struct CatMany;

template <typename First>
struct CatMany<First> {
  using type = First;
};

template <typename First, typename Second, typename... Rest>
struct CatMany<First, Second, Rest...> {
  using type = typename CatMany<CatType<First, Second>, Rest...>::type;
};

template <typename... Lists>
using CatManyType = typename CatMany<Lists...>::type;

template <std::uint8_t I>
using UInt8 = std::integral_constant<std::uint8_t, I>;

template <std::uint16_t I>
using UInt16 = std::integral_constant<std::uint16_t, I>;

template <std::uint32_t I>
using UInt32 = std::integral_constant<std::uint32_t, I>;

template <std::uint64_t I>
using UInt64 = std::integral_constant<std::uint64_t, I>;

template <std::int8_t I>
using Int8 = std::integral_constant<std::int8_t, I>;

template <std::int16_t I>
using Int16 = std::integral_constant<std::int16_t, I>;

template <std::int32_t I>
using Int32 = std::integral_constant<std::int32_t, I>;

template <std::int64_t I>
using Int64 = std::integral_constant<std::int64_t, I>;

template <typename T>
[[nodiscard]] constexpr auto GetValue() noexcept {
  return T::value;
}

template <std::size_t I>
using IndexType = std::integral_constant<std::size_t, I>;

template <typename T, T... Values>
struct IntegerSequence {
  static constexpr std::size_t kSize = sizeof...(Values);

  template <std::size_t I>
    requires(I < kSize)
  using At = std::integral_constant<T, std::get<I>(std::make_tuple(std::integral_constant<T, Values>{}...))>;
};

template <std::size_t... Indices>
using IndexSequence = IntegerSequence<std::size_t, Indices...>;

template <char... Chars>
using String = IntegerSequence<char, Chars...>;

template <typename... Ts>
concept AllTrue = (Ts::value && ...);

template <typename List>
struct ListSize;

template <typename... Ts>
struct ListSize<TypeList<Ts...>> : SizeType<sizeof...(Ts)> {};

template <typename... Ts>
struct ListSize<std::tuple<Ts...>> : SizeType<sizeof...(Ts)> {};

template <typename... Ts>
struct Inherit : Ts... {};

template <typename List>
struct InheritFromList;

template <typename... Ts>
struct InheritFromList<TypeList<Ts...>> : Ts... {};

template <>
struct Inherit<TypeList<>> {};

template <typename... Ts>
struct Inherit<TypeList<Ts...>> : Ts... {};

}  // namespace ctti::detail
