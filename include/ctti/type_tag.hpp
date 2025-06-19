#pragma once

namespace ctti {

template <typename T>
struct type_tag {
  using type = T;
  constexpr type_tag() noexcept = default;
};

}  // namespace ctti

