#pragma once

#include <ctti/detail/symbol_impl.hpp>

#include <concepts>
#include <type_traits>
#include <utility>

namespace ctti::detail {

struct DefaultSymbolMappingFunction {
  constexpr DefaultSymbolMappingFunction() noexcept = default;

  template <typename Source, typename SourceSymbol, typename Sink, typename SinkSymbol>
  void operator()(const Source& source, [[maybe_unused]] SourceSymbol src_symbol, Sink& sink,
                  [[maybe_unused]] SinkSymbol sink_symbol) const noexcept {
    if constexpr (SourceSymbol::template is_owner_of<Source>() && SinkSymbol::template is_owner_of<Sink>()) {
      sink.*(SinkSymbol::template get_member<Sink>()) = source.*(SourceSymbol::template get_member<Source>());
    }
  }
};

template <typename SourceSymbol, typename SinkSymbol, typename Source, typename Sink, typename Function>
  requires std::invocable<const Function&, const Source&, SourceSymbol&&, Sink&, SinkSymbol&&>
void Map(const Source& source, Sink& sink, const Function& function) noexcept(
    std::is_nothrow_invocable_v<const Function&, const Source&, SourceSymbol, Sink&, SinkSymbol>) {
  function(source, SourceSymbol(), sink, SinkSymbol());
}

template <typename SourceSymbol, typename SinkSymbol, typename Source, typename Sink>
void Map(const Source& source, Sink& sink) noexcept(
    std::is_nothrow_invocable_v<DefaultSymbolMappingFunction, const Source&, SourceSymbol, Sink&, SinkSymbol>) {
  Map<SourceSymbol, SinkSymbol>(source, sink, DefaultSymbolMappingFunction());
}

template <typename SourceSymbol, typename SinkSymbol, typename Function = DefaultSymbolMappingFunction>
class SymbolMapping {
public:
  constexpr SymbolMapping() noexcept
    requires std::constructible_from<Function>
      : function_{} {}
  explicit constexpr SymbolMapping(Function&& function) noexcept : function_(std::forward<Function>(function)) {}

  constexpr SymbolMapping(const SymbolMapping&) noexcept = default;
  constexpr SymbolMapping(SymbolMapping&&) noexcept = default;
  constexpr ~SymbolMapping() noexcept = default;

  constexpr SymbolMapping& operator=(const SymbolMapping&) noexcept = default;
  constexpr SymbolMapping& operator=(SymbolMapping&&) noexcept = default;

  template <typename Source, typename Sink>
    requires std::invocable<const Function&, const Source&, SourceSymbol&&, Sink&, SinkSymbol&&>
  void operator()(const Source& source, Sink& sink) const
      noexcept(std::is_nothrow_invocable_v<const Function&, const Source&, SourceSymbol, Sink&, SinkSymbol>) {
    function_(source, SourceSymbol(), sink, SinkSymbol());
  }

private:
  Function function_;
};

template <typename SourceSymbol, typename SinkSymbol, typename Function>
constexpr SymbolMapping<SourceSymbol, SinkSymbol, std::remove_cvref_t<Function>> MakeMapping(
    Function&& function) noexcept {
  return SymbolMapping<SourceSymbol, SinkSymbol, std::remove_cvref_t<Function>>(std::forward<Function>(function));
}

template <typename SourceSymbol, typename SinkSymbol>
constexpr SymbolMapping<SourceSymbol, SinkSymbol> MakeMapping() noexcept {
  return SymbolMapping<SourceSymbol, SinkSymbol>();
}

template <typename Source, typename Sink, typename... Mappings>
  requires(std::invocable<const Mappings&, const Source&, Sink&> && ...)
void Map(const Source& source, Sink& sink,
         const Mappings&... mappings) noexcept((std::is_nothrow_invocable_v<const Mappings&, const Source&, Sink&> &&
                                                ...)) {
  (mappings(source, sink), ...);
}

}  // namespace ctti::detail
