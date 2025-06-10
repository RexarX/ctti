#ifndef CTTI_DETAIL_MAP_IMPL_HPP
#define CTTI_DETAIL_MAP_IMPL_HPP

#include <ctti/detail/symbol_impl.hpp>

#include <concepts>
#include <type_traits>
#include <utility>

namespace ctti::detail {

struct DefaultSymbolMappingFunction {
  constexpr DefaultSymbolMappingFunction() noexcept = default;

  template <typename Source, typename SourceSymbol, typename Sink, typename SinkSymbol>
  void operator()(const Source& source, SourceSymbol, Sink& sink, SinkSymbol) const noexcept {
    if constexpr (SourceSymbol::template is_owner_of<Source>() && SinkSymbol::template is_owner_of<Sink>()) {
      sink.*(SinkSymbol::template get_member<Sink>()) = source.*(SourceSymbol::template get_member<Source>());
    }
  }
};

template <typename SourceSymbol, typename SinkSymbol, typename Source, typename Sink, typename Function>
  requires std::invocable<Function, const Source&, SourceSymbol&&, Sink&, SinkSymbol&&>
void Map(const Source& source, Sink& sink, const Function& function) noexcept(
    std::is_nothrow_invocable_v<Function, const Source&, SourceSymbol&&, Sink&, SinkSymbol&&>) {
  function(source, SourceSymbol(), sink, SinkSymbol());
}

template <typename SourceSymbol, typename SinkSymbol, typename Source, typename Sink>
void Map(const Source& source, Sink& sink) noexcept {
  Map<SourceSymbol, SinkSymbol>(source, sink, DefaultSymbolMappingFunction());
}

template <typename SourceSymbol, typename SinkSymbol, typename Function = DefaultSymbolMappingFunction>
class SymbolMapping {
public:
  constexpr SymbolMapping(Function function = {}) : function_(function) {}

  template <typename Source, typename Sink>
    requires std::invocable<Function, const Source&, SourceSymbol&&, Sink&, SinkSymbol&&>
  void operator()(const Source& source, Sink& sink) const noexcept {
    function_(source, SourceSymbol(), sink, SinkSymbol());
  }

private:
  Function function_;
};

template <typename SourceSymbol, typename SinkSymbol, typename Function>
constexpr SymbolMapping<SourceSymbol, SinkSymbol, std::remove_cvref_t<Function>> MakeMapping(
    Function&& function) noexcept {
  return {std::forward<Function>(function)};
}

template <typename SourceSymbol, typename SinkSymbol>
constexpr SymbolMapping<SourceSymbol, SinkSymbol> MakeMapping() noexcept {
  return {DefaultSymbolMappingFunction()};
}

template <typename Source, typename Sink, typename... Mappings>
void Map(const Source& source, Sink& sink, const Mappings&... mappings) noexcept {
  (mappings(source, sink), ...);
}

}  // namespace ctti::detail

#endif  // CTTI_DETAIL_MAP_IMPL_HPP
