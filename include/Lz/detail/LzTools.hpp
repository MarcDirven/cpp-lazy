#pragma once

#include <tuple>
#include <vector>
#include <type_traits>
#include <utility>
#include <algorithm>
#include <map>


namespace lz { namespace detail {
    template<class Container, class Iterator>
    void fillContainer(Iterator first, Container& container) {
        for (size_t i = 0; i < container.size(); i++) {
            container[i] = *first;
            ++first;
        }
    }

    template<class ValueType, size_t N, class Iterator>
    std::array<ValueType, N> fillArray(Iterator first) {
        std::array<ValueType, N> array{};
        fillContainer(first, array);
        return array;
    }

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

    template<typename T>
    struct IsContiguousContainer {
    private:
#if !(__cplusplus > 202002L || (defined(_MSVC_LANG) && _MSVC_LANG > 202002L))
        typedef std::true_type Yes;
        typedef std::false_type No;

        template<typename U>
        static auto test(int) -> decltype(std::declval<U>().data(), Yes()) {
            return {};
        };

        template<typename>
        static No test(...) {
            return {};
        }
#endif

    public:
#if __cplusplus > 202002L || (defined(_MSVC_LANG) && _MSVC_LANG > 202002L)
        static constexpr bool value =
            std::is_same<
                std::iterator_traits<std::begin(std::declval<T>())>,
                std::contiguous_iterator_tag>::value
#else
        static constexpr bool value = std::is_same<decltype(test<T>(0)), Yes>::value;
#endif
    };
}}
