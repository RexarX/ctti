#ifndef CTTI_DETAIL_MEMBER_TRAITS_HPP
#define CTTI_DETAIL_MEMBER_TRAITS_HPP

#include <ctti/detail/meta.hpp>

#include <concepts>
#include <functional>
#include <type_traits>
#include <utility>

namespace ctti::detail {

template <typename Member>
struct MemberTraits;

// Primary template for data members
template <typename T, typename Class>
struct MemberTraits<T Class::*> {
  using value_type = T;
  using pointer_type = T Class::*;
  using class_type = Class;

  static constexpr bool kIsDataMember = true;
  static constexpr bool kIsFunctionMember = false;
  static constexpr bool kIsConstMember = std::is_const_v<T>;

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
    requires std::derived_from<std::remove_reference_t<Obj>, Class> && std::convertible_to<Value&&, value_type> &&
             (!std::is_const_v<T>)
  static constexpr void Set(Obj& object, pointer_type member,
                            Value&& value) noexcept(std::is_nothrow_assignable_v<value_type&, Value&&>) {
    Get(object, member) = std::forward<Value>(value);
  }
};

// Function member specializations with exact matching
template <typename Return, typename Class, typename... Args>
struct MemberTraits<Return (Class::*)(Args...)> {
  using value_type = std::remove_cvref_t<Return>;
  using pointer_type = Return (Class::*)(Args...);
  using class_type = Class;
  using return_type = Return;
  using args_tuple = std::tuple<Args...>;

  static constexpr bool kIsDataMember = false;
  static constexpr bool kIsFunctionMember = true;
  static constexpr bool kIsConstMember = false;
  static constexpr bool kIsNoexcept = false;
  static constexpr std::size_t kArity = sizeof...(Args);

  template <std::size_t I>
    requires(I < kArity)
  using ArgType = std::tuple_element_t<I, args_tuple>;

  template <typename Obj, typename... CallArgs>
    requires std::is_invocable_r_v<Return, pointer_type, Obj&&, CallArgs&&...>
  static constexpr Return Call(Obj&& object, pointer_type member, CallArgs&&... args) noexcept(
      std::is_nothrow_invocable_v<pointer_type, Obj&&, CallArgs&&...>) {
    return std::invoke(member, std::forward<Obj>(object), std::forward<CallArgs>(args)...);
  }

  template <typename... CallArgs>
  static constexpr bool CanCall() noexcept {
    return std::is_invocable_v<pointer_type, Class&, CallArgs...>;
  }
};

template <typename Return, typename Class, typename... Args>
struct MemberTraits<Return (Class::*)(Args...) const> {
  using value_type = std::remove_cvref_t<Return>;
  using pointer_type = Return (Class::*)(Args...) const;
  using class_type = Class;
  using return_type = Return;
  using args_tuple = std::tuple<Args...>;

  static constexpr bool kIsDataMember = false;
  static constexpr bool kIsFunctionMember = true;
  static constexpr bool kIsConstMember = true;
  static constexpr bool kIsNoexcept = false;
  static constexpr std::size_t kArity = sizeof...(Args);

  template <std::size_t I>
    requires(I < kArity)
  using ArgType = std::tuple_element_t<I, args_tuple>;

  template <typename Obj, typename... CallArgs>
    requires std::is_invocable_r_v<Return, pointer_type, Obj&&, CallArgs&&...>
  static constexpr Return Call(Obj&& object, pointer_type member, CallArgs&&... args) noexcept(
      std::is_nothrow_invocable_v<pointer_type, Obj&&, CallArgs&&...>) {
    return std::invoke(member, std::forward<Obj>(object), std::forward<CallArgs>(args)...);
  }

  template <typename... CallArgs>
  static constexpr bool CanCall() noexcept {
    return std::is_invocable_v<pointer_type, const Class&, CallArgs...>;
  }
};

template <auto MemberPtr>
struct OverloadSignature {
  using traits_type = MemberTraits<std::remove_cvref_t<decltype(MemberPtr)>>;
  using class_type = typename traits_type::class_type;

  static constexpr auto kMemberPtr = MemberPtr;

  template <typename Obj, typename... Args>
  static constexpr bool CanCall() noexcept {
    if constexpr (traits_type::kIsFunctionMember) {
      return std::is_invocable_v<decltype(MemberPtr), Obj&&, Args&&...>;
    } else {
      return sizeof...(Args) == 0 && std::derived_from<std::remove_cvref_t<Obj>, class_type>;
    }
  }

  template <typename Obj, typename... Args>
    requires(CanCall<Obj, Args...>())
  static constexpr decltype(auto) Call(Obj&& obj, Args&&... args) {
    if constexpr (traits_type::kIsFunctionMember) {
      return std::invoke(kMemberPtr, std::forward<Obj>(obj), std::forward<Args>(args)...);
    } else {
      static_assert(sizeof...(Args) == 0, "Data members don't accept arguments");
      return std::forward<Obj>(obj).*kMemberPtr;
    }
  }
};

template <auto... MemberPtrs>
class MemberOverloadSet {
public:
  static constexpr std::size_t kCount = sizeof...(MemberPtrs);

  template <typename Obj, typename... Args>
  static constexpr bool HasOverload() noexcept {
    if constexpr (kCount > 0) {
      return (CanCallOverload<MemberPtrs, Obj, Args...>() || ...);
    }
    return false;
  }

  template <typename Obj, typename... Args>
    requires(HasOverload<Obj, Args...>())
  static constexpr decltype(auto) Call(Obj&& obj, Args&&... args) {
    return CallFirstMatching<Obj, Args...>(std::forward<Obj>(obj), std::forward<Args>(args)...);
  }

private:
  template <auto MemberPtr, typename Obj, typename... Args>
  static constexpr bool CanCallOverload() noexcept {
    using member_traits = MemberTraits<std::remove_cvref_t<decltype(MemberPtr)>>;

    if constexpr (member_traits::kIsFunctionMember) {
      // Strict type checking - no implicit conversions for incompatible types
      if constexpr (sizeof...(Args) == 0) {
        return std::is_invocable_v<decltype(MemberPtr), Obj>;
      } else {
        return std::is_invocable_v<decltype(MemberPtr), Obj, Args...> &&
               (std::convertible_to<Args, typename member_traits::template ArgType<std::size_t{}>> && ...);
      }
    } else {
      return sizeof...(Args) == 0 && std::derived_from<std::remove_cvref_t<Obj>, typename member_traits::class_type>;
    }
  }

  template <typename Obj, typename... Args>
  static constexpr decltype(auto) CallFirstMatching(Obj&& obj, Args&&... args) {
    return TryCall<0, Obj, Args...>(std::forward<Obj>(obj), std::forward<Args>(args)...);
  }

  template <std::size_t Index, typename Obj, typename... Args>
  static constexpr decltype(auto) TryCall(Obj&& obj, Args&&... args) {
    if constexpr (Index < kCount) {
      constexpr auto current_ptr = std::get<Index>(std::make_tuple(MemberPtrs...));

      if constexpr (CanCallOverload<current_ptr, Obj, Args...>()) {
        return std::invoke(current_ptr, std::forward<Obj>(obj), std::forward<Args>(args)...);
      } else {
        return TryCall<Index + 1, Obj, Args...>(std::forward<Obj>(obj), std::forward<Args>(args)...);
      }
    } else {
      static_assert(Index < kCount, "No matching overload found");
    }
  }
};

}  // namespace ctti::detail

#endif  // CTTI_DETAIL_MEMBER_TRAITS_HPP
