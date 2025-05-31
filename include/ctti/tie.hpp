#ifndef CTTI_TIE_HPP
#define CTTI_TIE_HPP

#include <ctti/detail/meta.hpp>
#include <ctti/symbol.hpp>

#include <tuple>

namespace ctti {

namespace detail {

template <typename Symbols, typename Refs>
struct tie_t;

template <typename... Symbols, typename... Refs>
struct tie_t<ctti::meta::list<Symbols...>, ctti::meta::list<Refs...>> {
  constexpr tie_t(Refs&... refs) : _refs{refs...} {}

  template <typename T>
  void operator=(const T& object) {
    assign(object, std::index_sequence_for<Symbols...>{});
  }

private:
  template <std::size_t Index>
  using Symbol = ctti::meta::pack_get_t<Index, Symbols...>;

  template <std::size_t Index, typename T>
    requires(Symbol<Index>::template is_member_of<T>())
  void assign_member(const T& object) {
    std::get<Index>(_refs) = ctti::get_member_value<Symbol<Index>>(object);
  }

  template <std::size_t Index, typename T>
    requires(!Symbol<Index>::template is_member_of<T>())
  void assign_member(const T&) {
    // Do nothing - symbol is not a member
  }

  template <typename T, std::size_t... Indices>
  void assign(const T& object, std::index_sequence<Indices...>) {
    (assign_member<Indices>(object), ...);
  }

  std::tuple<Refs&...> _refs;
};

}  // namespace detail

template <typename... Symbols, typename... Refs>
constexpr ctti::detail::tie_t<ctti::meta::list<Symbols...>, ctti::meta::list<Refs...>> tie(Refs&... refs) {
  return {refs...};
}

}  // namespace ctti

#endif  // CTTI_TIE_HPP
