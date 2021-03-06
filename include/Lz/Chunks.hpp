#pragma once

#ifndef LZ_CHUNKS_HPP
#define LZ_CHUNKS_HPP

#include "detail/ChunksIterator.hpp"

namespace lz {
template<class Iterator>
class Chunks final : public internal::BasicIteratorView<internal::ChunksIterator<Iterator>> {
public:
    using iterator = internal::ChunksIterator<Iterator>;
    using const_iterator = iterator;
    using value_type = typename iterator::value_type;

    LZ_CONSTEXPR_CXX_20 Chunks(Iterator begin, Iterator end, const std::size_t chunkSize) :
        internal::BasicIteratorView<iterator>(iterator(std::move(begin), end, chunkSize), iterator(end, end, chunkSize)) {
    }

    constexpr Chunks() = default;
};

/**
 * @addtogroup ItFns
 * @{
 */

/**
 * Chops a sequence into chunks of `chunkSize`. The value type of the iterator is another iterator, so a double for loop is
 * necessary to iterate over.
 * @param begin The beginning of the sequence.
 * @param end The ending of the sequence.
 * @param chunkSize The size of the chunks to be.
 * @return A Chunk iterator view object.
 */
template<LZ_CONCEPT_ITERATOR Iterator>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 Chunks<Iterator> chunksRange(Iterator begin, Iterator end, const std::size_t chunkSize) {
    return { std::move(begin), std::move(end), chunkSize };
}

/**
 * Chops a sequence into chunks of `chunkSize`. The value type of the iterator is another iterator, so a double for loop is
 * necessary to iterate over.
 * @param iterable The sequence to be chopped into chunks.
 * @param chunkSize The size of the chunks to be.
 * @return A Chunk iterator view object.
 */
template<LZ_CONCEPT_ITERABLE Iterable>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 Chunks<internal::IterTypeFromIterable<Iterable>>
chunks(Iterable&& iterable, const std::size_t chunkSize) {
    return chunksRange(internal::begin(std::forward<Iterable>(iterable)), internal::end(std::forward<Iterable>(iterable)),
                       chunkSize);
}

// End of group
/**
 * @}
 */

} // namespace lz

#endif // LZ_CHUNKS_HPP
