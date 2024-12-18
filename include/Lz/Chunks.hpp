#pragma once

#ifndef LZ_CHUNKS_HPP
#define LZ_CHUNKS_HPP

#include "Lz/detail/iterators/chunks.hpp"

namespace lz {

LZ_MODULE_EXPORT_SCOPE_BEGIN

// TODO: make better implementation here? I.e. using iterator_tags

template<class Iterator, class S>
class chunks_iterable final : public detail::basic_iterable<detail::chunks_iterator<Iterator, S>, default_sentinel> {
public:
    using iterator = detail::chunks_iterator<Iterator, S>;
    using const_iterator = iterator;
    using value_type = typename iterator::value_type;

    LZ_CONSTEXPR_CXX_20
    chunks_iterable(Iterator begin, S end, const std::size_t chunk_size) :
        detail::basic_iterable<iterator, default_sentinel>(iterator(std::move(begin), std::move(end), chunk_size)) {
    }

    constexpr chunks_iterable() = default;
};

template<class Iterator>
class chunks_iterable<Iterator, Iterator> final : public detail::basic_iterable<detail::chunks_iterator<Iterator, Iterator>> {
public:
    using iterator = detail::chunks_iterator<Iterator, Iterator>;
    using const_iterator = iterator;
    using value_type = typename iterator::value_type;

    LZ_CONSTEXPR_CXX_20
    chunks_iterable(Iterator begin, Iterator end, const std::size_t chunk_size) :
        detail::basic_iterable<iterator>(iterator(begin, begin, end, chunk_size), iterator(end, begin, end, chunk_size)) {
    }

    constexpr chunks_iterable() = default;
};

/**
 * @addtogroup ItFns
 * @{
 */

/**
 * Chops a sequence into chunks of `chunk_size`. The value type of the iterator is another iterator, so a double for loop is
 * necessary to iterate over.
 * @param iterable The sequence to be chopped into chunks.
 * @param chunk_size The size of the chunks to be.
 * @return A Chunk iterator view object.
 */
template<LZ_CONCEPT_ITERABLE Iterable>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 chunks_iterable<iter_t<Iterable>, sentinel_t<Iterable>>
chunks(Iterable&& iterable, const std::size_t chunk_size) {
    return { detail::begin(std::forward<Iterable>(iterable)), detail::end(std::forward<Iterable>(iterable)), chunk_size };
}

// End of group
/**
 * @}
 */

LZ_MODULE_EXPORT_SCOPE_END

} // namespace lz

#endif // LZ_CHUNKS_HPP
