#pragma once

#include <ctti/detail/map_impl.hpp>

#include <concepts>
#include <utility>

namespace ctti {

using default_symbol_mapping_function = detail::DefaultSymbolMappingFunction;

/**
 * @brief Maps data from a source object to a sink object using a provided mapping function.
 * @tparam SourceSymbol A unique symbol representing the source type.
 * @tparam SinkSymbol A unique symbol representing the sink type.
 * @tparam Source The type of the source object.
 * @tparam Sink The type of the sink object.
 * @tparam Function The type of the mapping function.
 * @param source The source object to map data from.
 * @param sink The sink object to map data to.
 * @param function The mapping function that defines how to map data from the source to the sink.
 */
template <auto SourceSymbol, auto SinkSymbol, typename Source, typename Sink, typename Function>
  requires std::invocable<const Function&, const Source&, decltype(SourceSymbol), Sink&, decltype(SinkSymbol)>
void map(const Source& source, Sink& sink, const Function& function) noexcept(
    noexcept(detail::Map<decltype(SourceSymbol), decltype(SinkSymbol)>(source, sink, function))) {
  detail::Map<decltype(SourceSymbol), decltype(SinkSymbol)>(source, sink, function);
}

/**
 * @brief Maps data from a source object to a sink object using default mapping behavior.
 * @tparam SourceSymbol A unique symbol representing the source type.
 * @tparam SinkSymbol A unique symbol representing the sink type.
 * @tparam Source The type of the source object.
 * @tparam Sink The type of the sink object.
 * @param source The source object to map data from.
 * @param sink The sink object to map data to.
 */
template <auto SourceSymbol, auto SinkSymbol, typename Source, typename Sink>
void map(const Source& source, Sink& sink) noexcept {
  detail::Map<decltype(SourceSymbol), decltype(SinkSymbol)>(source, sink);
}

/**
 * @brief Maps data from a source object to a sink object using multiple mapping definitions.
 * @tparam Source The type of the source object.
 * @tparam Sink The type of the sink object.
 * @tparam Mappings The types of the mapping definitions.
 * @param source The source object to map data from.
 * @param sink The sink object to map data to.
 * @param mappings The mapping definitions that define how to map data from the source to the sink.
 */
template <typename Source, typename Sink, typename... Mappings>
void map(const Source& source, Sink& sink, const Mappings&... mappings) noexcept {
  detail::Map(source, sink, mappings...);
}

/**
 * @brief Creates a mapping object that defines how to map data between two types using a provided function.
 * @tparam SourceSymbol A unique symbol representing the source type.
 * @tparam SinkSymbol A unique symbol representing the sink type.
 * @tparam Function The type of the mapping function.
 * @param function The mapping function that defines how to map data from the source to the sink.
 * @return A mapping object that can be used to map data between the specified source and sink types.
 */
template <auto SourceSymbol, auto SinkSymbol, typename Function>
[[nodiscard]] constexpr auto make_mapping(Function&& function) noexcept {
  return detail::MakeMapping<decltype(SourceSymbol), decltype(SinkSymbol)>(std::forward<Function>(function));
}

/**
 * @brief Creates a mapping object that defines how to map data between two types using default mapping behavior.
 * @tparam SourceSymbol A unique symbol representing the source type.
 * @tparam SinkSymbol A unique symbol representing the sink type.
 * @return A mapping object that can be used to map data between the specified source and sink types.
 */
template <auto SourceSymbol, auto SinkSymbol>
[[nodiscard]] constexpr auto make_mapping() noexcept {
  return detail::MakeMapping<decltype(SourceSymbol), decltype(SinkSymbol)>();
}

template <auto SourceSymbol, auto SinkSymbol, typename Function = default_symbol_mapping_function>
using symbol_mapping = detail::SymbolMapping<decltype(SourceSymbol), decltype(SinkSymbol), Function>;

}  // namespace ctti
