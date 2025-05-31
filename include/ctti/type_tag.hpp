#ifndef CTTI_TYPE_TAG_HPP
#define CTTI_TYPE_TAG_HPP

namespace ctti {

template <typename T>
struct type_tag {
  constexpr type_tag() noexcept = default;
  using type = T;
};

}  // namespace ctti

#endif  // CTTI_TYPE_TAG_HPP
