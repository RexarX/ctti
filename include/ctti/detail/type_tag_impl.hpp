#pragma once

namespace ctti::detail {

template <typename T>
struct TypeTag {
  using type = T;
  constexpr TypeTag() noexcept = default;
};

}  // namespace ctti::detail

