#ifndef CTTI_MODEL_HPP
#define CTTI_MODEL_HPP

#include <ctti/detail/model_impl.hpp>
#include <ctti/type_tag.hpp>

namespace ctti {

template <typename... Symbols>
class model {
private:
  using internal_model = detail::Model<Symbols...>;

public:
  using symbol_list = typename internal_model::SymbolList;
  static constexpr std::size_t size = internal_model::kSize;
};

template <typename T>
constexpr auto reflect_model(type_tag<T> tag) {
  return ctti_model(tag);
}

template <typename T>
using model_of = detail::ModelOf<T>;

template <typename T>
constexpr bool has_model_v = detail::HasModel<T>::kValue;

template <typename T>
constexpr ctti::model<> ctti_model(ctti::type_tag<T>) noexcept {
  return {};
}

}  // namespace ctti

#endif  // CTTI_MODEL_HPP
