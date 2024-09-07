#pragma once

#ifndef LZ_DETAIL_PROCS_HPP
#define LZ_DETAIL_PROCS_HPP

#include "Lz/detail/CompilerChecks.hpp"
#include "Lz/detail/Traits.hpp"

#include <array> // std::get
#include <cstddef>
#include <iterator>
#include <limits>

#ifndef NDEBUG
#include <exception>
#endif // NDEBUG

#if defined(__cpp_lib_stacktrace) && LZ_HAS_INCLUDE(<stacktrace>)
#include <stacktrace>
#endif

namespace lz {
namespace detail {

[[noreturn]] inline void assertionFail(const char* file, const int line, const char* func, const char* message) {
#if defined(__cpp_lib_stacktrace) && LZ_HAS_INCLUDE(<stacktrace>)
    auto st = std::stacktrace::current();
    auto str = std::to_string(st);
    std::fprintf(stderr, "%s:%d assertion failed in function '%s' with message:\n\t%s\nStacktrace:\n%s\n", file, line, func,
                 message, str.c_str());
#else
    std::fprintf(stderr, "%s:%d assertion failed in function '%s' with message:\n\t%s\n", file, line, func, message);
#endif
    std::terminate();
}

#define LZ_ASSERT(CONDITION, MSG) ((CONDITION) ? ((void)0) : (lz::detail::assertionFail(__FILE__, __LINE__, __func__, MSG)))

template<class Iterable>
constexpr auto begin(Iterable&& c) noexcept -> decltype(std::forward<Iterable>(c).begin()) {
    return std::forward<Iterable>(c).begin();
}

template<class Iterable>
constexpr auto end(Iterable&& c) noexcept -> decltype(std::forward<Iterable>(c).end()) {
    return std::forward<Iterable>(c).end();
}

template<class T, size_t N>
constexpr T* begin(T (&array)[N]) noexcept {
    return std::begin(array);
}

template<class T, size_t N>
constexpr T* end(T (&array)[N]) noexcept {
    return std::end(array);
}

template<class>
class FunctionContainer;

template<class Fn, std::size_t... I>
struct TupleExpand {
    FunctionContainer<Fn> _fn{};

    constexpr TupleExpand() = default;

    explicit constexpr TupleExpand(Fn fn) : _fn(std::move(fn)) {
    }

    template<class Tuple>
    LZ_CONSTEXPR_CXX_14 auto operator()(Tuple&& tuple) -> decltype(_fn(std::get<I>(std::forward<Tuple>(tuple))...)) {
        return _fn(std::get<I>(std::forward<Tuple>(tuple))...);
    }

    template<class Tuple>
    LZ_CONSTEXPR_CXX_14 auto operator()(Tuple&& tuple) const -> decltype(_fn(std::get<I>(std::forward<Tuple>(tuple))...)) {
        return _fn(std::get<I>(std::forward<Tuple>(tuple))...);
    }
};

template<class Fn, std::size_t... I>
constexpr TupleExpand<Fn, I...> makeExpandFn(Fn fn, IndexSequence<I...>) {
    return TupleExpand<Fn, I...>(std::move(fn));
}

template<LZ_CONCEPT_INTEGRAL Arithmetic>
LZ_CONSTEXPR_CXX_14 bool isEven(const Arithmetic value) noexcept {
    return (value % 2) == 0;
}

template<class... Ts>
LZ_CONSTEXPR_CXX_14 void decompose(const Ts&...) noexcept {
}

template<class Result, LZ_CONCEPT_INTEGRAL Arithmetic>
LZ_CONSTEXPR_CXX_14 Result roundEven(const Arithmetic a, const Arithmetic b) noexcept {
    LZ_ASSERT(a != 0 && b != 0, "division by zero error");
    if (b == 1) {
        return static_cast<Result>(a);
    }
    if (b == -1) {
        return -static_cast<Result>(a);
    }
    if (isEven(a) && isEven(b)) {
        return static_cast<Result>(a / b);
    }
    return static_cast<Result>(a / b) + 1;
}

template<class Iter>
DiffType<Iter> sizeHint(Iter first, Iter last) {
    if LZ_CONSTEXPR_IF (IsRandomAccess<Iter>::value) {
        return std::distance(std::move(first), std::move(last));
    }
    else {
        return 0;
    }
}

#ifdef LZ_HAS_EXECUTION
template<class T>
struct IsSequencedPolicy : std::is_same<T, std::execution::sequenced_policy> {};

template<class T>
struct IsForwardOrStronger : std::is_convertible<IterCat<T>, std::forward_iterator_tag> {};

template<class T>
constexpr bool IsSequencedPolicyV = IsSequencedPolicy<T>::value;

template<class T>
constexpr bool IsForwardOrStrongerV = IsForwardOrStronger<T>::value;

template<class Execution, class Iterator>
constexpr bool isCompatibleForExecution() {
    static_assert(std::is_execution_policy_v<Execution>, "Execution must be of type std::execution::*...");
    constexpr bool isSequenced = IsSequencedPolicyV<Execution>;
    if constexpr (!isSequenced) {
        static_assert(IsForwardOrStrongerV<Iterator>,
                      "The iterator type must be forward iterator or stronger. Prefer using std::execution::seq");
    }
    return isSequenced;
}

#endif // LZ_HAS_EXECUTION

#if defined(LZ_STANDALONE) && (!defined(LZ_HAS_FORMAT))
template<class T>
struct SafeBufferSize : std::integral_constant<std::size_t, std::numeric_limits<T>::digits10 + 3> {};

template<>
struct SafeBufferSize<bool> : std::integral_constant<std::size_t, sizeof("false") + 1> {};

inline void toStringFromBuff(const char value, char buff[SafeBufferSize<char>::value]) {
    buff[0] = value;
    std::fill(buff + 1, buff + SafeBufferSize<char>::value, '\0');
}

inline void toStringFromBuff(const bool value, char buff[SafeBufferSize<bool>::value]) {
    std::snprintf(buff, SafeBufferSize<bool>::value, "%s", value ? "true" : "false");
}

#if !defined(__cpp_lib_to_chars)
template<class TCast, class T>
void toStringFromBuff(const T value, char buff[SafeBufferSize<T>::value], const char* fmt) {
    std::snprintf(buff, SafeBufferSize<T>::value, fmt, static_cast<TCast>(value));
}
#endif // !defined(__cpp_lib_to_chars)

#ifdef __cpp_lib_to_chars
template<class T>
void toStringFromBuff(const T value, char buff[SafeBufferSize<T>::value]) {
    std::to_chars(std::begin(buff), std::end(buff), value);
}
#elif defined(__cpp_if_constexpr)
template<class T>
void toStringFromBuff(const T value, char buff[SafeBufferSize<T>::value]) {
    if constexpr (std::is_integral<T>::value) {
        if constexpr (std::is_signed<T>::value) {
            toStringFromBuff<long long>(value, buff, "%lld");
        }
        else {
            toStringFromBuff<unsigned long long>(value, buff, "%llu");
        }
        return;
    }
    else if constexpr (std::is_floating_point<T>::value) {
        toStringFromBuff<long double>(value, buff, "%Lf");
    }
}
#else
template<class T>
EnableIf<std::is_integral<T>::value && std::is_signed<T>::value>
toStringFromBuff(const T value, char buff[SafeBufferSize<T>::value]) {
    toStringFromBuff<long long>(value, buff, "%lld");
}

template<class T>
EnableIf<std::is_integral<T>::value && !std::is_signed<T>::value>
toStringFromBuff(const T value, char buff[SafeBufferSize<T>::value]) {
    toStringFromBuff<unsigned long long>(value, buff, "%llu");
}

template<class T>
EnableIf<std::is_floating_point<T>::value> toStringFromBuff(const T value, char buff[SafeBufferSize<T>::value]) {
    toStringFromBuff<long double>(value, buff, "%Lf");
}
#endif
#endif // if defined(LZ_STANDALONE) && (!defined(LZ_HAS_FORMAT))
} // namespace detail
} // namespace lz

#endif // LZ_DETAIL_PROCS_HPP