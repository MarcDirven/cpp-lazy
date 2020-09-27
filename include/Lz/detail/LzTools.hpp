#pragma once

#ifndef LZ_LZ_TOOLS_HPP
#define LZ_LZ_TOOLS_HPP

#include <utility>

#if __cplusplus > 201703L || (defined(_MSVC_LANG) && _MSVC_LANG > 201703L)
  #define LZ_HAS_CXX_20
#endif

#if __has_include(<execution>) && ((__cplusplus >= 201703L) || defined(_MSVC_LANG) && _MSVC_LANG >= 201703L)
  #define LZ_HAS_EXECUTION
#endif


namespace lz {
#if (__has_include(<concepts>)) && (defined LZ_HAS_CXX_20)
    template<class I>
    concept BasicIterator = std::input_or_output_iterator<I>;

    template<class I>
    concept BasicIterable = requires(I i) {
        { std::begin(i) } -> BasicIterator;
        { std::end(i) } -> BasicIterator;
    };

    template<class A, class B>
    concept LessThanComparable = requires(A a, B b) {
        { a < b } -> std::convertible_to<bool>;
    };

    template<class I>
    concept Arithmetic = std::is_arithmetic_v<I>;

  #define LZ_CONCEPT_ARITHMETIC Arithmetic
  #define LZ_CONCEPT_INVOCABLE std::invocable
  #define LZ_CONCEPT_INTEGRAL std::integral
  #define LZ_CONCEPT_ITERABLE BasicIterable
  #define LZ_CONCEPT_ITERATOR BasicIterator

  #define LZ_REQUIRES_CALLABLE(RET_VAL, FUNC, ...) requires Callable<RET_VAL, FUNC, ##__VA_ARGS__>
  #define LZ_REQUIRES_LESS_THAN(A, B) requires LessThanComparable<A, B>
#else
  #define LZ_CONCEPT_ARITHMETIC class
  #define LZ_CONCEPT_INVOCABLE class
  #define LZ_CONCEPT_INTEGRAL class
  #define LZ_CONCEPT_ITERATOR class
  #define LZ_CONCEPT_ITERABLE class

  #define LZ_REQUIRES_LESS_THAN(A, B)
#endif
}


#define __LZ_STRINGIFY__(x) #x
#define __LZ_TO_STRING__(x) __LZ_STRINGIFY__(x)
#define __LZ_FILE_LINE__ __FILE__ ": " __LZ_TO_STRING__(__LINE__)


namespace lz { namespace detail {
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