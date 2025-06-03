#ifndef CTTI_TIE_HPP
#define CTTI_TIE_HPP

#include <ctti/detail/tie_impl.hpp>

namespace ctti {

template <typename... Symbols, typename... Refs>
constexpr detail::TieType<detail::TypeList<Symbols...>, detail::TypeList<Refs...>> tie(Refs&... refs) {
  return detail::Tie<Symbols...>(refs...);
}

}  // namespace ctti

#endif  // CTTI_TIE_HPP
