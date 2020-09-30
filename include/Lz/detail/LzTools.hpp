#pragma once

#ifndef LZ_LZ_TOOLS_HPP
#define LZ_LZ_TOOLS_HPP

#define LZ_CURRENT_VERSION "2.0.0"

#if (__cplusplus >= 201703L) || ((defined(_MSVC_LANG)) && (_MSVC_LANG >= 201703L))
  #define LZ_HAS_CXX17
#endif
#if __cplusplus > 201703L || ((defined(_MSVC_LANG) && (_MSVC_LANG > 201703L)))
  #define LZ_HAS_CXX_20
#endif

#if __has_include(<execution>) && defined(LZ_HAS_CXX_20)
  #define LZ_HAS_EXECUTION
  #include <execution>
#endif

#if __has_include(<string_view>) && defined(LZ_HAS_CXX17)
  #define LZ_HAS_STRING_VIEW
#endif

namespace lz {
#if __has_include(<concepts>) && (defined(LZ_HAS_CXX_20))
  #define LZ_HAS_CONCEPTS
  #include <concepts>

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

    template<class A, class B>
    concept LessThanComparable = requires(A a, B b) {
        { a < b } -> std::convertible_to<bool>;
    };

    template<class I>
    concept Arithmetic = std::is_arithmetic_v<I>;

  #define LZ_CONCEPT_ARITHMETIC             Arithmetic
  #define LZ_CONCEPT_INVOCABLE              std::invocable
  #define LZ_CONCEPT_INTEGRAL               std::integral
  #define LZ_CONCEPT_ITERABLE               BasicIterable
  #define LZ_CONCEPT_ITERATOR               std::input_or_output_iterator
  #define LZ_CONCEPT_BIDIRECTIONAL_ITERATOR std::bidirectional_iterator
  #define LZ_CONCEPT_BIDIRECTIONAL_ITERABLE BidirectionalIterable

  #define LZ_REQUIRES_LESS_THAN(A, B) requires LessThanComparable<A, B>
#else  // ^^^ has concepts !has concepts vvv
  #define LZ_CONCEPT_ARITHMETIC             class
  #define LZ_CONCEPT_INVOCABLE              class
  #define LZ_CONCEPT_INTEGRAL               class
  #define LZ_CONCEPT_ITERATOR               class
  #define LZ_CONCEPT_ITERABLE               class
  #define LZ_CONCEPT_BIDIRECTIONAL_ITERATOR class
  #define LZ_CONCEPT_BIDIRECTIONAL_ITERABLE class

  #define LZ_REQUIRES_LESS_THAN(A, B)
#endif  // __cpp_lib_concepts
}


#define LZ_STRINGIFY(x) #x
#define LZ_TO_STRING(x) LZ_STRINGIFY(x)
#define LZ_FILE_LINE __FILE__ ": " LZ_TO_STRING(__LINE__)


namespace lz { namespace detail {
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

    template<class T>
    constexpr bool IsParallelPolicyV = IsParallelPolicy<T>::value;

    template<class Execution, class Iterator>
    constexpr void verifyIteratorAndPolicies(Execution, Iterator) {
        static_assert(std::is_execution_policy_v<Execution>, "Execution must be of type std::execution::*...");

        if constexpr (!detail::IsSequencedPolicyV<Execution>) {
            static_assert(detail::IsForwardOrStrongerV<Iterator>,
                          "The iterator type must be forward iterator or stronger. Prefer using std::execution::seq");
        }
    }

#endif // LZ_HAS_EXECUTION
    template<class T>
    class FakePointerProxy {
        T t;

    public:
        explicit FakePointerProxy(const T& t) :
            t(t) {
        }

        T* operator->() {
            return &t;
        }
    };

    template<class Iterable>
    using IterType = std::decay_t<decltype(std::begin(std::declval<Iterable>()))>;
	
    template<class Iterator>
    using ValueTypeIterator = typename std::iterator_traits<Iterator>::value_type;

    template<class Iterable>
    using ValueTypeIterable = ValueTypeIterator<IterType<Iterable>>;

    template<class Function, class... Args>
    using FunctionReturnType = decltype(std::declval<Function>()(std::declval<Args>()...));

    template<LZ_CONCEPT_INTEGRAL Arithmetic>
    inline bool isEven(const Arithmetic value) {
        return (value & 1) == 0;
    }
}}

#endif