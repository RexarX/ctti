#pragma once

#include <ctti/detail/compile_time_string.hpp>
#include <ctti/detail/hash_impl.hpp>
#include <ctti/detail/meta.hpp>

#include <array>
#include <concepts>
#include <cstddef>
#include <string_view>
#include <type_traits>
#include <utility>

namespace ctti::detail {

template <auto Value>
struct AttributeValue {
  using value_type = decltype(Value);
  static constexpr value_type kValue = Value;

  friend constexpr bool operator==(const AttributeValue& /*value*/, const value_type& other) noexcept {
    return kValue == other;
  }

  friend constexpr bool operator==(const value_type& other, const AttributeValue& /*value*/) noexcept {
    return kValue == other;
  }

  [[nodiscard]] constexpr value_type Get() const noexcept { return kValue; }
  constexpr explicit operator value_type() const noexcept { return Get(); }
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
[[nodiscard]] constexpr bool AttributeHasValue() noexcept {
  if constexpr (requires {
                  typename Attr::value_type;
                  { Attr::value } -> std::convertible_to<typename Attr::value_type>;
                }) {
    // Make sure we're using exactly the same conditions for comparing values as defined in attribute_value's operator==
    return Attr::value == Value;
  }
  return false;
}

template <typename Attr, typename Tag>
[[nodiscard]] constexpr bool AttributeHasTag() noexcept {
  if constexpr (requires { typename Attr::tag_type; }) {
    return std::same_as<typename Attr::tag_type, Tag>;
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
  [[nodiscard]] static constexpr bool Has() noexcept {
    return Contains<T, attributes_type>::value;
  }

  template <auto Value>
  [[nodiscard]] static constexpr bool HasValue() noexcept {
    return (... || AttributeHasValue<Attributes, Value>());
  }

  template <typename Tag>
  [[nodiscard]] static constexpr bool HasTag() noexcept {
    return (... || AttributeHasTag<Attributes, Tag>());
  }

  template <CompileTimeString Name>
  [[nodiscard]] static constexpr bool HasNamed() noexcept {
    return (... || (requires { Attributes::name; } && Attributes::name == Name.View()));
  }

  template <typename F>
    requires(std::invocable<const F&, Identity<Attributes>> && ...)
  static constexpr void ForEach(const F& func) noexcept((std::is_nothrow_invocable_v<const F&, Identity<Attributes>> &&
                                                         ...)) {
    (func(Identity<Attributes>{}), ...);
  }

  [[nodiscard]] static constexpr auto GetNames() noexcept -> std::array<std::string_view, kSize> {
    if constexpr (kSize > 0) {
      return GetAttributeNames(std::index_sequence_for<Attributes...>{});
    } else {
      return {};
    }
  }

private:
  template <std::size_t... Is>
  [[nodiscard]] static constexpr auto GetAttributeNames(std::index_sequence<Is...> /*sequence*/) noexcept
      -> std::array<std::string_view, kSize> {
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
