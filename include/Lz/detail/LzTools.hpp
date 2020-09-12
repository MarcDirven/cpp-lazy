#pragma once


#include <utility>


#if __cplusplus < 201703L || (defined(_MSVC_LANG) && _MSVC_LANG < 201703L)
  #define CXX_LT_17
#endif
#if __cplusplus >= 202002L || (defined(_MSVC_LANG) && _MSVC_LANG >= 202002L)
  #define HAS_CXX_20
#endif


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


    template<class Function, class... Args>
    using FunctionReturnType = decltype(std::declval<Function>()(std::declval<Args>()...));

    template<class Arithmetic>
    inline bool isEven(const Arithmetic value) {
        return (value & 1) == 0;
    }
}}
