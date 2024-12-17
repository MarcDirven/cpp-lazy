#pragma once

#ifndef LZ_FILTER_HPP
#define LZ_FILTER_HPP

#include "detail/basic_iterable.hpp"
#include "detail/iterators/filter.hpp"

namespace lz {

LZ_MODULE_EXPORT_SCOPE_BEGIN

template<LZ_CONCEPT_ITERATOR Iterator, class S, class UnaryPredicate>
class filter_iterable final
    : public detail::basic_iterable<detail::filter_iterator<Iterator, S, UnaryPredicate>,
                                    typename detail::filter_iterator<Iterator, S, UnaryPredicate>::sentinel> {
public:
    using iterator = detail::filter_iterator<Iterator, S, UnaryPredicate>;
    using const_iterator = iterator;
    using value_type = typename iterator::value_type;

private:
    using Sentinel = typename detail::filter_iterator<Iterator, S, UnaryPredicate>::sentinel;

    filter_iterable(Iterator begin, S end, UnaryPredicate function, std::forward_iterator_tag /* unused */) :
        detail::basic_iterable<iterator, default_sentinel>(iterator(begin, begin, end, function)) {
    }

    filter_iterable(Iterator begin, Iterator end, UnaryPredicate function, std::bidirectional_iterator_tag /* unused */) :
        detail::basic_iterable<iterator, iterator>(iterator(begin, begin, end, function), iterator(end, begin, end, function)) {
    }

public:
    filter_iterable(Iterator begin, S end, UnaryPredicate function) :
        filter_iterable(std::move(begin), std::move(end), std::move(function), iter_cat_t<Iterator>{}) {
    }

    constexpr filter_iterable() = default;
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
filter_iterable<iter_t<Iterable>, sentinel_t<Iterable>, UnaryPredicate> filter(Iterable&& iterable, UnaryPredicate predicate) {
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