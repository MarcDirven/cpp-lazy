#pragma once

#ifndef LZ_TAKE_EVERY_HPP
#define LZ_TAKE_EVERY_HPP

#include "Lz/detail/basic_iterable.hpp"
#include "Lz/detail/iterators/take_every.hpp"

namespace lz {

LZ_MODULE_EXPORT_SCOPE_BEGIN

template<LZ_CONCEPT_ITERATOR Iterator, class S>
class take_every_iterable final : public detail::basic_iterable<detail::take_every_iterator<Iterator, S>,
                                                                typename detail::take_every_iterator<Iterator, S>::sentinel> {
public:
    using iterator = detail::take_every_iterator<Iterator, S>;
    using const_iterator = iterator;

    using value_type = typename iterator::value_type;

    LZ_CONSTEXPR_CXX_20
    take_every_iterable(Iterator begin, Iterator end, const diff_type<Iterator> offset, std::bidirectional_iterator_tag) :
        detail::basic_iterable<iterator>(iterator(begin, begin, end, offset), iterator(end, begin, end, offset)) {
    }

    LZ_CONSTEXPR_CXX_20
    take_every_iterable(Iterator begin, S end, const diff_type<Iterator> offset, std::forward_iterator_tag) :
        detail::basic_iterable<iterator, default_sentinel>(iterator(std::move(begin), std::move(end), offset)) {
    }

public:
    LZ_CONSTEXPR_CXX_20
    take_every_iterable(Iterator begin, S end, const diff_type<Iterator> offset) :
        take_every_iterable(std::move(begin), std::move(end), offset, iter_cat_t<Iterator>{}) {
    }

    constexpr take_every_iterable() = default;
};

// Start of group
/**
 * @addtogroup ItFns
 * @{
 */

/**
 * @brief This iterator can be used to select elements with `offset` amount.
 * @details If one would like to select every 2nd argument one can use this iterator. Example (pseudo code):
 * `take_every({1, 2, 3}, 2)`. This will select `1` and `3`. If you would like to skip the first element as well
 * one can use: `take_every({1, 2, 3}, 2, 2)` the second `2` is the start index, making it select only `3`.
 * @param iterable An object that can be iterated over.
 * @param offset The index to add every iteration, aka the index to 'select'.
 * @param start The start index iterator. Can be used to skip the first element as well.
 * @return A take_every_iterable object.
 */
template<LZ_CONCEPT_ITERABLE Iterable>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 take_every_iterable<iter_t<Iterable>, sentinel_t<Iterable>>
take_every(Iterable&& iterable, const diff_type<iter_t<Iterable>> offset, iter_t<Iterable> start) {
    return { std::move(start), detail::end(std::forward<Iterable>(iterable)), offset };
}

/**
 * @brief This iterator can be used to select elements with `offset` amount.
 * @details If one would like to select every 2nd argument one can use this iterator. Example (pseudo code):
 * `take_every({1, 2, 3}, 2)`. This will select `1` and `3`. If you would like to skip the first element as well
 * one can use: `take_every({1, 2, 3}, 2, 2)` the second `2` is the start index, making it select only `3`.
 * @param iterable An object that can be iterated over.
 * @param offset The index to add every iteration, aka the index to 'select'.
 * @return A take_every_iterable object.
 */
template<LZ_CONCEPT_ITERABLE Iterable>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 take_every_iterable<iter_t<Iterable>, sentinel_t<Iterable>>
take_every(Iterable&& iterable, const diff_type<iter_t<Iterable>> offset) {
    return { detail::begin(std::forward<Iterable>(iterable)), detail::end(std::forward<Iterable>(iterable)), offset };
}

// End of group
/**
 * @}
 */

LZ_MODULE_EXPORT_SCOPE_END

} // namespace lz

#endif