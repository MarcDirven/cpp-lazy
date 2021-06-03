#pragma once

#ifndef LZ_TAKE_EVERY_HPP
#define LZ_TAKE_EVERY_HPP

#include "detail/BasicIteratorView.hpp"
#include "detail/TakeEveryIterator.hpp"

namespace lz {
template<LZ_CONCEPT_ITERATOR Iterator>
class TakeEvery final : public internal::BasicIteratorView<internal::TakeEveryIterator<Iterator>> {
  public:
    using iterator = internal::TakeEveryIterator<Iterator>;
    using const_iterator = iterator;

    using value_type = typename iterator::value_type;

    /**
     * @brief TakeEvery constructor.
     * @param begin The beginning of the sequence.
     * @param end The ending of the sequence.
     * @param offset The offset to add each iteration, aka the amount of elements to skip.
     * @param distance The distance between `begin` and `end`.
     */
    constexpr TakeEvery(Iterator begin, Iterator end, const std::size_t offset, typename iterator::difference_type distance)
        : internal::BasicIteratorView<iterator>(iterator(std::move(begin), end, offset, distance),
                                                iterator(end, end, offset, distance)) {
    }

    constexpr TakeEvery() = default;
};

// Start of group
/**
 * @addtogroup ItFns
 * @{
 */

/**
 * @brief This iterator can be used to select elements with `offset` amount. If MSVC and the type is
 * an STL iterator, pass a pointer iterator, not an actual iterator object.
 * @details If one would like to select every 2nd argument one can use this iterator. Example (pseudo code):
 * `takeEvery({1, 2, 3}, 2)`. This will select `1` and `3`. If you would like to skip the first element as well
 * one can use: `takeEvery({1, 2, 3}, 2, 2)` the second `2` is the start indexOf, making it select only `3`.
 * @param begin The beginning of the sequence.
 * @param end The ending of the sequence.
 * @param offset The index to add every iteration, aka the index to 'select'.
 * @param start The start indexOf, optional. Can be used to skip the first element as well.
 * @return A TakeEvery object.
 */
template<LZ_CONCEPT_ITERATOR Iterator>
LZ_NODISCARD constexpr TakeEvery<Iterator> takeEveryRange(Iterator begin, Iterator end, const internal::DiffType<Iterator> offset,
                                                          const internal::DiffType<Iterator> start = 0) {
    using lz::distance;
    using lz::next;
    using std::distance;
    using std::next;
    return TakeEvery<Iterator>(next(begin, start), end, offset, distance(begin, end));
}

/**
 * @brief This iterator can be used to select elements with `offset` amount.
 * @details If one would like to select every 2nd argument one can use this iterator. Example (pseudo code):
 * `takeEvery({1, 2, 3}, 2)`. This will select `1` and `3`. If you would like to skip the first element as well
 * one can use: `takeEvery({1, 2, 3}, 2, 2)` the second `2` is the start indexOf, making it select only `3`.
 * @param iterable An object that can be iterated over.
 * @param offset The index to add every iteration, aka the index to 'select'.
 * @param start The start indexOf, optional. Can be used to skip the first element as well.
 * @return A TakeEvery object.
 */
template<LZ_CONCEPT_ITERABLE Iterable, class Iterator = internal::IterTypeFromIterable<Iterable>>
LZ_NODISCARD constexpr TakeEvery<Iterator>
takeEvery(Iterable&& iterable, const internal::DiffType<Iterator> offset, const internal::DiffType<Iterator> start = 0) {
    return takeEveryRange(internal::begin(std::forward<Iterable>(iterable)), internal::end(std::forward<Iterable>(iterable)),
                          offset, start);
}

// End of group
/**
 * @}
 */
} // namespace lz

#endif