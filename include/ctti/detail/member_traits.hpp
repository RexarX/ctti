#pragma once

#include <concepts>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <tuple>
#include <type_traits>
#include <utility>

namespace ctti::detail {

enum class OverloadMatchQuality : std::int8_t {
  kNotCallable = -1,  // Cannot be called with given arguments
  kFallback = 0,      // Can be called but with some issues
  kConvertible = 1,   // Arguments can be converted to parameter types
  kPromotable = 2,    // Arguments can be promoted (e.g., int to double)
  kExactMatch = 3     // Perfect argument type match
};

template <typename Member>
struct MemberTraits;

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
  [[nodiscard]] static constexpr const value_type& Get(const Obj& object, pointer_type member) noexcept {
    return object.*member;
  }

  template <typename Obj>
    requires std::derived_from<std::remove_reference_t<Obj>, Class>
  [[nodiscard]] static constexpr value_type& Get(Obj& object, pointer_type member) noexcept {
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

template <typename Return, typename Class, typename... Args>
struct MemberTraits<Return (Class::*)(Args...)> {
  using value_type = std::remove_cvref_t<Return>;
  using pointer_type = Return (Class::*)(Args...);
  using class_type = Class;
  using return_type = Return;
  using args_tuple = std::tuple<Args...>;
  using signature_type = Return(Args...);

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
  [[nodiscard]] static constexpr Return Call(Obj&& object, pointer_type member, CallArgs&&... args) noexcept(
      std::is_nothrow_invocable_v<pointer_type, Obj&&, CallArgs&&...>) {
    return std::invoke(member, std::forward<Obj>(object), std::forward<CallArgs>(args)...);
  }

  template <typename Signature>
  [[nodiscard]] static constexpr bool CanCallWithSignature() noexcept {
    return std::same_as<Signature, signature_type>;
  }
};

template <typename Return, typename Class, typename... Args>
struct MemberTraits<Return (Class::*)(Args...) const> {
  using value_type = std::remove_cvref_t<Return>;
  using pointer_type = Return (Class::*)(Args...) const;
  using class_type = Class;
  using return_type = Return;
  using args_tuple = std::tuple<Args...>;
  using signature_type = Return(Args...) const;

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
  [[nodiscard]] static constexpr Return Call(Obj&& object, pointer_type member, CallArgs&&... args) noexcept(
      std::is_nothrow_invocable_v<pointer_type, Obj&&, CallArgs&&...>) {
    return std::invoke(member, std::forward<Obj>(object), std::forward<CallArgs>(args)...);
  }

  template <typename Signature>
  [[nodiscard]] static constexpr bool CanCallWithSignature() noexcept {
    return std::same_as<Signature, signature_type>;
  }
};

template <auto MemberPtr>
struct OverloadSignature {
  using traits_type = MemberTraits<std::remove_cvref_t<decltype(MemberPtr)>>;
  using class_type = typename traits_type::class_type;

  static constexpr auto kMemberPtr = MemberPtr;

  template <typename Signature>
  [[nodiscard]] static constexpr bool CanCallWithSignature() noexcept {
    if constexpr (traits_type::kIsFunctionMember) {
      return traits_type::template CanCallWithSignature<Signature>();
    } else {
      return false;
    }
  }

  template <typename Obj, typename... Args>
    requires(std::invocable<decltype(MemberPtr), Obj &&, Args && ...>)
  [[nodiscard]] static constexpr decltype(auto) Call(Obj&& obj, Args&&... args) noexcept(
      (std::is_nothrow_invocable_v<decltype(MemberPtr), Obj&&, Args&&...>)) {
    if constexpr (traits_type::kIsFunctionMember) {
      return std::invoke(kMemberPtr, std::forward<Obj>(obj), std::forward<Args>(args)...);
    } else {
      static_assert(sizeof...(Args) == 0, "Data members don't accept arguments");
      return std::forward<Obj>(obj).*kMemberPtr;
    }
  }
};

// Helper to check if argument types can be promoted to parameter types
template <typename From, typename To>
[[nodiscard]] constexpr bool IsPromotable() noexcept {
  if constexpr (std::same_as<From, To>) {
    return false;  // Exact match, not a promotion
  } else if constexpr (std::integral<From> && std::integral<To>) {
    return sizeof(From) <= sizeof(To) && std::is_signed_v<From> == std::is_signed_v<To>;
  } else if constexpr (std::floating_point<From> && std::floating_point<To>) {
    return sizeof(From) <= sizeof(To);
  } else if constexpr (std::integral<From> && std::floating_point<To>) {
    return true;  // int to float is a promotion
  }
  return false;
}

// Helper to rank overload matches
template <auto MemberPtr, typename Obj, typename... Args>
[[nodiscard]] constexpr OverloadMatchQuality GetOverloadMatchQuality() noexcept {
  using member_traits = MemberTraits<std::remove_cvref_t<decltype(MemberPtr)>>;

  if constexpr (!std::invocable<decltype(MemberPtr), Obj, Args...>) {
    return OverloadMatchQuality::kNotCallable;
  } else if constexpr (member_traits::kIsFunctionMember && sizeof...(Args) == member_traits::kArity) {
    // Check argument compatibility for function members
    using expected_args = typename member_traits::args_tuple;
    using provided_args = std::tuple<std::remove_cvref_t<Args>...>;

    if constexpr (std::same_as<provided_args, expected_args>) {
      return OverloadMatchQuality::kExactMatch;
    } else {
      // Check if all arguments can be promoted
      constexpr bool all_promotable = []<std::size_t... Is>(std::index_sequence<Is...>) {
        return (IsPromotable<std::tuple_element_t<Is, provided_args>, std::tuple_element_t<Is, expected_args>>() &&
                ...);
      }(std::index_sequence_for<Args...>{});

      if (all_promotable) {
        return OverloadMatchQuality::kPromotable;
      }

      // Check if all arguments are convertible
      constexpr bool all_convertible = (std::convertible_to<Args, typename member_traits::template ArgType<0>> && ...);
      if (all_convertible) {
        return OverloadMatchQuality::kConvertible;
      }

      return OverloadMatchQuality::kFallback;
    }
  } else if constexpr (!member_traits::kIsFunctionMember && sizeof...(Args) == 0) {
    return OverloadMatchQuality::kExactMatch;  // Data member access
  } else {
    return OverloadMatchQuality::kFallback;
  }
}

template <auto... MemberPtrs>
class MemberOverloadSet {
public:
  static constexpr std::size_t kCount = sizeof...(MemberPtrs);

  template <typename Signature>
  [[nodiscard]] static constexpr bool HasOverloadWithSignature() noexcept {
    if constexpr (kCount > 0) {
      return (CanCallOverloadWithSignature<MemberPtrs, Signature>() || ...);
    }
    return false;
  }

  template <typename Obj, typename... Args>
  [[nodiscard]] static constexpr bool HasOverload() noexcept {
    if constexpr (kCount > 0) {
      return (CanCallOverload<MemberPtrs, Obj, Args...>() || ...);
    }
    return false;
  }

  template <typename Obj, typename... Args>
    requires(HasOverload<Obj, Args...>())
  [[nodiscard]] static constexpr decltype(auto) Call(Obj&& obj, Args&&... args) noexcept(
      noexcept(CallBestMatching<Obj, Args...>(std::forward<Obj>(obj), std::forward<Args>(args)...))) {
    return CallBestMatching<Obj, Args...>(std::forward<Obj>(obj), std::forward<Args>(args)...);
  }

private:
  template <auto MemberPtr, typename Signature>
  [[nodiscard]] static constexpr bool CanCallOverloadWithSignature() noexcept {
    return OverloadSignature<MemberPtr>::template CanCallWithSignature<Signature>();
  }

  template <auto MemberPtr, typename Obj, typename... Args>
  [[nodiscard]] static constexpr bool CanCallOverload() noexcept {
    return GetOverloadMatchQuality<MemberPtr, Obj, Args...>() != OverloadMatchQuality::kNotCallable;
  }

  template <typename Obj, typename... Args>
  [[nodiscard]] static constexpr decltype(auto) CallBestMatching(Obj&& obj, Args&&... args) {
    return FindBestMatch<0, OverloadMatchQuality::kNotCallable, 0, Obj, Args...>(std::forward<Obj>(obj),
                                                                                 std::forward<Args>(args)...);
  }

  template <std::size_t Index, OverloadMatchQuality BestQuality, std::size_t BestIndex, typename Obj, typename... Args>
  [[nodiscard]] static constexpr decltype(auto) FindBestMatch(Obj&& obj, Args&&... args);
};

template <auto... MemberPtrs>
template <std::size_t Index, OverloadMatchQuality BestQuality, std::size_t BestIndex, typename Obj, typename... Args>
constexpr decltype(auto) MemberOverloadSet<MemberPtrs...>::FindBestMatch(Obj&& obj, Args&&... args) {
  if constexpr (Index < kCount) {
    constexpr auto current_ptr = std::get<Index>(std::make_tuple(MemberPtrs...));
    constexpr auto current_quality = GetOverloadMatchQuality<current_ptr, Obj, Args...>();

    if constexpr (current_quality > BestQuality) {
      return FindBestMatch<Index + 1, current_quality, Index, Obj, Args...>(std::forward<Obj>(obj),
                                                                            std::forward<Args>(args)...);
    } else {
      return FindBestMatch<Index + 1, BestQuality, BestIndex, Obj, Args...>(std::forward<Obj>(obj),
                                                                            std::forward<Args>(args)...);
    }
  } else {
    // We've found the best match, now call it
    static_assert(BestQuality != OverloadMatchQuality::kNotCallable, "No viable overload found");
    constexpr auto best_ptr = std::get<BestIndex>(std::make_tuple(MemberPtrs...));
    return std::invoke(best_ptr, std::forward<Obj>(obj), std::forward<Args>(args)...);
  }
}

}  // namespace ctti::detail
