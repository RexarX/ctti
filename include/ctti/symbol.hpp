#ifndef CTTI_SYMBOL_HPP
#define CTTI_SYMBOL_HPP

#include <ctti/detail/symbol_impl.hpp>

namespace ctti {

template <const char* SymbolName>
struct symbol {
  static constexpr std::string_view name = detail::Symbol<SymbolName>::kName;
  static constexpr std::uint64_t hash = detail::Symbol<SymbolName>::kHash;

  template <typename T>
  using member_type = detail::Symbol<SymbolName>::member_type;

  template <typename T>
  using value_type = detail::Symbol<SymbolName>::value_type;

  template <typename T>
  static constexpr bool is_member_of() noexcept {
    return detail::Symbol<SymbolName>::template IsOwnerOf<T>();
  }

  template <typename T>
  static constexpr member_type<T> get_member() noexcept {
    return detail::Symbol<SymbolName>::template GetMember<T>();
  }

  template <typename T>
  static constexpr std::string_view member_name() noexcept {
    return detail::Symbol<SymbolName>::template GetMemberName<T>();
  }
};

template <typename Member>
struct member_traits;

template <typename T, typename Class>
struct member_traits<T Class::*> {
  using value_type = detail::MemberTraits<T Class::*>::value_type;
  using pointer_type = detail::MemberTraits<T Class::*>::pointer_type;

  template <typename Obj>
    requires std::derived_from<std::remove_reference_t<Obj>, Class>
  static constexpr const value_type& get(const Obj& object, pointer_type member) noexcept {
    return detail::MemberTraits<T Class::*>::Get(object, member);
  }

  template <typename Obj>
    requires std::derived_from<std::remove_reference_t<Obj>, Class>
  static constexpr value_type& get(Obj& object, pointer_type member) noexcept {
    return detail::MemberTraits<T Class::*>::Get(object, member);
  }

  template <typename Obj, typename Value>
    requires std::derived_from<std::remove_reference_t<Obj>, Class> && std::convertible_to<Value&&, value_type>
  static constexpr void set(Obj& object, pointer_type member,
                            Value&& value) noexcept(std::is_nothrow_assignable_v<value_type&, Value&&>) {
    detail::MemberTraits<T Class::*>::Set(object, member, std::forward<Value>(value));
  }
};

template <typename T, typename Class, typename... Args>
struct member_traits<T (Class::*)(Args...)> {
  using value_type = detail::MemberTraits<T (Class::*)(Args...)>::value_type;
  using pointer_type = detail::MemberTraits<T (Class::*)(Args...)>::pointer_type;

  template <typename Obj, typename... CallArgs>
    requires std::is_invocable_r_v<T, pointer_type, Obj&&, CallArgs&&...>
  static constexpr T get(Obj&& object, pointer_type member,
                         CallArgs&&... args) noexcept(std::is_nothrow_invocable_v<pointer_type, Obj&&, CallArgs&&...>) {
    return detail::MemberTraits<T (Class::*)(Args...)>::Get(std::forward<Obj>(object), member,
                                                            std::forward<CallArgs>(args)...);
  }
};

template <typename T, typename Class, typename... Args>
struct member_traits<T (Class::*)(Args...) const> {
  using value_type = detail::MemberTraits<T (Class::*)(Args...) const>::value_type;
  using pointer_type = detail::MemberTraits<T (Class::*)(Args...) const>::pointer_type;

  template <typename Obj, typename... CallArgs>
    requires std::is_invocable_r_v<T, pointer_type, Obj&&, CallArgs&&...>
  static constexpr T get(Obj&& object, pointer_type member,
                         CallArgs&&... args) noexcept(std::is_nothrow_invocable_v<pointer_type, Obj&&, CallArgs&&...>) {
    return detail::MemberTraits<T (Class::*)(Args...) const>::Get(std::forward<Obj>(object), member,
                                                                  std::forward<CallArgs>(args)...);
  }
};

template <typename Class, typename Arg>
struct member_traits<void (Class::*)(Arg)> {
  using value_type = detail::MemberTraits<void (Class::*)(Arg)>::value_type;
  using pointer_type = detail::MemberTraits<void (Class::*)(Arg)>::pointer_type;

  template <typename Obj, typename CallArg>
    requires std::invocable<pointer_type, Obj, CallArg>
  static constexpr void set(Obj&& object, pointer_type member,
                            CallArg&& arg) noexcept(std::is_nothrow_invocable_v<pointer_type, Obj&&, CallArg&&>) {
    detail::MemberTraits<void (Class::*)(Arg)>::Set(std::forward<Obj>(object), member, std::forward<CallArg>(arg));
  }
};

template <std::uint64_t Hash>
using symbol_from_hash = detail::SymbolFromHash<Hash>;

template <std::size_t Hash>
using no_symbol = detail::NoSymbol<Hash>;

template <typename SymbolType, typename T>
  requires(SymbolType::template is_owner_of<T>())
constexpr decltype(auto) get_member_value(const T& object) noexcept {
  return detail::MemberTraits<typename SymbolType::template member_type<T>>::Get(object,
                                                                                 SymbolType::template get_member<T>());
}

template <typename SymbolType, typename T>
  requires(SymbolType::template is_owner_of<T>())
constexpr decltype(auto) get_member_value(T& object) noexcept {
  return detail::MemberTraits<typename SymbolType::template member_type<T>>::Get(object,
                                                                                 SymbolType::template get_member<T>());
}

template <typename SymbolType, typename T, typename... Args>
  requires(SymbolType::template is_owner_of<T>()) &&
          detail::CanSetMember<T, typename SymbolType::template member_type<T>, Args...>
constexpr void set_member_value(T& object, Args&&... args) noexcept {
  detail::MemberTraits<typename SymbolType::template member_type<T>>::Set(object, SymbolType::template get_member<T>(),
                                                                          std::forward<Args>(args)...);
}

template <std::uint64_t Hash>
constexpr auto get_symbol_from_hash() noexcept {
  return SymbolFromHashImpl(detail::UInt64<Hash>{});
}

}  // namespace ctti

#define CTTI_DEFINE_SYMBOL(name) CTTI_DETAIL_DEFINE_SYMBOL(name)

#endif  // CTTI_SYMBOL_HPP
