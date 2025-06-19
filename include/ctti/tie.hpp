#pragma once

#include <ctti/detail/tie_impl.hpp>

namespace ctti {

template <auto... Symbols, typename... Refs>
  requires(sizeof...(Symbols) == sizeof...(Refs))
constexpr detail::TieType<detail::TypeList<decltype(Symbols)...>, detail::TypeList<Refs...>> tie(
    Refs&... refs) noexcept {
  return detail::Tie<decltype(Symbols)...>(refs...);
}

}  // namespace ctti
