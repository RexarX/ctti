#ifndef CTTI_TIE_HPP
#define CTTI_TIE_HPP

#include <ctti/detail/tie_impl.hpp>

namespace ctti {

template <auto... Symbols, typename... Refs>
constexpr detail::TieType<detail::TypeList<decltype(Symbols)...>, detail::TypeList<Refs...>> tie(Refs&... refs) {
  return detail::Tie<decltype(Symbols)...>(refs...);
}

}  // namespace ctti

#endif  // CTTI_TIE_HPP
