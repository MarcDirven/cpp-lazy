#pragma once

#ifndef LZ_DETAIL_ALGORITHM_HPP
#define LZ_DETAIL_ALGORITHM_HPP

#include "Lz/detail/procs.hpp"
#include "Lz/detail/traits.hpp"

#if defined(__cpp_lib_not_fn) && defined(LZ_HAS_CXX_17)
#include <functional> // std::not_fn
#endif

namespace lz {
namespace detail {
template<class Iter, class S>
diff_type<Iter> distance_impl(Iter begin, S end, std::forward_iterator_tag) {
    diff_type<Iter> dist = 0;
    for (; begin != end; ++begin, ++dist) {
    }
    return dist;
}

template<class Iter, class S>
diff_type<Iter> distance_impl(Iter begin, S end, std::random_access_iterator_tag) {
    return end - begin;
}

template<class Iter, class S>
diff_type<Iter> distance(Iter begin, S end) {
    return distance_impl(std::move(begin), std::move(end), iter_cat<Iter>{});
}

template<class Iter, class S, class T, class BinaryPredicate>
T accumulate(Iter begin, S end, T init, BinaryPredicate binary_op) {
    for (; begin != end; ++begin) {
        init = binary_op(std::move(init), *begin);
    }
    return init;
}

template<class Iter, class S, class BinaryPredicate>
Iter max_element(Iter begin, S end, BinaryPredicate binary_predicate) {
    Iter max = begin;
    for (; begin != end; ++begin) {
        if (binary_predicate(*max, *begin)) {
            max = begin;
        }
    }
    return max;
}

template<class Iter, class S, class BinaryPredicate>
Iter min_element(Iter begin, S end, BinaryPredicate binary_predicate) {
    using value_type = value_type<Iter>;
#if defined(__cpp_lib_not_fn) && defined(LZ_HAS_CXX_17)
    return max_element(std::move(begin), std::move(end), std::not_fn(std::move(binary_predicate)));
#else
    return max_element(std::move(begin), std::move(end),
                       [&binary_predicate](const value_type& a, const value_type& b) { return !binary_predicate(a, b); });
#endif
}

template<class Iter, class S, class UnaryPredicate>
Iter find_if(Iter begin, S end, UnaryPredicate unary_predicate) {
    for (; begin != end; ++begin) {
        if (unary_predicate(*begin)) {
            break;
        }
    }
    return begin;
}

template<class Iter, class S>
Iter find(Iter begin, S end, const value_type<Iter>& value) {
    return find_if(std::move(begin), std::move(end), MAKE_BIN_OP(std::equal_to, value_type<Iter>){});
}

template<class Iter, class S, class Iter2, class S2, class BinaryPredicate>
std::pair<Iter, Iter> search(Iter begin, S end, Iter2 begin2, S2 end2, BinaryPredicate binary_predicate) {
    while (begin != end) {
        Iter it = begin;
        Iter2 it2 = begin2;
        while (it2 != end2 && it != end && binary_predicate(*it, *it2)) {
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

template<class Iter, class S, class UnaryPredicate>
Iter find_if_not(Iter begin, S end, UnaryPredicate unary_predicate) {
#if defined(__cpp_lib_not_fn) && defined(LZ_HAS_CXX_17)
    return find_if(std::move(begin), std::move(end), std::not_fn(std::move(unary_predicate)));
#else
    using value_type = value_type<Iter>;
    return find_if(std::move(begin), std::move(end),
                   [&unary_predicate](const value_type& value) { return !unary_predicate(value); });
#endif
}

template<class Iterator, class S, class T, class U>
value_type<Iterator> find_or_default(Iterator begin, S end, const T& to_find, const U& default_value) {
    return static_cast<value_type<Iterator>>(find(std::move(begin), end, to_find) == end ? default_value : to_find);
}

template<class Iterator, class S, class T, class UnaryPredicate>
value_type<Iterator> find_or_default_if(Iterator begin, S end, UnaryPredicate unary_predicate, const T& default_value) {
    using CastType = value_type<Iterator>;
    const Iterator pos = find_if(std::move(begin), end, std::move(unary_predicate));
    return static_cast<CastType>(pos == end ? default_value : *pos);
}

template<class Iter, class S, class UnaryPredicate>
Iter partition(Iter begin, S end, UnaryPredicate unary_predicate) {
    begin = find_if_not(begin, end, unary_predicate);
    if (begin == end) {
        return begin;
    }
    for (Iter it = begin; it != end; ++it, ++begin) {
        if (unary_predicate(*it)) {
            std::swap(begin, it);
        }
    }
    return begin;
}

template<class Iterator, class S, class BinaryOp>
double mean(Iterator begin, S end, BinaryOp binary_op) {
    diff_type<Iterator> distance{ 0 };
    value_type<Iterator> sum{ 0 };

    for (; begin != end; ++begin, ++distance) {
        sum = binary_op(std::move(sum), *begin);
    }
    if (distance == 0) {
        return distance;
    }
    return static_cast<double>(sum) / static_cast<double>(static_cast<std::size_t>(distance));
}

template<class Iterator, class S, class UnaryOp>
void for_each(Iterator begin, S end, UnaryOp unary_op) {
    for (; begin != end; ++begin) {
        unary_op(*begin);
    }
}

template<class Iterator, class S, class UnaryPredicate>
void for_each_while(Iterator begin, const S& end, UnaryPredicate unary_predicate) {
    for (; begin != end; ++begin) {
        if (!unary_predicate(*begin)) {
            break;
        }
    }
}
template<class SourceIterator, class SourceS, class OutputIterator>
void copy(SourceIterator begin, SourceS end, OutputIterator output) {
    for (; begin != end; ++begin, ++output) {
        *output = *begin;
    }
}

template<class SourceIterator, class SourceSentinel, class OutputIterator, class UnaryOp>
void transform(SourceIterator begin, SourceSentinel end, OutputIterator output, UnaryOp op) {
    for (; begin != end; ++begin, ++output) {
        *output = op(*begin);
    }
}

template<class Iter, class S, class Iter2, class S2, class BinaryPredicate>
bool equal(Iter begin, S end, Iter2 begin2, S2 end2, BinaryPredicate binary_predicate = {}) {
    for (; begin != end && begin2 != end2; ++begin, ++begin2) {
        if (!binary_predicate(*begin, *begin2)) {
            return false;
        }
    }
    return begin == end && begin2 == end2;
}

template<class Iter, class S, class T, class BinaryPredicate>
Iter lower_bound(Iter begin, S end, const T& value, BinaryPredicate binary_predicate) {
    auto count = detail::distance(begin, end);
    while (count > 0) {
        const auto step = count / 2;
        auto it = begin;
        std::advance(it, step);
        if (binary_predicate(*it, value)) {
            begin = ++it;
            count -= step + 1;
        }
        else {
            count = step;
        }
    }
    return begin;
}

template<class Iter, class S, class T, class BinaryPredicate>
bool binary_search(Iter begin, S end, const T& value, BinaryPredicate binary_predicate) {
    Iter it = detail::lower_bound(std::move(begin), end, value, binary_predicate);
    return it != end && !binary_predicate(value, *it);
}

template<class Iterator, class S, class UnaryPredicate>
bool all_of(Iterator iterator, S s, UnaryPredicate unary_predicate) {
    return find_if_not(std::move(iterator), s, std::move(unary_predicate)) == s;
}

template<class Iterator, class S, class UnaryPredicate>
bool any_of(Iterator iterator, S s, UnaryPredicate unary_predicate) {
    return find_if(std::move(iterator), s, std::move(unary_predicate)) != s;
}

template<class Iterator, class S, class UnaryPredicate>
bool none_of(Iterator iterator, S s, UnaryPredicate unary_predicate) {
    return !any_of(std::move(iterator), std::move(s), std::move(unary_predicate));
}

template<class Iterator, class S, class BinaryPredicate>
Iterator adjacent_find(Iterator begin, S end, BinaryPredicate binary_predicate) {
    if (begin == end) {
        return begin;
    }
    auto next = begin;
    for (++next; next != end; ++begin, ++next) {
        if (binary_predicate(*begin, *next)) {
            return begin;
        }
    }
    return next;
}
} // namespace detail
} // namespace lz

#endif
