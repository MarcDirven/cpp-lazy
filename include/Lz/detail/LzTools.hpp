#pragma once

#include <tuple>
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
}}
