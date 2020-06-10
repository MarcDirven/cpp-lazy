#pragma once


#include <tuple>
#include <type_traits>
#include <utility>


namespace detail {
    template<class Container, class Iterator>
    void fillContainer(Iterator first, Container& container) {
        for (size_t i = 0; i < container.size(); i++) {
            container[i] = *first;
            ++first;
        }
    }

    template<size_t N, class Iterator>
    void fillArray(Iterator first) {
        std::array<typename std::iterator_traits<Iterator>::value_type, N> array;
        return fillContainer(first, array);
    }
}
