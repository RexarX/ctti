#pragma once

#include <ctti/detail/attributes_impl.hpp>

#include <cstddef>
#include <string_view>

namespace ctti {

template <auto Value>
struct attribute_value {
  using value_type = decltype(Value);
  static constexpr value_type value = Value;

  friend constexpr bool operator==([[maybe_unused]] const attribute_value& val, const value_type& other) noexcept {
    return value == other;
  }

  friend constexpr bool operator==(const value_type& other, [[maybe_unused]] const attribute_value& val) noexcept {
    return value == other;
  }

  constexpr value_type get() const noexcept { return value; }
  constexpr operator value_type() const noexcept { return get(); }
};

template <typename Tag>
struct tag_attribute {
  using tag_type = Tag;
  static constexpr std::string_view name = "tag_attribute";
};

template <typename StringHolder>
struct named_attribute {
  static constexpr std::string_view name = StringHolder::value;
};

template <typename... Attributes>
  requires(detail::AttributeType<Attributes> && ...)
class attribute_list {
private:
  using internal_list = detail::AttributeList<Attributes...>;

public:
  using attributes_type = detail::TypeList<Attributes...>;

  static constexpr std::size_t size = internal_list::kSize;

  template <std::size_t I>
    requires(I < size)
  using at = typename internal_list::template At<I>;

  template <typename T>
  static constexpr bool has() noexcept {
    return internal_list::template Has<T>();
  }

  template <auto Value>
  static constexpr bool has_value() noexcept {
    return internal_list::template HasValue<Value>();
  }

  template <typename Tag>
  static constexpr bool has_tag() noexcept {
    return internal_list::template HasTag<Tag>();
  }

  template <typename NamedAttr>
  static constexpr bool has_named() noexcept {
    return detail::Contains<NamedAttr, attributes_type>::value;
  }

  template <typename F>
    requires(std::invocable<const F&, detail::Identity<Attributes>> && ...)
  static constexpr void for_each(const F& func) noexcept(noexcept(internal_list::ForEach(std::declval<const F&>()))) {
    internal_list::ForEach(func);
  }
};

using no_attributes = attribute_list<>;

struct read_only_tag {};
struct write_only_tag {};
struct deprecated_tag {};
struct internal_tag {};
struct validated_tag {};

using read_only = tag_attribute<read_only_tag>;
using write_only = tag_attribute<write_only_tag>;
using deprecated = tag_attribute<deprecated_tag>;
using internal = tag_attribute<internal_tag>;
using validated = tag_attribute<validated_tag>;

template <int Version>
using since = attribute_value<Version>;

using description = named_attribute<detail::DescriptionHolder>;

template <detail::CompileTimeString Desc>
using description_with_value = named_attribute<detail::DescriptionValueHolder<Desc>>;

}  // namespace ctti
