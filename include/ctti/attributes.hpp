#pragma once

#include <ctti/detail/attributes_impl.hpp>
#include <ctti/detail/compile_time_string.hpp>
#include <ctti/detail/meta.hpp>

#include <concepts>
#include <cstddef>
#include <string_view>

namespace ctti {

/**
 * @brief Represents an attribute with a specific value.
 * @tparam Value The value of the attribute, which can be of any type.
 */
template <auto Value>
struct attribute_value {
  using value_type = decltype(Value);
  static constexpr value_type value = Value;

  friend constexpr bool operator==(const attribute_value& /*value*/, const value_type& other) noexcept {
    return value == other;
  }

  friend constexpr bool operator==(const value_type& other, const attribute_value& /*value*/) noexcept {
    return value == other;
  }

  /**
   * @brief Retrieves the value of the attribute.
   * @return The value of the attribute.
   */
  [[nodiscard]] constexpr value_type get() const noexcept { return value; }
  [[nodiscard]] constexpr explicit operator value_type() const noexcept { return get(); }
};

/**
 * @brief Represents a tag attribute, which is identified by its type.
 * @tparam Tag The type used to identify the tag attribute.
 */
template <typename Tag>
struct tag_attribute {
  using tag_type = Tag;
  static constexpr std::string_view name = "tag_attribute";
};

/**
 * @brief Represents a named attribute, identified by a compile-time string.
 * @tparam StringHolder A type that holds a static constexpr std::string_view named 'value'.
 */
template <typename StringHolder>
  requires(requires {
    { StringHolder::value } -> std::convertible_to<std::string_view>;
  })
struct named_attribute {
  static constexpr std::string_view name = StringHolder::value;
};

/**
 * @brief A list of attributes, which can include tag attributes, named attributes, and attribute values.
 * @tparam Attributes A variadic list of attribute types.
 */
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

  /**
   * @brief Checks if the attribute list contains an attribute of type T.
   * @tparam T The type of the attribute to check for.
   * @return true if the attribute list contains an attribute of type T, false otherwise.
   */
  template <typename T>
  [[nodiscard]] static constexpr bool has() noexcept {
    return internal_list::template Has<T>();
  }

  /**
   * @brief Checks if the attribute list contains an attribute with the specified value.
   * @tparam Value The value of the attribute to check for.
   * @return true if the attribute list contains an attribute with the specified value, false otherwise
   */
  template <auto Value>
  [[nodiscard]] static constexpr bool has_value() noexcept {
    return internal_list::template HasValue<Value>();
  }

  /**
   * @brief Checks if the attribute list contains a tag attribute of the specified type.
   * @tparam Tag The type of the tag attribute to check for.
   * @return true if the attribute list contains a tag attribute of the specified type, false otherwise.
   */
  template <typename Tag>
  [[nodiscard]] static constexpr bool has_tag() noexcept {
    return internal_list::template HasTag<Tag>();
  }

  /**
   * @brief Checks if the attribute list contains a named attribute of the specified type.
   * @tparam NamedAttr The type of the named attribute to check for.
   * @return true if the attribute list contains a named attribute of the specified type, false otherwise.
   */
  template <typename NamedAttr>
  [[nodiscard]] static constexpr bool has_named() noexcept {
    return detail::Contains<NamedAttr, attributes_type>::value;
  }

  /**
   * @brief Applies a given function to each attribute in the attribute list.
   * @tparam F The type of the function to apply. It must be invocable with each attribute type.
   * @param func The function to apply to each attribute.
   */
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
