#pragma once

namespace ctti {

/**
 * @brief A tag type that holds a type.
 * @tparam T The type to hold.
 */
template <typename T>
struct type_tag {
  using type = T;
  constexpr type_tag() noexcept = default;
};

}  // namespace ctti
