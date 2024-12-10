#pragma once

#ifndef LZ_CHUNK_IF_HPP
#define LZ_CHUNK_IF_HPP

#include "detail/iterators/ChunkIfIterator.hpp"

namespace lz {

LZ_MODULE_EXPORT_SCOPE_BEGIN

template<class Iterator, class S, class UnaryPredicate>
class ChunkIf final : public detail::BasicIteratorView<detail::ChunkIfIterator<Iterator, S, UnaryPredicate>, DefaultSentinel> {
public:
    using iterator = detail::ChunkIfIterator<Iterator, S, UnaryPredicate>;
    using const_iterator = iterator;
    using value_type = typename iterator::value_type;

    constexpr ChunkIf() = default;

    ChunkIf(Iterator begin, S end, UnaryPredicate predicate) :
        detail::BasicIteratorView<iterator, DefaultSentinel>(iterator(std::move(begin), std::move(end), std::move(predicate))) {
    }
};

/**
 * @addtogroup ItFns
 * @{
 */

/**
 * Chops the sequence into pieces of iterators, when `unaryPredicate` function returns true.
 * @param iterable The iterable to chop up.
 * @param unaryPredicate The predicate that must return a boolean, to indicate whether or not to make a new chunk.
 * @return A chunk if iterator view object.
 */
template<LZ_CONCEPT_ITERABLE Iterable, class UnaryPredicate>
ChunkIf<IterT<Iterable>, SentinelT<Iterable>, UnaryPredicate> chunkIf(Iterable&& iterable, UnaryPredicate unaryPredicate) {
    return { detail::begin(std::forward<Iterable>(iterable)), detail::end(std::forward<Iterable>(iterable)),
             std::move(unaryPredicate) };
}

// End of group
/**
 * @}
 */

LZ_MODULE_EXPORT_SCOPE_END

} // namespace lz


#endif // LZ_CHUNK_IF_HPP
