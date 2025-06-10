#ifndef CTTI_DETAIL_ATTRIBUTES_IMPL_HPP
#define CTTI_DETAIL_ATTRIBUTES_IMPL_HPP

#include <ctti/detail/compile_time_string.hpp>
#include <ctti/detail/hash_impl.hpp>
#include <ctti/detail/meta.hpp>

#include <array>
#include <concepts>
#include <string_view>
#include <type_traits>

namespace ctti::detail {

template <auto Value>
struct AttributeValue {
  using value_type = decltype(Value);
  static constexpr value_type kValue = Value;

  constexpr value_type Get() const noexcept { return kValue; }
  constexpr operator value_type() const noexcept { return Get(); }

  friend constexpr bool operator==(const AttributeValue&, const value_type& other) noexcept { return kValue == other; }
  friend constexpr bool operator==(const value_type& other, const AttributeValue&) noexcept { return kValue == other; }
};

template <typename Tag>
struct TagAttribute {
  using tag_type = Tag;
  static constexpr std::string_view kName = "tag_attribute";
  static constexpr HashType kHash = Fnv1aHash(kName);
};

template <CompileTimeString Name>
struct NamedAttribute {
  static constexpr std::string_view kName = Name.View();
  static constexpr HashType kHash = Fnv1aHash(kName);
};

template <typename T>
concept AttributeType = (requires {
                          typename T::value_type;
                          { T::value } -> std::convertible_to<typename T::value_type>;
                        }) || (requires {
                          typename T::tag_type;
                          { T::name } -> std::convertible_to<std::string_view>;
                        }) || (requires {
                          { T::name } -> std::convertible_to<std::string_view>;
                        });

template <typename Attr, auto Value>
constexpr bool AttributeHasValue() noexcept {
  if constexpr (requires {
                  typename Attr::value_type;
                  { Attr::value } -> std::convertible_to<typename Attr::value_type>;
                }) {
    // Make sure we're using exactly the same conditions for comparing values
    // as defined in attribute_value's operator==
    return Attr::value == Value;
  }
  return false;
}

template <typename Attr, typename Tag>
constexpr bool AttributeHasTag() noexcept {
  if constexpr (requires { typename Attr::tag_type; }) {
    return std::is_same_v<typename Attr::tag_type, Tag>;
  }
  return false;
}

template <typename... Attributes>
  requires(AttributeType<Attributes> && ...)
struct AttributeList {
  using attributes_type = TypeList<Attributes...>;
  static constexpr std::size_t kSize = sizeof...(Attributes);

  template <std::size_t I>
    requires(I < kSize)
  using At = typename attributes_type::template At<I>;

  template <typename T>
  static constexpr bool Has() noexcept {
    return Contains<T, attributes_type>::value;
  }

  template <auto Value>
  static constexpr bool HasValue() noexcept {
    return (... || AttributeHasValue<Attributes, Value>());
  }

  template <typename Tag>
  static constexpr bool HasTag() noexcept {
    return (... || AttributeHasTag<Attributes, Tag>());
  }

  template <CompileTimeString Name>
  static constexpr bool HasNamed() noexcept {
    return (... || (requires { Attributes::name; } && Attributes::name == Name.View()));
  }

  template <typename F>
  static constexpr void ForEach(F&& f) {
    (f(Identity<Attributes>{}), ...);
  }

  static constexpr std::array<std::string_view, kSize> GetNames() noexcept {
    if constexpr (kSize > 0) {
      return GetAttributeNames(std::index_sequence_for<Attributes...>{});
    } else {
      return {};
    }
  }

private:
  template <std::size_t... Is>
  static constexpr std::array<std::string_view, kSize> GetAttributeNames(std::index_sequence<Is...>) noexcept {
    auto get_name = []<typename Attr>() -> std::string_view {
      if constexpr (requires { Attr::kName; }) {
        return Attr::kName;
      } else if constexpr (requires { Attr::name; }) {
        return Attr::name;
      } else {
        return "unnamed_attribute";
      }
    };

    return { get_name.template operator()<Attributes>()... };
  }
};

struct ReadOnlyTag {};
struct WriteOnlyTag {};
struct DeprecatedTag {};
struct InternalTag {};
struct ValidatedTag {};

struct DescriptionHolder {
  static constexpr std::string_view value = "description";
};

template <CompileTimeString Desc>
struct DescriptionValueHolder {
  static constexpr std::string_view value = Desc.View();
};

}  // namespace ctti::detail

#endif  // CTTI_DETAIL_ATTRIBUTES_IMPL_HPP
