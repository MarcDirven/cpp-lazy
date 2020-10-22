#pragma once

#ifndef LZ_LZ_TOOLS_HPP
#define LZ_LZ_TOOLS_HPP

#include <tuple>
#include <iterator>

#ifdef __has_include
  #define CPP_LAZY_HAS_INCLUDE(FILE) __has_include(FILE)
#else
  #define CPP_LAZY_HAS_INCLUDE(FILE) 0
#endif

#if (defined(__GNUC__)) && !(defined(__clang__))
  #define LZ_GCC_VERSION __GNUC__
#endif

#if defined(_MSVC_LANG) && (_MSVC_LANG >= 201103L) && (_MSVC_LANG < 201402L)
  #define LZ_HAS_CXX11
#elif (__cplusplus >= 201103L) && (__cplusplus < 201402L) // ^^^ has msvc && cxx 11 vvv has cxx 11
  #define LZ_HAS_CXX11
#endif // end has cxx 11

#if (__cplusplus >= 201402) || ((defined(_MSVC_LANG)) && _MSVC_LANG >= 201402L)
  #define LZ_HAS_CXX14
#endif // end has cxx 14

#if (__cplusplus >= 201703L) || ((defined(_MSVC_LANG)) && (_MSVC_LANG >= 201703L))
  #define LZ_HAS_CXX_17
#endif // Has cxx 17

#if (__cplusplus > 201703L) || ((defined(_MSVC_LANG) && (_MSVC_LANG > 201703L)))
  #define LZ_HAS_CXX_20
#endif // Has cxx 20

#if CPP_LAZY_HAS_INCLUDE(<execution>) && (defined(LZ_HAS_CXX_17) && (defined(__cpp_lib_execution)))
  #define LZ_HAS_EXECUTION
  #include <execution>
#endif // has execution

#if CPP_LAZY_HAS_INCLUDE(<string_view>) && (defined(LZ_HAS_CXX_17) && (defined(__cpp_lib_string_view)))
  #define LZ_HAS_STRING_VIEW
#endif // has string view

#if CPP_LAZY_HAS_INCLUDE(<concepts>) && (defined(LZ_HAS_CXX_20)) && (defined(__cpp_lib_concepts))
  #define LZ_HAS_CONCEPTS
  #include <concepts>
#endif // has concepts

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
    concept RandomAccesIterable = requires(I i) {
        { std::begin(i) } -> std::random_access_iterator;
        { std::end(i) } -> std::random_access_iterator;
    };

    template<class I>
    concept ContiguousIterable = requires(I i) {
        { std::begin(i) } -> std::contiguous_iterator;
        { std::end(i) } -> std::contiguous_iterator;
    };

    template<class I>
    concept RandomAccesOrHigherIterable = ContiguousIterable<I> || RandomAccesIterable<I>;

    template<class I>
    concept RandomAccesOrHigherIterator = std::random_access_iterator<I> || std::contiguous_iterator<I>;

    template<class A, class B>
    concept LessThanComparable = requires(A a, B b) {
        { a < b } -> std::convertible_to<bool>;
    };

    template<class I>
    concept Arithmetic = std::is_arithmetic_v<I>;


} // End namespace lz

#define LZ_CONCEPT_ARITHMETIC             lz::Arithmetic
#define LZ_CONCEPT_INTEGRAL               std::integral
#define LZ_CONCEPT_INVOCABLE              std::invocable
#define LZ_CONCEPT_ITERABLE               lz::BasicIterable
#define LZ_CONCEPT_ITERATOR               std::input_or_output_iterator
#define LZ_CONCEPT_RA_ITERATOR            RandomAccesOrHigherIterator
#define LZ_CONCEPT_RA_ITERABLE            RandomAccesOrHigherIterable
#define LZ_CONCEPT_BIDIRECTIONAL_ITERATOR std::bidirectional_iterator
#define LZ_CONCEPT_BIDIRECTIONAL_ITERABLE lz::BidirectionalIterable

#define LZ_REQUIRES_LESS_THAN(A, B) requires LessThanComparable<A, B>
#else  // ^^^ has concepts !has concepts vvv
#define LZ_CONCEPT_ARITHMETIC             class
#define LZ_CONCEPT_INTEGRAL               class
#define LZ_CONCEPT_ITERATOR               class
#define LZ_CONCEPT_INVOCABLE              class
#define LZ_CONCEPT_ITERABLE               class
#define LZ_CONCEPT_BIDIRECTIONAL_ITERATOR class
#define LZ_CONCEPT_BIDIRECTIONAL_ITERABLE class
#define LZ_CONCEPT_RA_ITERATOR            class
#define LZ_CONCEPT_RA_ITERABLE            class

#define LZ_REQUIRES_LESS_THAN(A, B)
#endif  // lz has concepts

#define LZ_STRINGIFY(x) #x
#define LZ_TO_STRING(x) LZ_STRINGIFY(x)
#define LZ_FILE_LINE __FILE__ ": " LZ_TO_STRING(__LINE__)


namespace lz { namespace internal {
#ifdef LZ_HAS_EXECUTION
    template<class T>
    struct IsSequencedPolicy {
        static constexpr bool value = std::is_same_v<std::decay_t<T>, std::execution::sequenced_policy>;
    };

    template<class T>
    struct IsParallelPolicy {
        static constexpr bool value = std::is_same_v<std::decay_t<T>, std::execution::parallel_policy>;
    };

    template<class T>
    struct IsForwardOrStronger {
        using IterCat = typename std::iterator_traits<T>::iterator_category;
        static constexpr bool value = !std::is_same_v<IterCat, std::input_iterator_tag> &&
            !std::is_same_v<IterCat, std::output_iterator_tag>;
    };

    template<class T>
    constexpr bool IsSequencedPolicyV = IsSequencedPolicy<T>::value;

    template<class T>
    constexpr bool IsForwardOrStrongerV = IsForwardOrStronger<T>::value;

    template<class Execution, class Iterator>
    constexpr void verifyIteratorAndPolicies() {
        static_assert(std::is_execution_policy_v<Execution>, "Execution must be of type std::execution::*...");

        if constexpr (!IsSequencedPolicyV<Execution>) {
            static_assert(internal::IsForwardOrStrongerV<Iterator>,
                "The iterator type must be forward iterator or stronger. Prefer using std::execution::seq");
        }
    }

    template<class Execution, class Iterator>
	constexpr bool checkForwardAndPolicies() {
        constexpr bool isSequencedPolicy = internal::IsSequencedPolicyV<Execution>;
        internal::verifyIteratorAndPolicies<Execution, Iterator>();
        return isSequencedPolicy;
    }

#endif // LZ_HAS_EXECUTION

#ifdef LZ_HAS_CXX11
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
#else // ^^^ has cxx 11 vvv cxx > 11
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
#endif // end cxx > 11

    template<class T>
    class FakePointerProxy {
        T _t;

    public:
        explicit FakePointerProxy(const T& t) :
            _t(t) {
        }

        decltype(std::addressof(_t)) operator->() {
            return std::addressof(_t);
        }
    };

    template<typename Same, typename First, typename... More>
    struct IsAllSame : std::integral_constant<bool, std::is_same<Same, First>::value && IsAllSame<First, More...>::value> {
    };

    template<typename Same, typename First>
    struct IsAllSame<Same, First> : std::is_same<Same, First> {
    };


    template<class T>
    struct IsBidirectionalOrStronger : std::integral_constant<bool,
        !std::is_same<T, std::input_iterator_tag>::value &&
        !std::is_same<T, std::output_iterator_tag>::value &&
        !std::is_same<T, std::forward_iterator_tag>::value> {
    };

    template<class T>
    struct IsRandomAccess : std::integral_constant<bool,
        IsBidirectionalOrStronger<T>::value && !std::is_same<std::bidirectional_iterator_tag, T>::value> {
    };

    template<class Iterable>
    using IterTypeFromIterable = decltype(std::begin(std::declval<Iterable>()));

    template<class Iterator>
    using ValueType = typename std::iterator_traits<Iterator>::value_type;

    template<class Iterator>
    using PointerType = typename std::iterator_traits<Iterator>::pointer;

    template<class Iterator>
    using RefType = typename std::iterator_traits<Iterator>::reference;

    template<class Iterator>
    using DiffType = typename std::iterator_traits<Iterator>::difference_type;

    template<class Function, class... Args>
    using FunctionReturnType = decltype(std::declval<Function>()(std::declval<Args>()...));

    template<LZ_CONCEPT_INTEGRAL Arithmetic>
    inline bool isEven(const Arithmetic value) {
        return (value & 1) == 0;
    }
}} // end lz::internal

#endif