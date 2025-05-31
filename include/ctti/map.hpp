#ifndef CTTI_MAP_HPP
#define CTTI_MAP_HPP

#include <ctti/symbol.hpp>

namespace ctti {

struct default_symbol_mapping_function {
  constexpr default_symbol_mapping_function() noexcept = default;

  template <typename Source, typename SourceSymbol, typename Sink, typename SinkSymbol>
  void operator()(const Source& source, SourceSymbol, Sink& sink, SinkSymbol) const {
    if constexpr (SourceSymbol::template is_member_of<Source>() && SinkSymbol::template is_member_of<Sink>()) {
      ctti::set_member_value<SinkSymbol>(sink, ctti::get_member_value<SourceSymbol>(source));
    }
  }
};

template <typename SourceSymbol, typename SinkSymbol, typename Source, typename Sink, typename Function>
void map(const Source& source, Sink& sink, const Function& function) {
  function(source, SourceSymbol(), sink, SinkSymbol());
}

template <typename SourceSymbol, typename SinkSymbol, typename Source, typename Sink>
void map(const Source& source, Sink& sink) {
  ctti::map<SourceSymbol, SinkSymbol>(source, sink, ctti::default_symbol_mapping_function());
}

template <typename SourceSymbol, typename SinkSymbol, typename Function = default_symbol_mapping_function>
struct symbol_mapping {
  constexpr symbol_mapping(Function function = {}) : function{function} {}

  Function function;

  template <typename Source, typename Sink>
  void operator()(const Source& source, Sink& sink) const {
    function(source, SourceSymbol(), sink, SinkSymbol());
  }
};

template <typename SourceSymbol, typename SinkSymbol, typename Function>
constexpr ctti::symbol_mapping<SourceSymbol, SinkSymbol, std::remove_cvref_t<Function>> mapping(Function&& function) {
  return {std::forward<Function>(function)};
}

template <typename SourceSymbol, typename SinkSymbol>
constexpr ctti::symbol_mapping<SourceSymbol, SinkSymbol> mapping() {
  return {ctti::default_symbol_mapping_function()};
}

template <typename Source, typename Sink, typename... Mappings>
void map(const Source& source, Sink& sink, const Mappings&... mappings) {
  (mappings(source, sink), ...);
}

}  // namespace ctti

#endif  // CTTI_MAP_HPP
