#ifndef CTTI_DETAIL_MODEL_IMPL_HPP
#define CTTI_DETAIL_MODEL_IMPL_HPP

#include <ctti/detail/meta.hpp>
#include <ctti/detail/symbol_impl.hpp>
#include <ctti/type_tag.hpp>

namespace ctti::detail {

template <typename... Symbols>
struct Model {
  using symbol_list = TypeList<Symbols...>;
  static constexpr std::size_t kSize = sizeof...(Symbols);

  using symbols = symbol_list;
  static constexpr std::size_t size = kSize;
};

template <typename T>
constexpr auto get_model_helper(int) -> decltype(ctti_model(type_tag<T>{}));

template <typename T>
constexpr Model<> get_model_helper(...);

template <typename T, typename = void>
struct GetModel {
  using type = decltype(get_model_helper<T>(0));
};

template <typename T>
struct GetModel<T, VoidType<typename T::ctti_model>> {
  using type = typename T::ctti_model;
};

template <typename T>
using ModelOf = typename GetModel<T>::type;

template <typename T>
struct HasModel : public BoolType<(ListSize<ModelOf<T>>::value > 0)> {};

}  // namespace ctti::detail

// Default implementation in global namespace for fallback
template <typename T>
constexpr ctti::detail::Model<> ctti_model(ctti::type_tag<T>) {
  return {};
}

#endif  // CTTI_DETAIL_MODEL_IMPL_HPP
