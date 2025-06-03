#ifndef CTTI_DETAIL_ATTRIBUTES_IMPL_HPP
#define CTTI_DETAIL_ATTRIBUTES_IMPL_HPP

#include <ctti/detail/hash.hpp>
#include <ctti/detail/meta.hpp>
#include <ctti/detail/preprocessor.hpp>
#include <ctti/detail/symbol_impl.hpp>

#include <concepts>
#include <string_view>
#include <type_traits>

namespace ctti::detail {

template <auto Value>
struct Attribute {
  using value_type = decltype(Value);
  static constexpr value_type kValue = Value;

  constexpr value_type Get() noexcept { return kValue; }
  constexpr operator value_type() const noexcept { return Get(); }

  friend constexpr bool operator==(const Attribute&, const value_type& other) noexcept { return kValue == other; }
  friend constexpr bool operator==(const value_type& other, const Attribute&) noexcept { return kValue == other; }
};

template <typename T>
concept AttributeType = requires {
  typename T::value_type;
  T::value;
} || requires {
  typename T::value_type;
  T::kValue;
} || requires {
  typename T::type;
  T::name;
} || requires {
  T::name;
};

template <typename T>
struct TagAttribute {
  using type = T;
  static constexpr std::string_view kName = "TagAttribute";
};

template <typename NameHolder>
struct NamedAttribute {
  static constexpr std::string_view kName = NameHolder::value;
};

}  // namespace ctti::detail

#define CTTI_DETAIL_NAMED_ATTRIBUTE(name)            \
  struct name##_holder {                             \
    static constexpr std::string_view value = #name; \
  };                                                 \
  using name = ctti::named_attribute<name##_holder>

#define CTTI_DETAIL_DEFINE_ANNOTATED_SYMBOL(name, ...)                                  \
  namespace ctti_symbols {                                                              \
  struct name {                                                                         \
    static constexpr std::string_view symbol = CTTI_PP_STR(name);                       \
    static constexpr std::uint64_t hash = ctti::detail::Fnv1aHash(symbol);             \
    using attributes = ctti::detail::TypeList<__VA_ARGS__>;                             \
                                                                                        \
    constexpr name() noexcept = default;                                                \
                                                                                        \
    template <typename T>                                                               \
    static constexpr bool has_attribute() noexcept {                                    \
      return ctti::detail::Contains<T, attributes>::value;                             \
    }                                                                                   \
                                                                                        \
    template <auto Value>                                                               \
    static constexpr bool has_attribute_value() noexcept {                              \
      return ctti::detail::Contains<ctti::attribute<Value>, attributes>::value;        \
    }                                                                                   \
                                                                                        \
    template <typename T, bool IsEnum = ctti::detail::EnumType<T>, typename = void>     \
    struct Impl {                                                                       \
      static constexpr bool kIsMemberOf = false;                                        \
      using member_type = std::nullptr_t;                                               \
                                                                                        \
      static constexpr member_type GetMember() noexcept {                               \
        return nullptr;                                                                 \
      }                                                                                 \
                                                                                        \
      static constexpr std::string_view GetMemberName() noexcept {                      \
        return CTTI_PP_STR(name);                                                       \
      }                                                                                 \
    };                                                                                  \
                                                                                        \
    template <typename T>                                                               \
    struct Impl<T, false, ctti::detail::VoidType<decltype(&T::name)>> {                 \
      static constexpr bool kIsMemberOf = true;                                         \
      using member_type = decltype(&T::name);                                           \
                                                                                        \
      static constexpr member_type GetMember() noexcept {                               \
        return &T::name;                                                                \
      }                                                                                 \
                                                                                        \
      static constexpr std::string_view GetMemberName() noexcept {                      \
        return CTTI_PP_STR(name);                                                       \
      }                                                                                 \
    };                                                                                  \
                                                                                        \
    template <typename T>                                                               \
    struct Impl<T, true, ctti::detail::VoidType<decltype(T::name)>> {                   \
      static constexpr bool kIsMemberOf = true;                                         \
      using member_type = decltype(T::name);                                            \
                                                                                        \
      static constexpr member_type GetMember() noexcept {                               \
        return T::name;                                                                 \
      }                                                                                 \
                                                                                        \
      static constexpr std::string_view GetMemberName() noexcept {                      \
        const auto full_name = ctti::detail::ValueNameOfImpl<T, T::name>::Apply();      \
        const std::size_t pos = full_name.rfind("::");                                  \
        if (pos != std::string_view::npos) {                                            \
          return full_name.substr(pos + 2);                                             \
        }                                                                               \
        return full_name;                                                               \
      }                                                                                 \
    };                                                                                  \
                                                                                        \
    template <typename T>                                                               \
    using member_type = typename Impl<T>::member_type;                                  \
                                                                                        \
    template <typename T>                                                               \
    static constexpr bool is_owner_of() noexcept {                                      \
      return Impl<T>::kIsMemberOf;                                                      \
    }                                                                                   \
                                                                                        \
    template <typename T>                                                               \
    static constexpr member_type<T> get_member() noexcept {                             \
      return Impl<T>::GetMember();                                                      \
    }                                                                                   \
                                                                                        \
    template <typename T>                                                               \
    static constexpr std::string_view member_name() noexcept {                          \
      return Impl<T>::GetMemberName();                                                  \
    }                                                                                   \
                                                                                        \
    template <typename T>                                                               \
    using value_type = typename ctti::detail::MemberTraits<member_type<T>>::value_type; \
  };                                                                                    \
  }                                                                                     \
  inline ctti_symbols::name SymbolFromHashImpl(ctti::detail::UInt64<ctti::detail::Fnv1aHash(CTTI_PP_STR(name))>);

#endif  // CTTI_DETAIL_ATTRIBUTES_IMPL_HPP
