#ifndef CTTI_DETAIL_TIE_IMPL_HPP
#define CTTI_DETAIL_TIE_IMPL_HPP

#include <ctti/detail/meta.hpp>
#include <ctti/detail/symbol_impl.hpp>

#include <tuple>
#include <type_traits>

namespace ctti::detail {

template <typename Symbols, typename Refs>
struct TieType;

template <typename... Symbols, typename... Refs>
struct TieType<TypeList<Symbols...>, TypeList<Refs...>> {
  constexpr TieType(Refs&... refs) noexcept : refs_(refs...) {}

  template <typename T>
  constexpr void operator=(const T& object) noexcept {
    Assign(object, std::index_sequence_for<Symbols...>{});
  }

private:
  template <std::size_t Index>
  using SymbolType = PackGetType<Index, Symbols...>;

  template <std::size_t Index, typename T>
  constexpr void AssignMember(const T& object) noexcept {
    using Symbol = SymbolType<Index>;

    if constexpr (Symbol::template is_owner_of<T>()) {
      auto member_ptr = Symbol::template get_member<T>();

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
    // When Symbol is not a member or member_ptr is nullptr, do nothing
  }

  template <typename T, std::size_t... Indices>
  constexpr void Assign(const T& object, std::index_sequence<Indices...>) noexcept {
    (AssignMember<Indices>(object), ...);
  }

  std::tuple<Refs&...> refs_;
};

template <typename... Symbols, typename... Refs>
constexpr TieType<TypeList<Symbols...>, TypeList<Refs...>> Tie(Refs&... refs) {
  return {refs...};
}

}  // namespace ctti::detail

#endif  // CTTI_DETAIL_TIE_IMPL_HPP
