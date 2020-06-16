#pragma once

#include <tuple>
#include <vector>
#include <type_traits>
#include <utility>


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

    template<class ValueType, class Allocator, class Iterator>
    std::vector<ValueType, Allocator> makeVector(Iterator begin, Iterator end, const Allocator& allocator) {
        std::vector<ValueType, Allocator> vector(allocator);
        vector.reserve(std::distance(begin, end));
        vector.insert(vector.begin(), begin, end);
        return vector;
    }

    template<class T>
    class FakePointerProxy {
        T t;

    public:
        explicit FakePointerProxy(const T& t):
            t(t)
        {
        }

        T* operator->() {
            return &t;
        }
    };
}}
