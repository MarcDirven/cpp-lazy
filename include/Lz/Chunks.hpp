#pragma once

#ifndef LZ_CHUNKS_HPP
#    define LZ_CHUNKS_HPP

#    include "detail/ChunksIterator.hpp"

namespace lz {
template<class, bool>
class Chunks;

template<class Iterator>
class Chunks<Iterator, true> final : public internal::BasicIteratorView<internal::ChunksIterator<Iterator, true>> {
public:
    using iterator = internal::ChunksIterator<Iterator, true>;
    using const_iterator = iterator;
    using value_type = typename iterator::value_type;

    LZ_CONSTEXPR_CXX_20
    Chunks(Iterator begin, Iterator end, const std::size_t chunkSize) :
        internal::BasicIteratorView<iterator>(iterator(begin, begin, end, chunkSize), iterator(end, begin, end, chunkSize)) {
    }

    constexpr Chunks() = default;
};

template<class Iterator>
class Chunks<Iterator, false> final : public internal::BasicIteratorView<internal::ChunksIterator<Iterator, false>> {
public:
    using iterator = internal::ChunksIterator<Iterator, false>;
    using const_iterator = iterator;
    using value_type = typename iterator::value_type;

    LZ_CONSTEXPR_CXX_20
    Chunks(Iterator begin, Iterator end, const std::size_t chunkSize) :
        internal::BasicIteratorView<iterator>(iterator(std::move(begin), chunkSize), iterator(std::move(end), chunkSize)) {
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
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 Chunks<Iterator, internal::IsBidirectional<Iterator>::value>
chunksRange(Iterator begin, Iterator end, const std::size_t chunkSize) {
    return { begin, end, chunkSize };
}

/**
 * Chops a sequence into chunks of `chunkSize`. The value type of the iterator is another iterator, so a double for loop is
 * necessary to iterate over.
 * @param iterable The sequence to be chopped into chunks.
 * @param chunkSize The size of the chunks to be.
 * @return A Chunk iterator view object.
 */
template<LZ_CONCEPT_ITERABLE Iterable, class I = internal::IterTypeFromIterable<Iterable>>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 Chunks<I, internal::IsBidirectional<I>::value>
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
