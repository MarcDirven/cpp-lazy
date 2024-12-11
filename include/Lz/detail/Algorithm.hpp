#pragma once

#ifndef LZ_DETAIL_ALGORITHM_HPP
#define LZ_DETAIL_ALGORITHM_HPP

#include "Lz/detail/Procs.hpp"
#include "Lz/detail/Traits.hpp"

#if defined(__cpp_lib_not_fn) && defined(LZ_HAS_CXX_17)
#include <functional> // std::not_fn
#endif

namespace lz {
namespace detail {
template<class Iter, class S>
DiffType<Iter> distanceImpl(Iter first, S last, std::forward_iterator_tag) {
    DiffType<Iter> dist = 0;
    for (; first != last; ++first, ++dist) {
    }
    return dist;
}

template<class Iter, class S>
DiffType<Iter> distanceImpl(Iter first, S last, std::random_access_iterator_tag) {
    return last - first;
}

template<class Iter, class S>
DiffType<Iter> distance(Iter first, S last) {
    return distanceImpl(std::move(first), std::move(last), IterCat<Iter>{});
}

template<class Iter, class S, class T, class BinaryOp>
T accumulate(Iter begin, S end, T init, BinaryOp binaryOp) {
    for (; begin != end; ++begin) {
        init = binaryOp(std::move(init), *begin);
    }
    return init;
}

template<class Iter, class S, class BinaryOp>
Iter maxElement(Iter begin, S end, BinaryOp binaryOp) {
    Iter max = begin;
    for (; begin != end; ++begin) {
        if (binaryOp(*max, *begin)) {
            max = begin;
        }
    }
    return max;
}

template<class Iter, class S, class BinaryOp>
Iter minElement(Iter begin, S end, BinaryOp binaryOp) {
    using ValueType = ValueType<Iter>;
    return maxElement(std::move(begin), std::move(end),
                      [&binaryOp](const ValueType& a, const ValueType& b) { return !binaryOp(a, b); });
}

template<class Iter, class S>
Iter find(Iter begin, S end, const ValueType<Iter>& value) {
    for (; begin != end; ++begin) {
        if (*begin == value) {
            break;
        }
    }
    return begin;
}

template<class Iter, class S, class Iter2, class S2>
std::pair<Iter, Iter> search(Iter begin, S end, Iter2 begin2, S2 end2) {
    while (begin != end) {
        Iter it = begin;
        Iter2 it2 = begin2;
        while (it2 != end2 && it != end && *it == *it2) {
            ++it;
            ++it2;
        }
        if (it2 == end2) {
            return { begin, it };
        }
        ++begin;
    }
    return { end, end };
}

template<class Iter, class S, class BinaryOp>
Iter findIf(Iter begin, S end, BinaryOp binaryOp) {
    for (; begin != end; ++begin) {
        if (binaryOp(*begin)) {
            break;
        }
    }
    return begin;
}

template<class Iter, class S, class BinaryOp>
Iter findIfNot(Iter begin, S end, BinaryOp binaryOp) {
#if defined(__cpp_lib_not_fn) && defined(LZ_HAS_CXX_17)
    return findIf(std::move(begin), std::move(end), std::not_fn(std::move(binaryOp)));
#else
    using ValueType = ValueType<Iter>;
    return findIf(std::move(begin), std::move(end), [&binaryOp](const ValueType& value) { return !binaryOp(value); });
#endif
}

template<class Iterator, class S, class T, class U>
ValueType<Iterator> findFirstOrDefault(Iterator begin, const S& end, T&& toFind, U&& defaultValue) {
    auto it = find(std::move(begin), end, std::forward<T>(toFind));
    return it == end ? static_cast<ValueType<Iterator>>(std::forward<U>(defaultValue)) : *it;
}

template<class Iterator, class S, class UnaryPredicate, class U>
ValueType<Iterator> findFirstOrDefaultIf(Iterator begin, const S& end, UnaryPredicate predicate, U&& defaultValue) {
    auto it = findIf(std::move(begin), end, std::move(predicate));
    return it == end ? static_cast<ValueType<Iterator>>(std::forward<U>(defaultValue)) : *it;
}

template<class Iter, class S, class BinaryPredicate>
Iter partition(Iter begin, S end, BinaryPredicate binaryPredicate) {
    begin = findIfnot(begin, end, binaryPredicate);
    if (begin == end) {
        return begin;
    }
    for (Iter it = begin; it != end; ++it, ++begin) {
        if (binaryPredicate(*it)) {
            std::swap(begin, it);
        }
    }
    return begin;
}

template<class Iterator, class S, class BinaryOp = MAKE_BIN_OP(std::plus, ValueType<Iterator>)>
double mean(Iterator begin, S end, BinaryOp binOp = {}) {
    DiffType<Iterator> distance{ 0 };
    ValueType<Iterator> sum{ 0 };

    for (; begin != end; ++begin, ++distance) {
        sum = binOp(std::move(sum), *begin);
    }
    if (distance == 0) {
        return distance;
    }
    return static_cast<double>(sum) / static_cast<double>(static_cast<std::size_t>(distance));
}

template<class Iterator, class S, class UnaryPredicate>
void forEach(Iterator begin, S end, UnaryPredicate unaryPredicate) {
    for (; begin != end; ++begin) {
        unaryPredicate(*begin);
    }
}

template<class Iterator, class S, class UnaryPredicate>
void forEachWhile(Iterator begin, const S& end, UnaryPredicate predicate) {
    for (; begin != end; ++begin) {
        if (!predicate(*begin)) {
            break;
        }
    }
}

template<class Iterator, class T, class U>
ValueType<Iterator> findFirstOrDefault(Iterator begin, Iterator end, const T& toFind, const U& defaultValue) {
    return static_cast<ValueType<Iterator>>(find(std::move(begin), end, toFind) == end ? defaultValue : toFind);
}

template<class Iterator, class T, class UnaryPredicate>
ValueType<Iterator> findFirstOrDefaultIf(Iterator begin, Iterator end, UnaryPredicate predicate, const T& defaultValue) {
    using CastType = ValueType<Iterator>;
    const Iterator pos = findIf(std::move(begin), end, std::move(predicate));
    return static_cast<CastType>(pos == end ? defaultValue : *pos);
}

template<class SourceIterator, class SourceS, class OutputIterator>
void copy(SourceIterator begin, SourceS end, OutputIterator output) {
    for (; begin != end; ++begin, ++output) {
        *output = *begin;
    }
}

template<class SourceIterator, class SourceSentinel, class OutputIterator, class BinaryOp>
void transform(SourceIterator begin, SourceSentinel end, OutputIterator output, BinaryOp op) {
    for (; begin != end; ++begin, ++output) {
        *output = op(*begin);
    }
}

template<class Iter, class S, class Iter2, class S2,
         class BinaryPredicate = MAKE_BIN_OP(std::equal_to, ValueTypeIterable<IterableA>)>
bool equal(Iter begin, S end, Iter2 begin2, S2 end2, BinaryPredicate predicate = {}) {
    for (; begin != end && begin2 != end2; ++begin, ++begin2) {
        if (!predicate(*begin, *begin2)) {
            return false;
        }
    }
    return begin == end && begin2 == end2;
}

template<class Iter, class S, class T, class Predicate>
Iter lowerBound(Iter begin, S end, const T& value, Predicate predicate) {
    auto count = detail::distance(begin, end);
    while (count > 0) {
        const auto step = count / 2;
        auto it = begin;
        std::advance(it, step);
        if (predicate(*it, value)) {
            begin = ++it;
            count -= step + 1;
        }
        else {
            count = step;
        }
    }
    return begin;
}

template<class Iter, class S, class T, class Predicate>
bool binarySearch(Iter begin, S end, const T& value, Predicate predicate) {
    Iter it = detail::lowerBound(std::move(begin), end, value, std::move(predicate));
    return it != end && *it == value;
}
} // namespace detail
} // namespace lz

#endif
