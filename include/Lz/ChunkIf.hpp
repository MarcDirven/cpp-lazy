#pragma once

#ifndef LZ_CHUNK_IF_HPP
#define LZ_CHUNK_IF_HPP

#include "detail/iterators/ChunkIfIterator.hpp"

namespace lz {

LZ_MODULE_EXPORT_SCOPE_BEGIN

#ifdef LZ_HAS_EXECUTION
template<class Iterator, class UnaryPredicate, class Execution>
class ChunkIf final : public detail::BasicIteratorView<detail::ChunkIfIterator<Iterator, UnaryPredicate, Execution>> {
public:
    using iterator = detail::ChunkIfIterator<Iterator, UnaryPredicate, Execution>;
#else  // ^^ LZ_HAS_EXECUTION vv !LZ_HAS_EXECUTION
template<class Iterator, class UnaryPredicate>
class ChunkIf final : public detail::BasicIteratorView<detail::ChunkIfIterator<Iterator, UnaryPredicate>> {
public:
    using iterator = detail::ChunkIfIterator<Iterator, UnaryPredicate>;
#endif // LZ_HAS_EXECUTION
    using const_iterator = iterator;
    using value_type = typename iterator::value_type;

    constexpr ChunkIf() = default;

#ifdef LZ_HAS_EXECUTION
    LZ_CONSTEXPR_CXX_20 ChunkIf(Iterator begin, Iterator end, UnaryPredicate predicate, Execution execution) :
        detail::BasicIteratorView<iterator>(iterator(begin, begin, end, predicate, execution),
                                            iterator(end, begin, end, predicate, execution)) {
    }
#else  // ^^ LZ_HAS_EXECUTION vv !LZ_HAS_EXECUTION
    ChunkIf(Iterator begin, Iterator end, UnaryPredicate predicate) :
        detail::BasicIteratorView<iterator>(iterator(begin, begin, end, predicate), iterator(end, begin, end, predicate)) {
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
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 ChunkIf<detail::IterTypeFromIterable<Iterable>, UnaryPredicate, Execution>
chunkIf(Iterable&& iterable, UnaryPredicate unaryPredicate, Execution execution = std::execution::seq) {
    return chunkIfRange(detail::begin(std::forward<Iterable>(iterable)), detail::end(std::forward<Iterable>(iterable)),
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
template<LZ_CONCEPT_ITERATOR Iterator, class UnaryPredicate>
ChunkIf<Iterator, UnaryPredicate> chunkIfRange(Iterator begin, Iterator end, UnaryPredicate unaryPredicate) {
    return { std::move(begin), std::move(end), std::move(unaryPredicate) };
}

/**
 * Chops the sequence into pieces of iterators, when `unaryPredicate` function returns true.
 * @param iterable The iterable to chop up.
 * @param unaryPredicate The predicate that must return a boolean, to indicate whether or not to make a new chunk.
 * @return A chunk if iterator view object.
 */
template<LZ_CONCEPT_ITERABLE Iterable, class UnaryPredicate>
ChunkIf<detail::IterTypeFromIterable<Iterable>, UnaryPredicate> chunkIf(Iterable&& iterable, UnaryPredicate unaryPredicate) {
    return chunkIfRange(detail::begin(std::forward<Iterable>(iterable)), detail::end(std::forward<Iterable>(iterable)),
                        std::move(unaryPredicate));
}
#endif // LZ_HAS_EXECUTION

// End of group
/**
 * @}
 */

LZ_MODULE_EXPORT_SCOPE_END

} // namespace lz


#endif // LZ_CHUNK_IF_HPP
