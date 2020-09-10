#pragma once

#include <numeric>
#include <iterator>
#include <algorithm>

#include "StringSplitter.hpp"
#include "Join.hpp"
#include "Map.hpp"
#include "Random.hpp"
#include "Enumerate.hpp"


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

        const detail::DifferenceType<Iterator> mid = len >> 1;
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

    template<class Iterable, class Compare>
    double median(Iterable&& iterable, Compare compare) {
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

    template<class Iterable>
    double median(Iterable&& iterable) {
        return median(std::begin(iterable), std::end(iterable), std::less<detail::ValueTypeIterable<Iterable>>());
    }

#ifdef CXX_LT_17
    template<class SubString = std::string, class String = std::string>
#else
    template<class SubString = std::string_view, class String = std::string_view>
#endif
    StringSplitter<SubString, String> lines(String&& string) {
        return split(string, "\n");
    }

    template<class Strings>
    auto unlines(Strings&& strings) -> Join<std::decay_t<decltype(std::begin(strings))>> {
        static_assert(std::is_same<std::string, typename std::decay_t<Strings>::value_type>::value
#ifndef CXX_LT_17
            || std::is_same<std::string_view, typename std::decay_t<Strings>::value_type>::value
#endif
            , "the type of the container should be std::string or std::string_view");
        return join(strings, "\n");
    }

    template<class Iterable>
    void shuffle(Iterable& iterable) {
        constexpr size_t min = 0;
        const auto max = static_cast<size_t>(std::distance(std::begin(iterable), std::end(iterable)));
        auto indexes = random(min, max, max);

#ifdef CXX_LT_17
        for (std::pair<size_t, size_t> idxRandomIdx : enumerate(indexes)) {
            std::swap(iterable[idxRandomIdx.first], iterable[idxRandomIdx.second]);
        }
#else
        for (auto [idx, randomIdx] : enumerated) {
            std::swap(iterable[idx], iterable[randomIdx]);
        }
#endif
    }
}