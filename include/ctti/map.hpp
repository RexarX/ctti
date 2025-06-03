#ifndef CTTI_MAP_HPP
#define CTTI_MAP_HPP

#include <ctti/detail/map_impl.hpp>

namespace ctti {

using default_symbol_mapping_function = detail::DefaultSymbolMappingFunction;

template <typename SourceSymbol, typename SinkSymbol, typename Source, typename Sink, typename Function>
  requires std::invocable<Function, const Source&, SourceSymbol&&, Sink&, SinkSymbol&&>
void map(const Source& source, Sink& sink, const Function& function) noexcept {
  detail::Map<SourceSymbol, SinkSymbol>(source, sink, function);
}

template <typename SourceSymbol, typename SinkSymbol, typename Source, typename Sink>
void map(const Source& source, Sink& sink) noexcept {
  detail::Map<SourceSymbol, SinkSymbol>(source, sink);
}

template <typename SourceSymbol, typename SinkSymbol, typename Function = default_symbol_mapping_function>
using symbol_mapping = detail::SymbolMapping<SourceSymbol, SinkSymbol, Function>;

template <typename SourceSymbol, typename SinkSymbol, typename Function>
constexpr auto make_mapping(Function&& function) noexcept {
  return detail::MakeMapping<SourceSymbol, SinkSymbol>(std::forward<Function>(function));
}

template <typename SourceSymbol, typename SinkSymbol>
constexpr auto make_mapping() noexcept {
  return detail::MakeMapping<SourceSymbol, SinkSymbol>();
}

template <typename Source, typename Sink, typename... Mappings>
void map(const Source& source, Sink& sink, const Mappings&... mappings) noexcept {
  detail::Map(source, sink, mappings...);
}

}  // namespace ctti

#endif  // CTTI_MAP_HPP
