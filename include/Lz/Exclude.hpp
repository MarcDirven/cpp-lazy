#pragma once

#ifndef LZ_EXCLUDE_HPP
#define LZ_EXCLUDE_HPP

#include "detail/BasicIteratorView.hpp"
#include "detail/iterators/ExcludeIterator.hpp"

namespace lz {

LZ_MODULE_EXPORT_SCOPE_BEGIN

template<class Iterator>
class Exclude final : public detail::BasicIteratorView<detail::ExcludeIterator<Iterator>> {
public:
    using iterator = detail::ExcludeIterator<Iterator>;
    using const_iterator = iterator;

    constexpr Exclude() = default;

    LZ_CONSTEXPR_CXX_20
    Exclude(Iterator begin, Iterator end, const detail::DiffType<Iterator> from, const detail::DiffType<Iterator> to) :
        detail::BasicIteratorView<iterator>(iterator(begin, begin, end, from, to), iterator(end, begin, end, from, to)) {
    }
};

/**
 * @addtogroup ItFns
 * @{
 */

/**
 * Excludes a range within [begin, end) where index is [from, to).
 * @param begin The beginning of the sequence.
 * @param end The ending of the sequence.
 * @param from Index to start excluding elements from.
 * @param to Index to stop at, note that its underlying element at this index is included.
 * @return An Exclude iterator view object.
 */
template<LZ_CONCEPT_ITERATOR Iterator>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 Exclude<Iterator>
excludeRange(Iterator begin, Iterator end, const detail::DiffType<Iterator> from, const detail::DiffType<Iterator> to) {
    return { std::move(begin), std::move(end), from, to };
}

/**
 * Excludes a range within `iterable` where index is [from, to).
 * @param iterable The iterable sequence.
 * @param from Index to start excluding elements from.
 * @param to Index to stop at, note that its underlying element at this index is included.
 * @return An Exclude iterator view object.
 */
template<LZ_CONCEPT_ITERABLE Iterable, class Iterator = detail::IterTypeFromIterable<Iterable>>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 Exclude<Iterator>
exclude(Iterable&& iterable, const detail::DiffType<Iterator> from, const detail::DiffType<Iterator> to) {
    return excludeRange(detail::begin(std::forward<Iterable>(iterable)), detail::end(std::forward<Iterable>(iterable)), from, to);
}

// End of group
/**
 * @}
 */

LZ_MODULE_EXPORT_SCOPE_END

} // namespace lz

#endif // LZ_SKIP_HPP
