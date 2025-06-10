#ifndef CTTI_DETAIL_MODEL_IMPL_HPP
#define CTTI_DETAIL_MODEL_IMPL_HPP

#include <ctti/detail/meta.hpp>
#include <ctti/detail/symbol_impl.hpp>
#include <ctti/type_tag.hpp>

namespace ctti::detail {

template <typename... Symbols>
struct Model {
  using SymbolList = TypeList<Symbols...>;
  static constexpr std::size_t kSize = sizeof...(Symbols);
};

template <typename T>
consteval auto GetModelHelper(int) -> decltype(ctti_model(ctti::type_tag<T>{}));

template <typename T>
consteval Model<> GetModelHelper(...);

template <typename T, typename = void>
struct GetModel {
  using Type = decltype(GetModelHelper<T>(0));
};

template <typename T>
struct GetModel<T, VoidType<typename T::ctti_model>> {
  using Type = typename T::ctti_model;
};

template <typename T>
using ModelOf = typename GetModel<T>::Type;

template <typename T>
class HasModel {
private:
  template <typename U>
  static constexpr bool CheckIntrusive() {
    if constexpr (requires { typename U::ctti_model; }) {
      using model_type = typename U::ctti_model;
      if constexpr (requires { model_type::size; }) {
        return model_type::size > 0;
      }
    }
    return false;
  }

  template <typename U>
  static constexpr bool CheckAdl() {
    if constexpr (requires { ctti_model(ctti::type_tag<U>{}); }) {
      using model_type = decltype(ctti_model(ctti::type_tag<U>{}));
      if constexpr (requires { model_type::size; }) {
        return model_type::size > 0;
      }
    }
    return false;
  }

public:
  static constexpr bool kValue = CheckIntrusive<T>() || CheckAdl<T>();
};

}  // namespace ctti::detail

#endif  // CTTI_DETAIL_MODEL_IMPL_HPP
