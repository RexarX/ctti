#ifndef CTTI_DETAIL_OVERLOAD_IMPL_HPP
#define CTTI_DETAIL_OVERLOAD_IMPL_HPP

#include <ctti/detail/concepts_impl.hpp>
#include <ctti/detail/hash.hpp>
#include <ctti/detail/meta.hpp>
#include <ctti/detail/name_impl.hpp>
#include <ctti/detail/preprocessor.hpp>
#include <ctti/detail/symbol_impl.hpp>

#include <concepts>
#include <functional>
#include <type_traits>

namespace ctti::detail {

template <typename Symbol, typename T, typename... Args>
  requires requires { Symbol::call(std::declval<T>(), std::declval<Args>()...); }
constexpr decltype(auto) CallOverload(T&& obj, Args&&... args) {
  return Symbol::call(std::forward<T>(obj), std::forward<Args>(args)...);
}

template <typename Symbol, typename T>
struct OverloadSet {
  T& obj_;

  template <typename... Args>
    requires requires { CallOverload<Symbol>(obj_, std::declval<Args>()...); }
  constexpr decltype(auto) operator()(Args&&... args) const {
    return CallOverload<Symbol>(obj_, std::forward<Args>(args)...);
  }
};

template <typename Symbol, typename T>
constexpr OverloadSet<Symbol, T> GetOverloadSet(T& obj) noexcept {
  return {obj};
}

template <typename Symbol, typename T>
struct OverloadQuery {
  template <typename... Args>
  static constexpr bool Has() noexcept {
    return Symbol::template has_overload<T, Args...>();
  }

  template <typename... Args>
  static constexpr auto Get() noexcept {
    if constexpr (Has<Args...>()) {
      return Symbol::template get_overload<T, Args...>();
    } else {
      return nullptr;
    }
  }
};

}  // namespace ctti::detail

#define CTTI_DETAIL_DEFINE_OVERLOADED_SYMBOL(name)                                                 \
  namespace ctti_symbols {                                                                         \
  struct name {                                                                                    \
    static constexpr std::string_view kSymbol = CTTI_PP_STR(name);                                 \
    static constexpr std::uint64_t kHash = ctti::detail::Fnv1aHash(kSymbol);                       \
    static constexpr std::string_view symbol = kSymbol;                                            \
    static constexpr std::uint64_t hash = kHash;                                                   \
                                                                                                   \
    template <typename T, typename... Args>                                                        \
    static constexpr bool has_overload() noexcept {                                                \
      if constexpr (requires(T& obj, Args&&... args) {                                             \
                      { obj.name(std::forward<Args>(args)...) };                                   \
                    }) {                                                                           \
        /* For char* arguments, be more restrictive */                                             \
        if constexpr (sizeof...(Args) == 1) {                                                      \
          using FirstArg = std::decay_t<std::tuple_element_t<0, std::tuple<Args...>>>;             \
          if constexpr (std::same_as<FirstArg, char*> || std::same_as<FirstArg, const char*>) {    \
            /* Only return true if there's no string overload available */                         \
            return !requires(T& obj, const std::string& str) {                                     \
              { obj.name(str) };                                                                   \
            };                                                                                     \
          }                                                                                        \
        }                                                                                          \
        return true;                                                                               \
      }                                                                                            \
      return false;                                                                                \
    }                                                                                              \
                                                                                                   \
    template <typename T, typename... Args>                                                        \
    static constexpr auto get_overload() noexcept {                                                \
      if constexpr (HasOverload<T, Args...>()) {                                                   \
        return [](auto&& obj, auto&&... args) constexpr noexcept -> decltype(auto) {               \
          return std::forward<decltype(obj)>(obj).name(std::forward<decltype(args)>(args)...);     \
        };                                                                                         \
      } else {                                                                                     \
        return std::nullptr_t{};                                                                   \
      }                                                                                            \
    }                                                                                              \
                                                                                                   \
    template <typename T, typename... Args>                                                        \
    static constexpr decltype(auto) call(T&& obj, Args&&... args)                                  \
      requires requires { std::forward<T>(obj).name(std::forward<Args>(args)...); }                \
    {                                                                                              \
      return std::forward<T>(obj).name(std::forward<Args>(args)...);                               \
    }                                                                                              \
                                                                                                   \
    template <typename T>                                                                          \
    static constexpr bool is_owner_of() noexcept {                                                 \
      return requires { &T::name; } || requires { T::name; } || requires(T obj) { obj.name(); } || \
             requires(T obj, int i) { obj.name(i); };                                              \
    }                                                                                              \
                                                                                                   \
    template <typename T, bool IsEnum = ctti::detail::EnumType<T>, typename = void>                \
    struct Impl {                                                                                  \
      static constexpr bool kIsMemberOf = is_owner_of<T>();                                        \
      using member_type = std::nullptr_t;                                                          \
      static constexpr member_type GetMember() noexcept {                                          \
        return nullptr;                                                                            \
      }                                                                                            \
      static constexpr std::string_view GetMemberName() noexcept {                                 \
        return CTTI_PP_STR(name);                                                                  \
      }                                                                                            \
    };                                                                                             \
                                                                                                   \
    template <typename T>                                                                          \
    struct Impl<T, false, ctti::detail::VoidType<decltype(&T::name)>> {                            \
      static constexpr bool kIsMemberOf = true;                                                    \
      using member_type = decltype(&T::name);                                                      \
      static constexpr member_type GetMember() noexcept {                                          \
        return &T::name;                                                                           \
      }                                                                                            \
      static constexpr std::string_view GetMemberName() noexcept {                                 \
        return CTTI_PP_STR(name);                                                                  \
      }                                                                                            \
    };                                                                                             \
                                                                                                   \
    template <typename T>                                                                          \
    struct Impl<T, true, ctti::detail::VoidType<decltype(T::name)>> {                              \
      static constexpr bool kIsMemberOf = true;                                                    \
      using member_type = decltype(T::name);                                                       \
      static constexpr member_type GetMember() noexcept {                                          \
        return T::name;                                                                            \
      }                                                                                            \
      static constexpr std::string_view GetMemberName() noexcept {                                 \
        const auto full_name = ctti::detail::ValueNameOfImpl<T, T::name>::Apply();                 \
        const std::size_t pos = full_name.rfind("::");                                             \
        if (pos != std::string_view::npos) {                                                       \
          return full_name.substr(pos + 2);                                                        \
        }                                                                                          \
        return full_name;                                                                          \
      }                                                                                            \
    };                                                                                             \
                                                                                                   \
    template <typename T>                                                                          \
    using member_type = typename Impl<T>::member_type;                                             \
                                                                                                   \
    template <typename T>                                                                          \
    using value_type = typename ctti::detail::MemberTraits<member_type<T>>::value_type;            \
                                                                                                   \
    template <typename T>                                                                          \
    static constexpr bool is_member_of() noexcept {                                                \
      return Impl<T>::kIsMemberOf;                                                                 \
    }                                                                                              \
                                                                                                   \
    template <typename T>                                                                          \
    static constexpr member_type<T> get_member() noexcept {                                        \
      return Impl<T>::GetMember();                                                                 \
    }                                                                                              \
                                                                                                   \
    template <typename T>                                                                          \
    static constexpr std::string_view member_name() noexcept {                                     \
      return Impl<T>::GetMemberName();                                                             \
    }                                                                                              \
  };                                                                                               \
  }                                                                                                \
  inline ctti_symbols::name SymbolFromHashImpl(ctti::detail::UInt64<ctti::detail::Fnv1aHash(CTTI_PP_STR(name))>);

#endif  // CTTI_DETAIL_OVERLOAD_IMPL_HPP
