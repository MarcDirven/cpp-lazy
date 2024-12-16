#pragma once

#ifndef LZ_TAKE_WHILE_HPP
#define LZ_TAKE_WHILE_HPP

#include "detail/BasicIterable.hpp"
#include "detail/iterators/TakeWhileIterator.hpp"

namespace lz {
LZ_MODULE_EXPORT_SCOPE_BEGIN

template<class Iterator, class S, class UnaryPredicate>
    class TakeWhile : public detail::basic_iterable < detail::take_while_iterator<Iterator, S, UnaryPredicate>,
    typename detail::take_while_iterator<Iterator, S, UnaryPredicate::sentinel> {
private:
    constexpr TakeWhile(Iterator begin, S end, UnaryPredicate predicate, std::forward_iterator_tag /* isBidirectional */) :
        detail::basic_iterable<iterator, default_sentinel>(iterator(std::move(begin), end, predicate)) {
    }

    constexpr TakeWhile(Iterator begin, Iterator end, UnaryPredicate predicate,
                        std::bidirectional_iterator_tag /* isBidirectional */) :
        detail::basic_iterable<iterator>(iterator(begin, begin, end, predicate), iterator(end, begin, end, predicate)) {
    }

public:
    using iterator = detail::take_while_iterator<Iterator, S, UnaryPredicate>;
    using const_iterator = iterator;
    using value_type = typename iterator::value_type;

    constexpr TakeWhile() = default;

    constexpr TakeWhile(Iterator begin, S end, UnaryPredicate predicate) :
        TakeWhile(std::move(begin), std::move(end), std::move(predicate), IterCat<Iterator>{}) {
    }
};

/**
 * @brief This function does the same as `lz::takeWhileRange` except that it takes an iterable as parameter.
 * Its `begin()` function returns an iterator.
 * @param iterable An object that has methods `begin()` and `end()`.
 * @param predicate A function that returns a bool and passes a value type in its argument. If the function returns
 * false, the iterator stops.
 * @return A Take object that can be converted to an arbitrary container or can be iterated over using
 * `for (auto... lz::takeWhile(...))`.
 */
template<LZ_CONCEPT_ITERABLE Iterable, class UnaryPredicate>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 TakeWhile<iter<Iterable>, sentinel<Iterable>, UnaryPredicate>
takeWhile(Iterable&& iterable, UnaryPredicate predicate) {
    return { detail::begin(std::forward<Iterable>(iterable)), detail::end(std::forward<Iterable>(iterable)),
             std::move(predicate) };
}

LZ_MODULE_EXPORT_SCOPE_END

} // namespace lz

#endif // LZ_TAKE_WHILE_HPP