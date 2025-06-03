#ifndef CTTI_DETAIL_TYPE_TAG_IMPL_HPP
#define CTTI_DETAIL_TYPE_TAG_IMPL_HPP

namespace ctti::detail {

template <typename T>
struct TypeTag {
  constexpr TypeTag() noexcept = default;
  using type = T;
};

}  // namespace ctti::detail

#endif  // CTTI_DETAIL_TYPE_TAG_IMPL_HPP
