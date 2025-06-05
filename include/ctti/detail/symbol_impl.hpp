#ifndef CTTI_DETAIL_SYMBOL_IMPL_HPP
#define CTTI_DETAIL_SYMBOL_IMPL_HPP

#include <ctti/detail/concepts_impl.hpp>
#include <ctti/detail/hash_impl.hpp>
#include <ctti/detail/meta.hpp>
#include <ctti/detail/name_impl.hpp>
#include <ctti/detail/preprocessor.hpp>
#include <ctti/detail/static_value_impl.hpp>

#include <concepts>
#include <functional>
#include <string_view>
#include <type_traits>
#include <utility>

namespace ctti::detail {

template <typename Member>
struct MemberTraits;

// Data member specialization
template <typename T, typename Class>
struct MemberTraits<T Class::*> {
  using value_type = T;
  using pointer_type = T Class::*;

  template <typename Obj>
    requires std::derived_from<std::remove_reference_t<Obj>, Class>
  static constexpr const value_type& Get(const Obj& object, pointer_type member) noexcept {
    return object.*member;
  }

  template <typename Obj>
    requires std::derived_from<std::remove_reference_t<Obj>, Class>
  static constexpr value_type& Get(Obj& object, pointer_type member) noexcept {
    return object.*member;
  }

  template <typename Obj, typename Value>
    requires std::derived_from<std::remove_reference_t<Obj>, Class> && std::convertible_to<Value&&, value_type>
  static constexpr void Set(Obj& object, pointer_type member,
                            Value&& value) noexcept(std::is_nothrow_assignable_v<value_type&, Value&&>) {
    Get(object, member) = std::forward<Value>(value);
  }
};

// Function member specializations
template <typename T, typename Class, typename... Args>
struct MemberTraits<T (Class::*)(Args...)> {
  using value_type = std::remove_cvref_t<T>;
  using pointer_type = T (Class::*)(Args...);

  template <typename Obj, typename... CallArgs>
    requires std::is_invocable_r_v<T, pointer_type, Obj&&, CallArgs&&...>
  static constexpr T Get(Obj&& object, pointer_type member,
                         CallArgs&&... args) noexcept(std::is_nothrow_invocable_v<pointer_type, Obj&&, CallArgs&&...>) {
    return std::invoke(member, std::forward<Obj>(object), std::forward<CallArgs>(args)...);
  }
};

template <typename T, typename Class, typename... Args>
struct MemberTraits<T (Class::*)(Args...) const> {
  using value_type = std::remove_cvref_t<T>;
  using pointer_type = T (Class::*)(Args...) const;

  template <typename Obj, typename... CallArgs>
    requires std::is_invocable_r_v<T, pointer_type, Obj&&, CallArgs&&...>
  static constexpr T Get(Obj&& object, pointer_type member,
                         CallArgs&&... args) noexcept(std::is_nothrow_invocable_v<pointer_type, Obj&&, CallArgs&&...>) {
    return std::invoke(member, std::forward<Obj>(object), std::forward<CallArgs>(args)...);
  }
};

template <typename Class, typename Arg>
struct MemberTraits<void (Class::*)(Arg)> {
  using pointer_type = void (Class::*)(Arg);
  using value_type = std::remove_cvref_t<Arg>;

  template <typename Obj, typename CallArg>
    requires std::invocable<pointer_type, Obj, CallArg>
  static constexpr void Set(Obj&& object, pointer_type member,
                            CallArg&& arg) noexcept(std::is_nothrow_invocable_v<pointer_type, Obj&&, CallArg&&>) {
    std::invoke(member, std::forward<Obj>(object), std::forward<CallArg>(arg));
  }
};

template <typename T, typename Member, typename... Args>
concept CanGetMember = requires(const T& object, Member member, Args&&... args) {
  { MemberTraits<Member>::Get(object, member, std::forward<Args>(args)...) };
};

template <typename T, typename Member, typename... Args>
concept CanSetMember = requires(T& object, Member member, Args&&... args) {
  { MemberTraits<Member>::Set(object, member, std::forward<Args>(args)...) };
};

template <std::size_t Hash>
struct NoSymbol {};

template <std::uint64_t Hash>
inline NoSymbol<Hash> SymbolFromHashImpl(UInt64<Hash>);

template <std::uint64_t Hash>
using SymbolFromHash = decltype(SymbolFromHashImpl(UInt64<Hash>{}));

template <const char* SymbolName>
struct Symbol {
  static constexpr std::string_view kName = SymbolName;
  static constexpr std::uint64_t kHash = Fnv1aHash(kName);

  template <typename T, bool IsEnum = EnumType<T>, typename = void>
  struct Implementation {
    static constexpr bool kIsMember = false;
    using member_type = std::nullptr_t;

    static constexpr member_type GetMember() noexcept { return nullptr; }
    static constexpr std::string_view GetMemberName() noexcept { return kName; }
  };

  template <typename T>
  using member_type = typename Implementation<T>::member_type;

  template <typename T>
  using value_type = typename MemberTraits<member_type<T>>::value_type;

  template <typename T>
  static constexpr bool IsOwnerOf() noexcept {
    return Implementation<T>::kIsMember;
  }

  template <typename T>
  static constexpr member_type<T> GetMember() noexcept {
    return Implementation<T>::GetMember();
  }

  template <typename T>
  static constexpr std::string_view GetMemberName() noexcept {
    return Implementation<T>::GetMemberName();
  }
};

// Interface functions
template <const char* SymbolName, typename T>
  requires(Symbol<SymbolName>::template IsOwnerOf<T>())
constexpr decltype(auto) GetMemberValue(const T& object) noexcept {
  return MemberTraits<typename Symbol<SymbolName>::template member_type<T>>::Get(
      object, Symbol<SymbolName>::template GetMember<T>());
}

template <const char* SymbolName, typename T>
  requires(Symbol<SymbolName>::template IsOwnerOf<T>())
constexpr decltype(auto) GetMemberValue(T& object) noexcept {
  return MemberTraits<typename Symbol<SymbolName>::template member_type<T>>::Get(
      object, Symbol<SymbolName>::template GetMember<T>());
}

template <const char* SymbolName, typename T, typename... Args>
  requires(Symbol<SymbolName>::template IsOwnerOf<T>()) &&
          CanSetMember<T, typename Symbol<SymbolName>::template member_type<T>, Args...>
constexpr void SetMemberValue(T& object, Args&&... args) noexcept {
  MemberTraits<typename Symbol<SymbolName>::template member_type<T>>::Set(
      object, Symbol<SymbolName>::template GetMember<T>(), std::forward<Args>(args)...);
}

}  // namespace ctti::detail

#define CTTI_DETAIL_DEFINE_SYMBOL(name)                                                 \
  namespace ctti_symbols {                                                              \
                                                                                        \
  struct name {                                                                         \
    static constexpr std::string_view symbol = CTTI_PP_STR(name);                       \
    static constexpr std::uint64_t hash = ctti::detail::Fnv1aHash(symbol);              \
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

#endif  // CTTI_DETAIL_SYMBOL_IMPL_HPP
