#pragma once

#include <ctti/detail/map_impl.hpp>

#include <concepts>
#include <utility>

namespace ctti {

using default_symbol_mapping_function = detail::DefaultSymbolMappingFunction;

template <auto SourceSymbol, auto SinkSymbol, typename Source, typename Sink, typename Function>
  requires std::invocable<const Function&, const Source&, decltype(SourceSymbol), Sink&, decltype(SinkSymbol)>
void map(const Source& source, Sink& sink, const Function& function) noexcept(
    noexcept(detail::Map<decltype(SourceSymbol), decltype(SinkSymbol)>(source, sink, function))) {
  detail::Map<decltype(SourceSymbol), decltype(SinkSymbol)>(source, sink, function);
}

template <auto SourceSymbol, auto SinkSymbol, typename Source, typename Sink>
void map(const Source& source, Sink& sink) noexcept {
  detail::Map<decltype(SourceSymbol), decltype(SinkSymbol)>(source, sink);
}

template <auto SourceSymbol, auto SinkSymbol, typename Function = default_symbol_mapping_function>
using symbol_mapping = detail::SymbolMapping<decltype(SourceSymbol), decltype(SinkSymbol), Function>;

template <auto SourceSymbol, auto SinkSymbol, typename Function>
constexpr auto make_mapping(Function&& function) noexcept {
  return detail::MakeMapping<decltype(SourceSymbol), decltype(SinkSymbol)>(std::forward<Function>(function));
}

template <auto SourceSymbol, auto SinkSymbol>
constexpr auto make_mapping() noexcept {
  return detail::MakeMapping<decltype(SourceSymbol), decltype(SinkSymbol)>();
}

template <typename Source, typename Sink, typename... Mappings>
void map(const Source& source, Sink& sink, const Mappings&... mappings) noexcept {
  detail::Map(source, sink, mappings...);
}

}  // namespace ctti
