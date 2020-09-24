#pragma once

#ifndef LZ_LZ_TOOLS_HPP
#define LZ_LZ_TOOLS_HPP

#include <utility>

#if __cplusplus < 201703L || (defined(_MSVC_LANG) && _MSVC_LANG < 201703L)
  #define LZ_CXX_LT_17
#endif
#if __cplusplus >= 202002L || (defined(_MSVC_LANG) && _MSVC_LANG >= 202002L)
  #define LZ_HAS_CXX_20
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

    template<class I1, class I2>
    concept LessThanComparable = requires(I1 it1, I2 it2) {
        { *it1 < *it2 } -> std::convertible_to<bool>;
    };

    template<class I>
    concept Arithmetic = std::is_floating_point_v<I> || std::is_arithmetic_v<I>;

  #define LZ_CONCEPT_ARITHMETIC Arithmetic
  #define LZ_REQUIRES_LESS_THAN(A, B) requires LessThanComparable<A, B>
  #define LZ_CONCEPT_ITERABLE BasicIterable
  #define LZ_CONCEPT_ITERATOR BasicIterator
#else
  #define LZ_CONCEPT_ARITHMETIC class
  #define LZ_REQUIRES_LESS_THAN(A, B)
  #define LZ_CONCEPT_ITERATOR class
  #define LZ_CONCEPT_ITERABLE class
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

    template<class Function, class... Args>
    using FunctionReturnType = decltype(std::declval<Function>()(std::declval<Args>()...));

    template<class Arithmetic>
    inline bool isEven(const Arithmetic value) {
        return (value & 1) == 0;
    }
}}

#endif