#pragma once

#ifndef LZ_EXCLUDE_HPP
#define LZ_EXCLUDE_HPP

#include "detail/BasicIterable.hpp"
#include "detail/iterators/ExcludeIterator.hpp"

namespace lz {

LZ_MODULE_EXPORT_SCOPE_BEGIN

template<class Iterator, class S>
class Exclude final : public detail::BasicIterable<detail::ExcludeIterator<Iterator, S>, S> {
public:
    using iterator = detail::ExcludeIterator<Iterator, S>;
    using const_iterator = iterator;

    constexpr Exclude() = default;

    LZ_CONSTEXPR_CXX_20
    Exclude(Iterator begin, S end, const DiffType<Iterator> from, const DiffType<Iterator> to) :
        detail::BasicIterable<iterator, S>(iterator(begin, end, from, to), std::move(end)) {
    }
};

/**
 * @addtogroup ItFns
 * @{
 */

/**
 * Excludes a range within @p iterable where index is [from, to).
 * @param iterable The iterable sequence.
 * @param from Index to start excluding elements from.
 * @param to Index to stop at, note that its underlying element at this index is included.
 * @return An Exclude iterator view object.
 */
template<LZ_CONCEPT_ITERABLE Iterable>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 Exclude<IterT<Iterable>, SentinelT<Iterable>>
exclude(Iterable&& iterable, const DiffType<IterT<Iterable>> from, const DiffType<IterT<Iterable>> to) {
    LZ_ASSERT(from <= to, "from must be less than or equal to `to`");
    return { detail::begin(std::forward<Iterable>(iterable)), detail::end(std::forward<Iterable>(iterable)), from, to };
}

// End of group
/**
 * @}
 */

LZ_MODULE_EXPORT_SCOPE_END

} // namespace lz

#endif // LZ_SKIP_HPP
