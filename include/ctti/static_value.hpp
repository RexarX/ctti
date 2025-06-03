#ifndef CTTI_STATIC_VALUE_HPP
#define CTTI_STATIC_VALUE_HPP

#include <ctti/detail/static_value_impl.hpp>

namespace ctti {

template <typename T, T Value>
using static_value = detail::StaticValue<T, Value>;

}  // namespace ctti

#define CTTI_STATIC_VALUE(x) ctti::static_value<std::remove_cvref_t<decltype(x)>, x>{}

#endif  // CTTI_STATIC_VALUE_HPP
