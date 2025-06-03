#ifndef CTTI_DETAIL_META_HPP
#define CTTI_DETAIL_META_HPP

#include <ctti/detail/concepts_impl.hpp>

#include <array>
#include <cstddef>
#include <cstdint>
#include <type_traits>
#include <utility>

namespace ctti::detail {

template <typename T>
using DecayType = std::decay_t<T>;

template <typename... Ts>
using VoidType = std::void_t<Ts...>;

template <typename... Bs>
concept AllTrue = (Bs::value && ...);

template <typename T>
struct Identity {
  using type = T;
};

template <typename Metafunction>
using TypeOf = typename Metafunction::type;

template <bool Cond, typename T = void>
using EnableIfType = std::enable_if_t<Cond, T>;

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

template <std::size_t I>
using SizeType = std::integral_constant<std::size_t, I>;

template <bool B>
using BoolType = std::integral_constant<bool, B>;
using TrueType = BoolType<true>;
using FalseType = BoolType<false>;

template <char C>
using CharType = std::integral_constant<char, C>;

template <typename... Symbols>
struct Model;

template <IntegralConstantType T>
consteval decltype(T::value) GetValue() noexcept {
  return T::value;
}

template <typename... Ts>
class TypeList {
public:
  static constexpr std::size_t kSize = sizeof...(Ts);

  template <std::size_t I>
    requires(I < kSize)
  using At = std::tuple_element_t<I, std::tuple<Ts...>>;

  template <template <typename> typename Predicate>
  static constexpr bool kAllSatisfy = (Predicate<Ts>::value && ...);

  template <template <typename> typename Predicate>
  static constexpr bool kAnySatisfy = (Predicate<Ts>::value || ...);

  template <typename F>
  static constexpr void ForEach(F&& f) {
    if constexpr (requires { (f.template operator()<Ts>(), ...); }) {
      (f.template operator()<Ts>(), ...);
    } else {
      (f(Identity<Ts>{}), ...);
    }
  }
};

template <std::size_t I, typename... Ts>
using PackGetType = std::tuple_element_t<I, std::tuple<Ts...>>;

template <typename List>
struct ListSize;

template <typename... Symbols>
struct ListSize<Model<Symbols...>> : public SizeType<sizeof...(Symbols)> {};

template <template <typename...> class Sequence, typename... Ts>
struct ListSize<Sequence<Ts...>> : public SizeType<sizeof...(Ts)> {};

template <typename... Ts>
struct ListSize<TypeList<Ts...>> : public SizeType<sizeof...(Ts)> {};

template <char... Chars>
using String = TypeList<std::integral_constant<char, Chars>...>;

template <typename Key, typename Value>
struct Pair {
  using key = Key;
  using value = Value;

  static consteval Pair<Key, Value> Make(const Key&, const Value&) { return {}; }
};

template <typename TPair>
using KeyType = typename TPair::key;

template <typename TPair>
using ValueType = typename TPair::value;

template <typename... Ts>
struct Inherit : Ts... {};

template <typename... Ts>
struct Inherit<TypeList<Ts...>> : Ts... {};

template <typename Lhs, typename Rhs>
struct Cat;

template <template <typename...> class Seq, typename... Lhs, typename... Rhs>
struct Cat<Seq<Lhs...>, Seq<Rhs...>> {
  using type = Seq<Lhs..., Rhs...>;
};

template <typename Lhs, typename Rhs>
using CatType = TypeOf<Cat<Lhs, Rhs>>;

template <std::size_t I>
using IndexType = std::integral_constant<std::size_t, I>;

template <typename T, T... Values>
using IntegerSequence = TypeList<std::integral_constant<T, Values>...>;

template <std::size_t... Is>
using IndexSequence = TypeList<IndexType<Is>...>;

template <typename T, typename List>
struct Contains;

template <typename T, template <typename...> class Seq, typename... Ts>
struct Contains<T, Seq<Ts...>> : std::bool_constant<(std::same_as<T, Ts> || ...)> {};

template <typename T, typename List>
constexpr bool kContains = Contains<T, List>::value;

}  // namespace ctti::detail

#endif  // CTTI_DETAIL_META_HPP
