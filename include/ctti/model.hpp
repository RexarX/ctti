#ifndef CTTI_MODEL_HPP
#define CTTI_MODEL_HPP

#include <ctti/detail/model_impl.hpp>
#include <ctti/type_tag.hpp>

namespace ctti {

template <typename... Symbols>
using model = detail::Model<Symbols...>;

// Forward to the ADL mechanism
template <typename T>
constexpr auto reflect_model(type_tag<T> tag) {
  return ctti_model(tag);
}

template <typename T>
using model_of = detail::ModelOf<T>;

template <typename T>
constexpr bool has_model_v = detail::HasModel<T>::value;

}  // namespace ctti

#endif  // CTTI_MODEL_HPP
