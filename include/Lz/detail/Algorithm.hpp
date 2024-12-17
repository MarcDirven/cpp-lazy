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
template<class Iterator, class S>
diff_type<Iterator> distance_impl(Iterator begin, S end, std::forward_iterator_tag) {
    diff_type<Iterator> dist = 0;
    for (; begin != end; ++begin, ++dist) {
    }
    return dist;
}

template<class Iterator, class S>
diff_type<Iterator> distance_impl(Iterator begin, S end, std::random_access_iterator_tag) {
    return end - begin;
}

template<class Iterator, class S>
diff_type<Iterator> distance(Iterator begin, S end) {
    return distance_impl(std::move(begin), std::move(end), iter_cat_t<Iterator>{});
}

template<class Iterator, class S, class T, class BinaryPredicate>
T accumulate(Iterator begin, S end, T init, BinaryPredicate binary_op) {
    for (; begin != end; ++begin) {
        init = binary_op(std::move(init), *begin);
    }
    return init;
}

template<class Iterator, class S, class BinaryPredicate>
Iterator max_element(Iterator begin, S end, BinaryPredicate binary_predicate) {
    if (begin == end) {
        return begin;
    }
    auto max = begin;
    for (++begin; begin != end; ++begin) {
        if (binary_predicate(*max, *begin)) {
            max = begin;
        }
    }
    return max;
}

template<class Iterator, class S, class BinaryPredicate>
Iterator min_element(Iterator begin, S end, BinaryPredicate binary_predicate) {
#if defined(__cpp_lib_not_fn) && defined(LZ_HAS_CXX_17)
    return max_element(std::move(begin), std::move(end), std::not_fn(std::move(binary_predicate)));
#else
    using value_type = val_t<Iter>;
    return max_element(std::move(begin), std::move(end),
                       [&binary_predicate](const value_type& a, const value_type& b) { return !binary_predicate(a, b); });
#endif
}

template<class Iterator, class S, class UnaryPredicate>
Iterator find_if(Iterator begin, S end, UnaryPredicate unary_predicate) {
    for (; begin != end; ++begin) {
        if (unary_predicate(*begin)) {
            break;
        }
    }
    return begin;
}

template<class Iterator, class S>
Iterator find(Iterator begin, S end, const val_t<Iterator>& value) {
    return find_if(std::move(begin), std::move(end), [&value](const val_t<Iterator>& val) { return val == value; });
}

template<class Iterator, class S, class T>
enable_if<std::is_same<Iterator, S>::value, Iterator> find_last(Iterator begin, S end, const T& value) {
    std::reverse_iterator<Iterator> rbegin(std::move(end));
    std::reverse_iterator<S> rend(std::move(begin));
    return std::find(std::move(rbegin), std::move(rend), value).base();
}

template<class Iterator, class S, class T>
enable_if<!std::is_same<Iterator, S>::value, Iterator> find_last(Iterator begin, S end, const T& value) {
    auto current = begin;
    for (; begin != end; ++begin) {
        if (*begin == value) {
            current = begin;
        }
    }
    return current;
}

template<class Iterator, class S, class BinaryPredicate>
enable_if<std::is_same<Iterator, S>::value, Iterator> find_last_if(Iterator begin, S end, BinaryPredicate binary_predicate) {
    std::reverse_iterator<Iterator> rbegin{ std::move(end) };
    std::reverse_iterator<S> rend{ std::move(begin) };
    return std::find_if(std::move(rbegin), std::move(rend), std::move(binary_predicate)).base();
}

template<class Iterator, class S, class BinaryPredicate>
enable_if<!std::is_same<Iterator, S>::value, Iterator> find_last_if(Iterator begin, S end, BinaryPredicate binary_predicate) {
    auto current = begin;
    for (; begin != end; ++begin) {
        if (binary_predicate(*begin)) {
            current = begin;
        }
    }
    return current;
}

template<class Iterator, class S, class Iter2, class S2, class BinaryPredicate>
std::pair<Iterator, Iterator> search(Iterator begin, S end, Iter2 begin2, S2 end2, BinaryPredicate binary_predicate) {
    while (begin != end) {
        auto it = begin;
        auto it2 = begin2;
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

template<class Iterator, class S, class UnaryPredicate>
Iterator find_if_not(Iterator begin, S end, UnaryPredicate unary_predicate) {
#if defined(__cpp_lib_not_fn) && defined(LZ_HAS_CXX_17)
    return find_if(std::move(begin), std::move(end), std::not_fn(std::move(unary_predicate)));
#else
    using value_type = val_t<Iter>;
    return find_if(std::move(begin), std::move(end),
                   [&unary_predicate](const value_type& value) { return !unary_predicate(value); });
#endif
}

template<class Iterator, class S, class UnaryPredicate>
Iterator find_last_if_not(Iterator begin, S end, UnaryPredicate unary_predicate) {
#if defined(__cpp_lib_not_fn) && defined(LZ_HAS_CXX_17)
    return find_last_if(std::move(begin), std::move(end), std::not_fn(std::move(unary_predicate)));
#else
    using value_type = val_t<Iter>;
    return find_last_if(std::move(begin), std::move(end),
                        [&unary_predicate](const value_type& value) { return !unary_predicate(value); });
#endif
}

template<class Iterator, class S, class T, class U>
val_t<Iterator> find_or_default(Iterator begin, S end, T&& to_find, U&& default_value) {
    return find(std::move(begin), end, to_find) == end ? std::forward<U>(default_value) : std::forward<T>(to_find);
}

template<class Iterator, class S, class T, class UnaryPredicate>
val_t<Iterator> find_or_default_if(Iterator begin, S end, UnaryPredicate unary_predicate, T&& default_value) {
    auto pos = find_if(std::move(begin), end, std::move(unary_predicate));
    return pos == end ? std::forward<T>(default_value) : *pos;
}

template<class Iterable, class UnaryPredicate, class T, class U>
val_t<Iterable> find_last_or_default(Iterable&& iterable, T&& to_find, U&& default_value) {
    return find_last(std::begin(iterable), std::end(iterable), to_find) == std::end(iterable) ? std::forward<U>(default_value)
                                                                                              : std::forward<T>(to_find);
}

template<class Iterable, class S, class UnaryPredicate, class U>
val_t<Iterable> find_last_or_default_if(Iterable&& iterable, UnaryPredicate unary_predicate, U&& default_value) {
    auto pos = find_last_if(std::begin(iterable), std::end(iterable), std::move(unary_predicate));
    return pos == std::end(iterable) ? std::forward<U>(default_value) : *pos;
}

template<class Iterator, class S, class T>
std::size_t index_of(Iterator begin, S end, const T& value) {
    std::size_t index = 0;
    for (; begin != end; ++begin, ++index) {
        if (*begin == value) {
            return index;
        }
    }
    return index;
}

template<class Iterator, class S, class UnaryPredicate>
std::size_t index_of_if(Iterator begin, S end, UnaryPredicate unary_predicate) {
    std::size_t index = 0;
    for (; begin != end; ++begin, ++index) {
        if (unary_predicate(*begin)) {
            return index;
        }
    }
    return index;
}

template<class Iterator, class S, class T>
bool contains(Iterator begin, S end, const T& value) {
    return find(std::move(begin), std::move(end), value) != end;
}

template<class Iterator1, class S1, class Iterator2, class S2, class BinaryPredicate>
bool starts_with(Iterator1 begin, S1 end, Iterator2 begin2, S2 end2, BinaryPredicate binary_predicate) {
    return search(std::move(begin), std::move(end), std::move(begin2), std::move(end2), std::move(binary_predicate)).first == end;
}

template<class Iterator1, class S1, class Iterator2, class S2, class BinaryPredicate>
bool ends_with(Iterator1 begin, S1 end, Iterator2 begin2, S2 end2, BinaryPredicate binary_predicate) {
    std::reverse_iterator<Iterator1> rbegin(std::move(end));
    std::reverse_iterator<S1> rend(std::move(begin));
    return search(std::move(rbegin), std::move(rend), std::move(begin2), std::move(end2), std::move(binary_predicate)).first ==
           end;
}

template<class Iterator, class S, class UnaryPredicate>
Iterator partition(Iterator begin, S end, UnaryPredicate unary_predicate) {
    begin = find_if_not(begin, end, unary_predicate);
    if (begin == end) {
        return begin;
    }
    for (auto it = begin; it != end; ++it, ++begin) {
        if (unary_predicate(*it)) {
            std::swap(begin, it);
        }
    }
    return begin;
}

template<class Iterator, class S, class BinaryOp>
double mean(Iterator begin, S end, BinaryOp binary_op) {
    diff_type<Iterator> distance{ 0 };
    val_t<Iterator> sum{ 0 };

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

template<class Iterator, class S, class Iter2, class S2, class BinaryPredicate>
bool equal(Iterator begin, S end, Iter2 begin2, S2 end2, BinaryPredicate binary_predicate = {}) {
    for (; begin != end && begin2 != end2; ++begin, ++begin2) {
        if (!binary_predicate(*begin, *begin2)) {
            return false;
        }
    }
    return begin == end && begin2 == end2;
}

template<class Iterator, class S, class T, class BinaryPredicate>
Iterator lower_bound(Iterator begin, S end, const T& value, BinaryPredicate binary_predicate) {
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

template<class Iterator, class S, class T, class BinaryPredicate>
bool binary_search(Iterator begin, S end, const T& value, BinaryPredicate binary_predicate) {
    auto it = detail::lower_bound(std::move(begin), end, value, binary_predicate);
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

template<class Iterator, class S, class UnaryPredicate>
diff_type<Iterator> count_if(Iterator begin, S end, UnaryPredicate unary_predicate) {
    diff_type<Iterator> count = 0;
    for (; begin != end; ++begin) {
        if (unary_predicate(*begin)) {
            ++count;
        }
    }
    return count;
}

template<class Iterator, class S, class T>
diff_type<Iterator> count(Iterator begin, S end, const T& value) {
    return count_if(std::move(begin), end, [&value](const T& val) { return val == value; });
}

template<class Iterator, class S, class BinaryPredicate>
bool is_sorted(Iterator begin, S end, BinaryPredicate binary_predicate) {
    if (begin == end) {
        return true;
    }
    auto next = begin;
    for (++next; next != end; ++begin, ++next) {
        if (binary_predicate(*next, *begin)) {
            return false;
        }
    }
    return true;
}

} // namespace detail
} // namespace lz

#endif
