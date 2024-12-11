#pragma once

#ifndef LZ_CHUNKS_HPP
#define LZ_CHUNKS_HPP

#include "detail/iterators/ChunksIterator.hpp"

namespace lz {

LZ_MODULE_EXPORT_SCOPE_BEGIN

template<class Iterator, class S>
class Chunks final : public detail::BasicIterable<detail::ChunksIterator<Iterator, S>, DefaultSentinel> {
public:
    using iterator = detail::ChunksIterator<Iterator, S>;
    using const_iterator = iterator;
    using value_type = typename iterator::value_type;

    LZ_CONSTEXPR_CXX_20
    Chunks(Iterator begin, S end, const std::size_t chunkSize) :
        detail::BasicIterable<iterator, DefaultSentinel>(iterator(std::move(begin), std::move(end), chunkSize)) {
    }

    constexpr Chunks() = default;
};

template<class Iterator>
class Chunks<Iterator, Iterator> final : public detail::BasicIterable<detail::ChunksIterator<Iterator, Iterator>> {
public:
    using iterator = detail::ChunksIterator<Iterator, Iterator>;
    using const_iterator = iterator;
    using value_type = typename iterator::value_type;

    LZ_CONSTEXPR_CXX_20
    Chunks(Iterator begin, Iterator end, const std::size_t chunkSize) :
        detail::BasicIterable<iterator>(iterator(begin, begin, end, chunkSize), iterator(end, begin, end, chunkSize)) {
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
 * @param iterable The sequence to be chopped into chunks.
 * @param chunkSize The size of the chunks to be.
 * @return A Chunk iterator view object.
 */
template<LZ_CONCEPT_ITERABLE Iterable>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 Chunks<IterT<Iterable>, SentinelT<Iterable>>
chunks(Iterable&& iterable, const std::size_t chunkSize) {
    return { detail::begin(std::forward<Iterable>(iterable)), detail::end(std::forward<Iterable>(iterable)), chunkSize };
}

// End of group
/**
 * @}
 */

LZ_MODULE_EXPORT_SCOPE_END

} // namespace lz

#endif // LZ_CHUNKS_HPP
