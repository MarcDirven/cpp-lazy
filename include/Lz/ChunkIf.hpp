#pragma once

#ifndef LZ_CHUNK_IF_HPP
#define LZ_CHUNK_IF_HPP

#include "detail/ChunkIfIterator.hpp"

namespace lz {
#ifdef LZ_HAS_EXECUTION
template<class Iterator, class UnaryPredicate, class Execution>
class ChunkIf final : public internal::BasicIteratorView<internal::ChunkIfIterator<Iterator, UnaryPredicate, Execution>> {
public:
    using iterator = internal::ChunkIfIterator<Iterator, UnaryPredicate, Execution>;
#else  // ^^ LZ_HAS_EXECUTION vv !LZ_HAS_EXECUTION
template<class Iterator, class UnaryPredicate>
class ChunkIf final : public internal::BasicIteratorView<internal::ChunkIfIterator<Iterator, UnaryPredicate>> {
public:
    using iterator = internal::ChunkIfIterator<Iterator, UnaryPredicate>;
#endif // LZ_HAS_EXECUTION
    using const_iterator = iterator;
    using value_type = typename iterator::value_type;

    constexpr ChunkIf() = default;

#ifdef LZ_HAS_EXECUTION
    LZ_CONSTEXPR_CXX_20 ChunkIf(Iterator begin, Iterator end, UnaryPredicate predicate, Execution execution) :
        internal::BasicIteratorView<iterator>(iterator(std::move(begin), end, predicate, execution),
                                              iterator(end, end, predicate, execution)) {
    }
#else  // ^^ LZ_HAS_EXECUTION vv !LZ_HAS_EXECUTION
    ChunkIf(Iterator begin, Iterator end, UnaryPredicate predicate) :
        internal::BasicIteratorView<iterator>(iterator(std::move(begin), end, predicate), iterator(end, end, predicate)) {
    }
#endif // LZ_HAS_EXECUTION
};

/**
 * @addtogroup ItFns
 * @{
 */

#ifdef LZ_HAS_EXECUTION
/**
 * Chops the sequence into pieces of iterators, when `unaryPredicate` function returns true.
 * @param begin The begin of the sequence to chop.
 * @param end The end of the sequence to chop.
 * @param unaryPredicate The predicate that must return a boolean, to indicate whether or not to make a new chunk.
 * @param execution The execution policy (used for `std::find_if`)
 * @return A chunk if iterator view object.
 */
template<class Iterator, class UnaryPredicate, class Execution = std::execution::sequenced_policy>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 ChunkIf<Iterator, UnaryPredicate, Execution>
chunkIfRange(Iterator begin, Iterator end, UnaryPredicate unaryPredicate, Execution execution = std::execution::seq) {
    return { std::move(begin), std::move(end), std::move(unaryPredicate), execution };
}

/**
 * Chops the sequence into pieces of iterators, when `unaryPredicate` function returns true.
 * @param iterable The iterable to chop up.
 * @param unaryPredicate The predicate that must return a boolean, to indicate whether or not to make a new chunk.
 * @param execution The execution policy (used for `std::find_if`)
 * @return A chunk if iterator view object.
 */
template<class Iterable, class UnaryPredicate, class Execution = std::execution::sequenced_policy>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 ChunkIf<internal::IterTypeFromIterable<Iterable>, UnaryPredicate, Execution>
chunkIf(Iterable&& iterable, UnaryPredicate unaryPredicate, Execution execution = std::execution::seq) {
    return chunkIfRange(internal::begin(std::forward<Iterable>(iterable)), internal::end(std::forward<Iterable>(iterable)),
                        std::move(unaryPredicate), execution);
}
#else  // ^^ LZ_HAS_EXECUTION vv !LZ_HAS_EXECUTION
/**
 * Chops the sequence into pieces of iterators, when `unaryPredicate` function returns true.
 * @param begin The begin of the sequence to chop.
 * @param end The end of the sequence to chop.
 * @param unaryPredicate The predicate that must return a boolean, to indicate whether or not to make a new chunk.
 * @return A chunk if iterator view object.
 */
template<class Iterator, class UnaryPredicate>
ChunkIf<Iterator, UnaryPredicate> chunkIfRange(Iterator begin, Iterator end, UnaryPredicate unaryPredicate) {
    return { std::move(begin), std::move(end), std::move(unaryPredicate) };
}

/**
 * Chops the sequence into pieces of iterators, when `unaryPredicate` function returns true.
 * @param iterable The iterable to chop up.
 * @param unaryPredicate The predicate that must return a boolean, to indicate whether or not to make a new chunk.
 * @return A chunk if iterator view object.
 */
template<class Iterable, class UnaryPredicate>
ChunkIf<internal::IterTypeFromIterable<Iterable>, UnaryPredicate> chunkIf(Iterable&& iterable, UnaryPredicate unaryPredicate) {
    return chunkIfRange(internal::begin(std::forward<Iterable>(iterable)), internal::end(std::forward<Iterable>(iterable)),
                        std::move(unaryPredicate));
}
#endif // LZ_HAS_EXECUTION

// End of group
/**
 * @}
 */
} // namespace lz

#endif // LZ_CHUNK_IF_HPP
