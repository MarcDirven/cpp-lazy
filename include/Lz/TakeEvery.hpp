#pragma once

#ifndef LZ_TAKE_EVERY_HPP
#    define LZ_TAKE_EVERY_HPP

#    include "detail/BasicIteratorView.hpp"
#    include "detail/TakeEveryIterator.hpp"

namespace lz {
template<LZ_CONCEPT_ITERATOR, bool /* isBidirectional */>
class TakeEvery;

template<LZ_CONCEPT_ITERATOR Iterator>
class TakeEvery<Iterator, true> final : public internal::BasicIteratorView<internal::TakeEveryIterator<Iterator, true>> {
public:
    using iterator = internal::TakeEveryIterator<Iterator, true>;
    using const_iterator = iterator;

    using value_type = typename iterator::value_type;

public:
    LZ_CONSTEXPR_CXX_20
    TakeEvery(Iterator begin, Iterator end, const internal::DiffType<Iterator> offset) :
        internal::BasicIteratorView<iterator>(iterator(begin, begin, end, offset), iterator(end, begin, end, offset)) {
    }

    constexpr TakeEvery() = default;
};

template<LZ_CONCEPT_ITERATOR Iterator>
class TakeEvery<Iterator, false> final : public internal::BasicIteratorView<internal::TakeEveryIterator<Iterator, false>> {
public:
    using iterator = internal::TakeEveryIterator<Iterator, false>;
    using const_iterator = iterator;

    using value_type = typename iterator::value_type;

public:
    LZ_CONSTEXPR_CXX_20
    TakeEvery(Iterator begin, Iterator end, const internal::DiffType<Iterator> offset) :
        internal::BasicIteratorView<iterator>(iterator(begin, end, offset), iterator(end, end, offset)) {
    }

    constexpr TakeEvery() = default;
};

// Start of group
/**
 * @addtogroup ItFns
 * @{
 */

/**
 * @brief This iterator can be used to select elements with `offset` amount.
 * @details If one would like to select every 2nd argument one can use this iterator. Example (pseudo code):
 * `takeEvery({1, 2, 3}, 2)`. This will select `1` and `3`. If you would like to skip the first element as well
 * one can use: `takeEvery({1, 2, 3}, 2, 2)` the second `2` is the start indexOf, making it select only `3`.
 * @param begin The beginning of the sequence.
 * @param end The ending of the sequence.
 * @param offset The index to add every iteration, aka the index to 'select'.
 * @param start The start indexOf, optional. Can be used to skip the first element as well. Optional parameter, default is 0.
 * @return A TakeEvery object.
 */
template<LZ_CONCEPT_ITERATOR Iterator>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 TakeEvery<Iterator, internal::IsBidirectional<Iterator>::value>
takeEveryRange(Iterator begin, Iterator end, const internal::DiffType<Iterator> offset,
               const internal::DiffType<Iterator> start = 0) {
    return { std::next(std::move(begin), start), std::move(end), offset };
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
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 TakeEvery<Iterator, internal::IsBidirectional<Iterator>::value>
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