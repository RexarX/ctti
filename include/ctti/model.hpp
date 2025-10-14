#pragma once

#include <ctti/detail/model_impl.hpp>
#include <ctti/type_tag.hpp>

namespace ctti {

/**
 * @brief Represents a compile-time type model consisting of a list of symbols.
 * @tparam Symbols The symbols that make up the model.
 */
template <typename... Symbols>
class model {
private:
  using internal_model = detail::Model<Symbols...>;

public:
  using symbol_list = typename internal_model::SymbolList;
  static constexpr std::size_t size = internal_model::kSize;
};

/**
 * @brief Retrieves the model associated with a given type.
 * @tparam T The type for which to retrieve the model.
 * @param tag A type tag representing the type T.
 * @return The model associated with the type T, or an empty model if none is defined.
 */
template <typename T>
[[nodiscard]] constexpr ctti::model<> ctti_model(ctti::type_tag<T> /*tag*/) noexcept {
  return {};
}

/**
 * @brief Reflects the model of a given type.
 * @tparam T The type for which to reflect the model.
 * @return The model associated with the type T.
 */
template <typename T>
[[nodiscard]] constexpr auto reflect_model(type_tag<T> tag) {
  return ctti_model(tag);
}

template <typename T>
using model_of = detail::ModelOf<T>;

template <typename T>
constexpr bool has_model_v = detail::HasModel<T>::kValue;

}  // namespace ctti
