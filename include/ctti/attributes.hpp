#ifndef CTTI_ATTRIBUTES_HPP
#define CTTI_ATTRIBUTES_HPP

#include <ctti/detail/attributes_impl.hpp>

namespace ctti {

template <auto Value>
struct attribute {
  using value_type = typename detail::Attribute<Value>::value_type;
  static constexpr value_type value = detail::Attribute<Value>::kValue;

  constexpr value_type get() const noexcept { return value; }
  constexpr operator value_type() const noexcept { return get(); }

  friend constexpr bool operator==(const attribute&, const value_type& other) noexcept { return Value == other; }
  friend constexpr bool operator==(const value_type& other, const attribute&) noexcept { return Value == other; }
};

template <typename T>
concept attribute_type = detail::AttributeType<T>;

template <typename T>
struct tag_attribute {
  using type = T;
  static constexpr std::string_view name = "TagAttribute";
};

template <typename NameHolder>
struct named_attribute {
  static constexpr std::string_view name = NameHolder::value;
};

template <typename... Attributes>
  requires(attribute_type<Attributes> && ...)
struct attribute_list {
  using attributes = detail::TypeList<Attributes...>;
  static constexpr std::size_t size = sizeof...(Attributes);

  template <std::size_t I>
    requires(I < size)
  using at = typename attributes::template At<I>;

  template <typename T>
  static constexpr bool has() noexcept {
    return detail::Contains<T, attributes>::value;
  }

  template <auto Value>
  static constexpr bool has_value() noexcept {
    return detail::Contains<attribute<Value>, attributes>::value;
  }

  template <typename T>
  static constexpr bool has_tag() noexcept {
    return detail::Contains<tag_attribute<T>, attributes>::value;
  }

  template <typename NamedAttr>
  static constexpr bool has_named() noexcept {
    return detail::Contains<NamedAttr, attributes>::value;
  }

  template <std::invocable<detail::Identity<Attributes>...> F>
  static constexpr void for_each(F&& f) {
    (f(detail::Identity<Attributes>{}), ...);
  }
};

struct ReadOnlyTag {};
struct WriteOnlyTag {};
struct DeprecatedTag {};
struct InternalTag {};

using read_only = tag_attribute<ReadOnlyTag>;
using write_only = tag_attribute<WriteOnlyTag>;
using deprecated = tag_attribute<DeprecatedTag>;
using internal = tag_attribute<InternalTag>;

template <int Version>
using since = attribute<Version>;

struct description_holder {
  static constexpr std::string_view value = "Description";
};

using description = named_attribute<description_holder>;

}  // namespace ctti

#define CTTI_NAMED_ATTRIBUTE(name) CTTI_DETAIL_NAMED_ATTRIBUTE(name)
#define CTTI_DEFINE_ANNOTATED_SYMBOL(name, ...) CTTI_DETAIL_DEFINE_ANNOTATED_SYMBOL(name, __VA_ARGS__)

#endif  // CTTI_ATTRIBUTES_HPP
