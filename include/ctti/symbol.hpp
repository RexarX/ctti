#ifndef CTTI_SYMBOL_HPP
#define CTTI_SYMBOL_HPP

#include <ctti/detail/hash.hpp>
#include <ctti/detail/meta.hpp>
#include <ctti/detail/preprocessor.hpp>
#include <ctti/detailed_nameof.hpp>
#include <ctti/name.hpp>

#include <concepts>

#define CTTI_DEFINE_SYMBOL(name)                                                         \
  struct name {                                                                          \
    constexpr name() noexcept = default;                                                 \
                                                                                         \
    static constexpr std::string_view symbol() noexcept {                                \
      return CTTI_PP_STR(name);                                                          \
    }                                                                                    \
                                                                                         \
    static constexpr std::uint64_t hash() noexcept {                                     \
      return ctti::detail::fnv1a_hash(symbol());                                         \
    }                                                                                    \
                                                                                         \
    template <typename T, bool IsEnum = std::is_enum_v<T>, typename = void>              \
    struct impl {                                                                        \
      static constexpr bool is_member_of() noexcept {                                    \
        return false;                                                                    \
      }                                                                                  \
                                                                                         \
      using member_type = std::nullptr_t;                                                \
                                                                                         \
      static constexpr member_type get_member() noexcept {                               \
        return nullptr;                                                                  \
      }                                                                                  \
                                                                                         \
      static constexpr std::string_view member_name() noexcept {                         \
        return CTTI_PP_STR(name);                                                        \
      }                                                                                  \
    };                                                                                   \
                                                                                         \
    template <typename T>                                                                \
    struct impl<T, false, ctti::meta::void_t<decltype(&T::name)>> {                      \
      static constexpr bool is_member_of() noexcept {                                    \
        return true;                                                                     \
      }                                                                                  \
                                                                                         \
      using member_type = decltype(&T::name);                                            \
                                                                                         \
      static constexpr member_type get_member() noexcept {                               \
        return &T::name;                                                                 \
      }                                                                                  \
                                                                                         \
      static constexpr std::string_view member_name() noexcept {                         \
        return CTTI_PP_STR(name);                                                        \
      }                                                                                  \
    };                                                                                   \
                                                                                         \
    template <typename T>                                                                \
    struct impl<T, true, ctti::meta::void_t<decltype(T::name)>> {                        \
      static constexpr bool is_member_of() noexcept {                                    \
        return true;                                                                     \
      }                                                                                  \
                                                                                         \
      using member_type = decltype(T::name);                                             \
                                                                                         \
      static constexpr member_type get_member() noexcept {                               \
        return T::name;                                                                  \
      }                                                                                  \
                                                                                         \
      static constexpr std::string_view member_name() noexcept {                         \
        const auto full_name = ctti::nameof<CTTI_STATIC_VALUE(&T::name)>();              \
        /* Find the last :: occurrence and return everything after it */                 \
        const size_t pos = full_name.rfind("::");                                        \
        if (pos != std::string_view::npos) {                                             \
          return full_name.substr(pos + 2); /* +2 to skip the :: */                      \
        }                                                                                \
        return full_name;                                                                \
      }                                                                                  \
    };                                                                                   \
                                                                                         \
    template <typename T>                                                                \
    using member_type = typename impl<T>::member_type;                                   \
                                                                                         \
    template <typename T>                                                                \
    static constexpr bool is_member_of() noexcept {                                      \
      return impl<T>::is_member_of();                                                    \
    }                                                                                    \
                                                                                         \
    template <typename T>                                                                \
    static constexpr member_type<T> get_member() noexcept {                              \
      return impl<T>::get_member();                                                      \
    }                                                                                    \
                                                                                         \
    template <typename T>                                                                \
    static constexpr ctti::name_t detailed_name() noexcept {                             \
      return ctti::detailed_nameof<CTTI_STATIC_VALUE(get_member<T>())>();                \
    }                                                                                    \
                                                                                         \
    template <typename T>                                                                \
    static constexpr std::string_view member_name() noexcept {                           \
      return impl<T>::member_name();                                                     \
    }                                                                                    \
                                                                                         \
    template <typename T>                                                                \
    using value_type = typename ctti::detail::member_traits<member_type<T>>::value_type; \
  };                                                                                     \
                                                                                         \
  inline name ctti_symbol_from_hash(ctti::meta::uint64<ctti::detail::fnv1a_hash(CTTI_PP_STR(name))>);

namespace ctti::detail {

template <typename Member>
struct member_traits;

template <typename T, typename Class>
struct member_traits<T Class::*> {
  using value_type = T;
  using pointer_type = T Class::*;

  template <typename Obj>
    requires std::is_base_of_v<Class, std::remove_reference_t<Obj>>
  static constexpr const value_type& get(const Obj& object, pointer_type member) {
    return object.*member;
  }

  template <typename Obj>
    requires std::is_base_of_v<Class, std::remove_reference_t<Obj>>
  static constexpr value_type& get(Obj& object, pointer_type member) {
    return object.*member;
  }

  template <typename Obj, typename Value>
    requires std::is_base_of_v<Class, std::remove_reference_t<Obj>> && std::convertible_to<Value&&, value_type>
  static constexpr void set(Obj& object, pointer_type member, Value&& value) {
    get(object, member) = std::forward<Value>(value);
  }
};

template <typename T, typename Class, typename... Args>
struct member_traits<T (Class::*)(Args...)> {
  using value_type = T;
  using pointer_type = T (Class::*)(Args...);

  template <typename Obj, typename... CallArgs>
    requires std::is_invocable_v<pointer_type, Obj, CallArgs...>
  static constexpr value_type get(Obj&& object, pointer_type member, CallArgs&&... args) {
    return (std::forward<Obj>(object).*member)(std::forward<CallArgs>(args)...);
  }
};

template <typename Class, typename Arg>
struct member_traits<void (Class::*)(Arg)> {
  using pointer_type = void (Class::*)(Arg);
  using value_type = std::remove_cvref_t<Arg>;

  template <typename Obj, typename CallArg>
    requires std::is_invocable_v<pointer_type, Obj, CallArg>
  static constexpr void set(Obj&& object, pointer_type member, CallArg&& arg) {
    (std::forward<Obj>(object).*member)(std::forward<CallArg>(arg));
  }
};

template <typename T, typename Class, typename... Args>
struct member_traits<T (Class::*)(Args...) const> {
  using value_type = T;
  using pointer_type = T (Class::*)(Args...) const;

  template <typename Obj, typename... CallArgs>
    requires std::is_invocable_v<pointer_type, Obj, CallArgs...>
  static constexpr value_type get(Obj&& object, pointer_type member, CallArgs&&... args) {
    return (std::forward<Obj>(object).*member)(std::forward<CallArgs>(args)...);
  }
};

template <typename T, typename Member, typename... Args>
concept has_member_get = requires(const T& object, Member member, Args&&... args) {
  { member_traits<Member>::get(object, member, std::forward<Args>(args)...) };
};

template <typename T, typename Member, typename... Args>
  requires has_member_get<T, Member, Args...>
constexpr decltype(auto) member_traits_get(const T& object, Member member, Args&&... args) {
  return member_traits<Member>::get(object, member, std::forward<Args>(args)...);
}

template <typename T, typename Member, typename... Args>
  requires has_member_get<T&, Member, Args...>
constexpr decltype(auto) member_traits_get(T& object, Member member, Args&&... args) {
  return member_traits<Member>::get(object, member, std::forward<Args>(args)...);
}

template <typename T, typename Member, typename... Args>
concept has_member_set = requires(T& object, Member member, Args&&... args) {
  { member_traits<Member>::set(object, member, std::forward<Args>(args)...) };
};

template <typename T, typename Member, typename... Args>
  requires has_member_set<T, Member, Args...>
constexpr void member_traits_set(T& object, Member member, Args&&... args) {
  member_traits<Member>::set(object, member, std::forward<Args>(args)...);
}

template <std::size_t Hash>
struct no_symbol_for_hash {};

}  // namespace ctti::detail

namespace ctti {

template <typename Symbol, typename T>
concept has_member = Symbol::template is_member_of<T>();

template <typename Symbol, typename T, typename... Args>
  requires has_member<Symbol, T>
constexpr decltype(auto) get_member_value(const T& object, Args&&... args) {
  return detail::member_traits_get(object, Symbol::template get_member<T>(), std::forward<Args>(args)...);
}

template <typename Symbol, typename T, typename... Args>
  requires has_member<Symbol, T>
constexpr decltype(auto) get_member_value(T& object, Args&&... args) {
  return detail::member_traits_get(object, Symbol::template get_member<T>(), std::forward<Args>(args)...);
}

template <typename Symbol, typename T, typename... Args>
  requires has_member<Symbol, T> && detail::has_member_set<T, typename Symbol::template member_type<T>, Args...>
constexpr void set_member_value(T& object, Args&&... args) {
  detail::member_traits_set(object, Symbol::template get_member<T>(), std::forward<Args>(args)...);
}

}  // namespace ctti

template <typename Hash>
inline ctti::detail::no_symbol_for_hash<Hash::value> ctti_symbol_from_hash(Hash);

#endif  // CTTI_SYMBOL_HPP
