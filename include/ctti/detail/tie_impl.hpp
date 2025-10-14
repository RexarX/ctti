#pragma once

#include <ctti/detail/meta.hpp>
#include <ctti/detail/symbol_impl.hpp>

#include <concepts>
#include <cstddef>
#include <functional>
#include <tuple>
#include <type_traits>

namespace ctti::detail {

template <typename Symbols, typename Refs>
class TieType;

template <typename... Symbols, typename... Refs>
class TieType<TypeList<Symbols...>, TypeList<Refs...>> {
public:
  explicit constexpr TieType(Refs&... refs) noexcept : refs_(refs...) {}
  constexpr TieType(const TieType&) noexcept = default;
  constexpr TieType(TieType&&) noexcept = default;
  constexpr ~TieType() noexcept = default;

  constexpr TieType& operator=(const TieType&) noexcept = default;
  constexpr TieType& operator=(TieType&&) noexcept = default;

  template <typename T>
    requires(!std::same_as<std::remove_cvref_t<T>, TieType>)
  constexpr TieType& operator=(const T& object) noexcept {
    Assign(object, std::index_sequence_for<Symbols...>{});
    return *this;
  }

private:
  template <std::size_t Index>
  using SymbolType = PackGetType<Index, Symbols...>;

  template <std::size_t Index, typename T>
  constexpr void AssignMember(const T& object) noexcept;

  template <typename T, std::size_t... Indices>
  constexpr void Assign(const T& object, std::index_sequence<Indices...> /*sequence*/) noexcept(
      (noexcept(AssignMember<Indices>(object)) && ...)) {
    (AssignMember<Indices>(object), ...);
  }

  std::tuple<Refs&...> refs_;
};

template <typename... Symbols, typename... Refs>
template <std::size_t Index, typename T>
constexpr void TieType<TypeList<Symbols...>, TypeList<Refs...>>::AssignMember(const T& object) noexcept {
  using Symbol = SymbolType<Index>;

  if constexpr (Symbol::template is_owner_of<T>()) {
    const auto member_ptr = Symbol::template get_member<T>();

    if constexpr (!std::same_as<decltype(member_ptr), std::nullptr_t>) {
      if constexpr (std::is_member_object_pointer_v<decltype(member_ptr)>) {
        std::get<Index>(refs_) = object.*member_ptr;
      } else if constexpr (std::is_member_function_pointer_v<decltype(member_ptr)>) {
        if constexpr (std::invocable<decltype(member_ptr), const T&>) {
          std::get<Index>(refs_) = std::invoke(member_ptr, object);
        }
      }
    }
  }
}

template <typename... Symbols, typename... Refs>
[[nodiscard]] constexpr auto Tie(Refs&... refs) noexcept -> TieType<TypeList<Symbols...>, TypeList<Refs...>> {
  return TieType<TypeList<Symbols...>, TypeList<Refs...>>(refs...);
}

}  // namespace ctti::detail
