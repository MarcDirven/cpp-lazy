#pragma once

#ifndef LZ_LZ_TOOLS_HPP
#define LZ_LZ_TOOLS_HPP

#include <iterator>
#include <tuple>

#if defined(__has_include)
#define LZ_HAS_INCLUDE(FILE) __has_include(FILE)
#else
#define LZ_HAS_INCLUDE(FILE) 0
#endif // __has_include

#if defined(__has_cpp_attribute)
#define LZ_HAS_ATTRIBUTE(ATTR) __has_cpp_attribute(ATTR)
#else
#define LZ_HAS_ATTRIBUTE(ATTR) 0
#endif // __has_cpp_attribute

#if (defined(__GNUC__)) && !(defined(__clang__))
#define LZ_GCC_VERSION __GNUC__
#endif // GNU/clang

#if defined(_MSVC_LANG)
#define LZ_MSVC _MSVC_LANG
#endif // _MSVC_LANG

#if (defined(LZ_MSVC) && (LZ_MSVC >= 201103L) && (LZ_MSVC < 201402L)) || ((__cplusplus >= 201103L) && (__cplusplus < 201402L))
#define LZ_HAS_CXX_11
#endif // end has cxx 11

#if (__cplusplus >= 201300) || ((defined(LZ_MSVC)) && (LZ_MSVC >= 201300))
#define LZ_CONSTEXPR_CXX_14 constexpr
#else
#define LZ_CONSTEXPR_CXX_14 inline
#endif // has cxx 14

#if (__cplusplus >= 201703L) || ((defined(LZ_MSVC)) && (LZ_MSVC >= 201703L))
#define LZ_HAS_CXX_17
#define LZ_CONSTEXPR_CXX_17 constexpr
#else
#define LZ_CONSTEXPR_CXX_17 inline
#endif // Has cxx 17

#if (__cplusplus > 201703L) || ((defined(LZ_MSVC) && (LZ_MSVC > 201703L)))
#define LZ_HAS_CXX_20
#if defined(__cpp_constexpr_dynamic_alloc) && defined(__cpp_lib_constexpr_dynamic_alloc) &&                                      \
    defined(__cpp_lib_constexpr_string) && defined(__cpp_lib_constexpr_vector) && defined(__cpp_lib_constexpr_algorithms)
#define LZ_CONSTEXPR_CXX_20 constexpr
#else
#define LZ_CONSTEXPR_CXX_20 inline
#endif // cpp constexpr new/algo
#else
#define LZ_CONSTEXPR_CXX_20 inline
#endif // Has cxx 20

#if LZ_HAS_ATTRIBUTE(nodiscard) && defined(LZ_HAS_CXX_17)
#define LZ_NODISCARD [[nodiscard]]
#else
#define LZ_NODISCARD
#endif // LZ_HAS_ATTRIBUTE(nodiscard)

#ifdef __cpp_ref_qualifiers
#define LZ_HAS_REF_QUALIFIER
#define LZ_CONST_REF_QUALIFIER const&
#else
#define LZ_CONST_REF_QUALIFIER
#endif // __cpp_ref_qualifiers

#if LZ_HAS_INCLUDE(<execution>) && (defined(LZ_HAS_CXX_17) && (defined(__cpp_lib_execution)))
#define LZ_HAS_EXECUTION
#include <execution>
#endif // has execution

#if LZ_HAS_INCLUDE(<string_view>) && (defined(LZ_HAS_CXX_17) && (defined(__cpp_lib_string_view)))
#define LZ_HAS_STRING_VIEW
#endif // has string view

#if LZ_HAS_INCLUDE(<concepts>) && (defined(LZ_HAS_CXX_20)) && (defined(__cpp_lib_concepts))
#define LZ_HAS_CONCEPTS
#include <concepts>
#endif // has concepts

#ifdef __cpp_if_constexpr
#define LZ_CONSTEXPR_IF constexpr
#else
#define LZ_CONSTEXPR_IF
#endif // __cpp_if_constexpr

#if defined(__cpp_lib_format) && (LZ_HAS_INCLUDE(<format>)) && defined(LZ_HAS_CXX_20)
#define LZ_HAS_FORMAT
#endif // format

#ifdef LZ_HAS_CONCEPTS
namespace lz {
template<class I>
concept BasicIterable = requires(I i) {
    { std::begin(i) } -> std::input_or_output_iterator;
    { std::end(i) } -> std::input_or_output_iterator;
};

template<class I>
concept BidirectionalIterable = requires(I i) {
    { std::begin(i) } -> std::bidirectional_iterator;
    { std::end(i) } -> std::bidirectional_iterator;
};

template<class I>
concept Arithmetic = std::is_arithmetic_v<I>;
} // End namespace lz

#define LZ_CONCEPT_ARITHMETIC lz::Arithmetic
#define LZ_CONCEPT_INTEGRAL std::integral
#define LZ_CONCEPT_INVOCABLE std::invocable
#define LZ_CONCEPT_ITERABLE lz::BasicIterable
#define LZ_CONCEPT_ITERATOR std::input_or_output_iterator
#define LZ_CONCEPT_BIDIRECTIONAL_ITERATOR std::bidirectional_iterator
#define LZ_CONCEPT_BIDIRECTIONAL_ITERABLE lz::BidirectionalIterable

#else // ^^^ has concepts !has concepts vvv
#define LZ_CONCEPT_ARITHMETIC class
#define LZ_CONCEPT_INTEGRAL class
#define LZ_CONCEPT_ITERATOR class
#define LZ_CONCEPT_INVOCABLE class
#define LZ_CONCEPT_ITERABLE class
#define LZ_CONCEPT_BIDIRECTIONAL_ITERATOR class
#define LZ_CONCEPT_BIDIRECTIONAL_ITERABLE class
#endif // lz has concepts

#ifndef NDEBUG
#include <exception>
#endif // NDEBUG

namespace lz {
namespace internal {

#ifdef NDEBUG
#define LZ_ASSERT(CONDITION, MSG) ((void)0)
#else

[[noreturn]] inline void assertionFail(const char* file, const int line, const char* func, const char* message) {
    std::fprintf(stderr, "%s:%d assertion failed in function '%s' with message:\n\t%s", file, line, func, message);
    std::terminate();
}

#define LZ_ASSERT(CONDITION, MSG) ((CONDITION) ? ((void)0) : (lz::internal::assertionFail(__FILE__, __LINE__, __func__, MSG)))
#endif // NDEBUG

/* forward declarations of all iterators that contain a custom distance implementation */
template<class... Iterators>
class CartesianProductIterator;

template<class Arithmetic>
class RangeIterator;

template<class Iterator>
class TakeEveryIterator;

template<class Iterator>
class ChunksIterator;

template<class Iterator, int N>
class FlattenIterator;

template<class Iterator>
class ExcludeIterator;

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

#ifdef LZ_HAS_CXX_11
template<bool B, class U = void>
using EnableIf = typename std::enable_if<B, U>::type;

template<std::size_t...>
struct IndexSequence {};

template<std::size_t N, std::size_t... Rest>
struct IndexSequenceHelper : public IndexSequenceHelper<N - 1, N - 1, Rest...> {};

template<std::size_t... Next>
struct IndexSequenceHelper<0, Next...> {
    using Type = IndexSequence<Next...>;
};

template<std::size_t N>
using MakeIndexSequence = typename IndexSequenceHelper<N>::Type;

template<class T>
using Decay = typename std::decay<T>::type;

template<std::size_t I, class T>
using TupleElement = typename std::tuple_element<I, T>::type;

template<bool B, class IfTrue, class IfFalse>
using Conditional = typename std::conditional<B, IfTrue, IfFalse>::type;
#else  // ^^^ has cxx 11 vvv cxx > 11
template<bool B, class T = void>
using EnableIf = std::enable_if_t<B, T>;

template<std::size_t... N>
using IndexSequence = std::index_sequence<N...>;

template<std::size_t N>
using MakeIndexSequence = std::make_index_sequence<N>;

template<class T>
using Decay = std::decay_t<T>;

template<std::size_t I, class T>
using TupleElement = std::tuple_element_t<I, T>;

template<bool B, class IfTrue, class IfFalse>
using Conditional = std::conditional_t<B, IfTrue, IfFalse>;
#endif // LZ_HAS_CXX_11

template<class Iterable>
using IterTypeFromIterable = decltype(begin(std::forward<Iterable>(std::declval<Iterable>())));

template<class Iterator>
using ValueType = typename std::iterator_traits<Iterator>::value_type;

template<class Iterator>
using RefType = typename std::iterator_traits<Iterator>::reference;

template<class Iterator>
using PointerType = typename std::iterator_traits<Iterator>::pointer;

template<class Iterator>
using DiffType = typename std::iterator_traits<Iterator>::difference_type;

template<class Iterator>
using IterCat = typename std::iterator_traits<Iterator>::iterator_category;

template<class Function, class... Args>
using FunctionReturnType = decltype(std::declval<Function>()(std::declval<Args>()...));

template<class Iterable>
using ValueTypeIterable = typename Decay<Iterable>::value_type;

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
constexpr bool checkForwardAndPolicies() {
    static_assert(std::is_execution_policy_v<Execution>, "Execution must be of type std::execution::*...");
    constexpr bool isSequenced = IsSequencedPolicyV<Execution>;
    if constexpr (!isSequenced) {
        static_assert(IsForwardOrStrongerV<Iterator>,
                      "The iterator type must be forward iterator or stronger. Prefer using std::execution::seq");
    }
    return isSequenced;
}

#endif // LZ_HAS_EXECUTION

constexpr char to_string(const char c) {
    return c;
}

LZ_CONSTEXPR_CXX_20 std::string to_string(const bool b) {
    return b ? "true" : "false";
}

template<class T>
class FakePointerProxy {
    T _t;

    using Pointer = decltype(std::addressof(_t));

public:
    constexpr explicit FakePointerProxy(const T& t) : _t(t) {
    }

    LZ_CONSTEXPR_CXX_17 Pointer operator->() const noexcept {
        return std::addressof(_t);
    }

    LZ_CONSTEXPR_CXX_17 Pointer operator->() noexcept {
        return std::addressof(_t);
    }
};

template<class T, class U, class... Vs>
struct IsAllSame : std::integral_constant<bool, std::is_same<T, U>::value && IsAllSame<U, Vs...>::value> {};

template<class T, class U>
struct IsAllSame<T, U> : std::is_same<T, U> {};

template<class Iterator>
struct IsBidirectional : std::is_convertible<IterCat<Iterator>, std::bidirectional_iterator_tag> {};

template<class Iterator>
struct IsForward : std::is_convertible<IterCat<Iterator>, std::forward_iterator_tag> {};

template<LZ_CONCEPT_INTEGRAL Arithmetic>
inline constexpr bool isEven(const Arithmetic value) noexcept {
    return (value & 1) == 0;
}
} // namespace internal

template<LZ_CONCEPT_ITERATOR... Iterators>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 typename internal::CartesianProductIterator<Iterators...>::difference_type
distance(const internal::CartesianProductIterator<Iterators...>&, const internal::CartesianProductIterator<Iterators...>&);

template<LZ_CONCEPT_ITERATOR Arithmetic>
LZ_NODISCARD LZ_CONSTEXPR_CXX_14 typename internal::RangeIterator<Arithmetic>::difference_type
distance(const internal::RangeIterator<Arithmetic>&, const internal::RangeIterator<Arithmetic>&);

template<LZ_CONCEPT_ITERATOR Iterator>
LZ_NODISCARD LZ_CONSTEXPR_CXX_17 typename internal::TakeEveryIterator<Iterator>::difference_type
distance(const internal::TakeEveryIterator<Iterator>&, const internal::TakeEveryIterator<Iterator>&);

template<LZ_CONCEPT_ITERATOR Iterator>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 typename internal::ChunksIterator<Iterator>::difference_type
distance(const internal::ChunksIterator<Iterator>&, const internal::ChunksIterator<Iterator>&);

template<LZ_CONCEPT_ITERATOR Iterator, int N>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 typename internal::FlattenIterator<Iterator, N>::difference_type
distance(const internal::FlattenIterator<Iterator, N>&, const internal::FlattenIterator<Iterator, N>&);

template<LZ_CONCEPT_ITERATOR Iterator>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 typename internal::ExcludeIterator<Iterator>::difference_type
distance(const internal::ExcludeIterator<Iterator>&, const internal::ExcludeIterator<Iterator>&);

template<LZ_CONCEPT_ITERATOR... Iterators>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 internal::CartesianProductIterator<Iterators...>
next(const internal::CartesianProductIterator<Iterators...>&,
     internal::DiffType<internal::CartesianProductIterator<Iterators...>> = 1);

template<LZ_CONCEPT_ARITHMETIC Arithmetic>
LZ_NODISCARD constexpr internal::RangeIterator<Arithmetic>
next(const internal::RangeIterator<Arithmetic>&, internal::DiffType<internal::RangeIterator<Arithmetic>> = 1);

template<LZ_CONCEPT_ITERATOR Iterator>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 internal::TakeEveryIterator<Iterator>
next(const internal::TakeEveryIterator<Iterator>&, internal::DiffType<internal::TakeEveryIterator<Iterator>> = 1);

template<LZ_CONCEPT_ITERATOR Iterator>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 internal::ChunksIterator<Iterator>
next(const internal::ChunksIterator<Iterator>&, internal::DiffType<internal::ChunksIterator<Iterator>> = 1);

template<LZ_CONCEPT_ITERATOR Iterator, int N>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 internal::FlattenIterator<Iterator, N>
next(const internal::FlattenIterator<Iterator, N>&, internal::DiffType<internal::FlattenIterator<Iterator, N>> = 1);

template<LZ_CONCEPT_ITERATOR Iterator>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 internal::ExcludeIterator<Iterator>
next(const internal::ExcludeIterator<Iterator>&, internal::DiffType<internal::ExcludeIterator<Iterator>> = 1);
} // namespace lz

#endif // LZ_LZ_TOOLS_HPP