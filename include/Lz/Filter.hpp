#pragma once

#ifndef LZ_FILTER_HPP
#define LZ_FILTER_HPP

#include "detail/BasicIteratorView.hpp"
#include "detail/iterators/FilterIterator.hpp"

namespace lz {

LZ_MODULE_EXPORT_SCOPE_BEGIN

template<LZ_CONCEPT_ITERATOR Iterator, class S, class UnaryPredicate>
class Filter final : public detail::BasicIteratorView<detail::FilterIterator<Iterator, S, UnaryPredicate>,
                                                      typename detail::FilterIterator<Iterator, S, UnaryPredicate>::Sentinel> {
public:
    using iterator = detail::FilterIterator<Iterator, S, UnaryPredicate>;
    using const_iterator = iterator;
    using value_type = typename iterator::value_type;

private:
    using Sentinel = typename detail::FilterIterator<Iterator, S, UnaryPredicate>::Sentinel;

    Filter(Iterator begin, S end, UnaryPredicate function, std::forward_iterator_tag /* unused */) :
        detail::BasicIteratorView<iterator, DefaultSentinel>(iterator(begin, begin, end, function)) {
    }

    Filter(Iterator begin, Iterator end, UnaryPredicate function, std::bidirectional_iterator_tag /* unused */) :
        detail::BasicIteratorView<iterator, iterator>(iterator(begin, begin, end, function),
                                                      iterator(end, begin, end, function)) {
    }

public:
    Filter(Iterator begin, S end, UnaryPredicate function) :
        Filter(std::move(begin), std::move(end), std::move(function), IterCat<Iterator>{}) {
    }

    constexpr Filter() = default;
};

/**
 * @addtogroup ItFns
 * @{
 */

/**
 * @brief Returns a forward filter iterator. If the `predicate` returns false, the value it is excluded.
 * @details I.e. `lz::filter({1, 2, 3, 4, 5}, [](int i){ return i % 2 == 0; });` will eventually remove all
 * elements that are not even.
 * @param iterable An iterable, e.g. a container / object with `begin()` and `end()` methods.
 * @param predicate A function that must return a bool, and needs a value type of the container as parameter.
 * @return A filter iterator that can be converted to an arbitrary container or can be iterated
 * over using `for (auto... lz::filter(...))`.
 */
template<class Iterable, class UnaryPredicate>
Filter<IterT<Iterable>, SentinelT<Iterable>, UnaryPredicate> filter(Iterable&& iterable, UnaryPredicate predicate) {
    return { detail::begin(std::forward<Iterable>(iterable)), detail::end(std::forward<Iterable>(iterable)),
             std::move(predicate) };
}

// End of group
/**
 * @}
 */

LZ_MODULE_EXPORT_SCOPE_END

} // namespace lz

#endif // end LZ_FILTER_HPP