#pragma once

#include <numeric>
#include <iterator>

#include <Lz/detail/LzTools.hpp>
#include <Lz/Map.hpp>


#if __has_include(<execution>)
  #define HAS_INCLUDE_EXECUTION
#endif


namespace lz {
    namespace detail {
        template<class Iterator>
        using ValueType = typename std::iterator_traits<Iterator>::value_type;

        template<class Iterator>
        using DifferenceType = typename std::iterator_traits<Iterator>::difference_type;

        template<class Iterable>
        using ValueTypeIterable = typename std::iterator_traits<decltype(std::declval<Iterable>().begin())>::value_type;
    }

    template<class Iterator>
    double mean(const Iterator begin, const Iterator end) {
        const detail::DifferenceType<Iterator> distance = std::distance(begin, end);
        const detail::ValueType<Iterator> sum = std::accumulate(begin, end, detail::ValueType<Iterator>(0));
        return static_cast<double>(sum) / distance;
    }

    template<class Iterable>
    double mean(const Iterable& container) {
        return mean(std::begin(container), std::end(container));
    }

    template<class Iterator, class Compare>
    double median(const Iterator begin, const Iterator end, Compare compare) {
        const detail::DifferenceType<Iterator> len = std::distance(begin, end);
        if (len == 0) {
            throw std::invalid_argument("the length of the sequence cannot be 0");
        }

        const detail::DifferenceType<Iterator> mid = len / 2;
        const Iterator midIter = std::next(begin, mid);
        std::nth_element(begin, midIter, end, compare);

        if (detail::isEven(len)) {
            const Iterator leftHalf = std::max_element(begin, midIter);
            return (*leftHalf + *midIter) / 2.;
        }
        return *midIter;
    }

    template<class Iterable, class Compare>
    double median(Iterable& iterable, Compare compare) {
        return median(std::begin(iterable), std::end(iterable), compare);
    }

    template<class Iterator>
    double median(const Iterator begin, const Iterator end) {
        return median(begin, end, std::less<detail::ValueType<Iterator>>());
    }

    template<class Iterable>
    double median(Iterable& iterable) {
        return median(std::begin(iterable), std::end(iterable), std::less<detail::ValueTypeIterable<Iterable>>());
    }
}