#pragma once

#include <ctti/detail/tie_impl.hpp>

namespace ctti {

/**
 * @brief Ties multiple references to their corresponding compile-time symbols.
 * @tparam Symbols The compile-time symbols to tie the references to.
 * @tparam Refs The types of the references to tie.
 * @param refs The references to tie to the symbols.
 * @return A TieType instance that holds the tied references and their symbols.
 */
template <auto... Symbols, typename... Refs>
  requires(sizeof...(Symbols) == sizeof...(Refs))
[[nodiscard]] constexpr auto tie(Refs&... refs) noexcept
    -> detail::TieType<detail::TypeList<decltype(Symbols)...>, detail::TypeList<Refs...>> {
  return detail::Tie<decltype(Symbols)...>(refs...);
}

}  // namespace ctti
