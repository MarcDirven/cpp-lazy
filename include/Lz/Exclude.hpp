#pragma once

#ifndef LZ_EXCLUDE_HPP
#define LZ_EXCLUDE_HPP

#include "detail/basic_iterable.hpp"
#include "detail/iterators/exclude.hpp"

namespace lz {

LZ_MODULE_EXPORT_SCOPE_BEGIN

template<class Iterator, class S>
class exclude_iterable final : public detail::basic_iterable<detail::exclude_iterator<Iterator, S>, S> {
public:
    using iterator = detail::exclude_iterator<Iterator, S>;
    using const_iterator = iterator;

    constexpr exclude_iterable() = default;

    LZ_CONSTEXPR_CXX_20
    exclude_iterable(Iterator begin, S end, const diff_type<Iterator> from, const diff_type<Iterator> to) :
        detail::basic_iterable<iterator, S>(iterator(begin, end, from, to), std::move(end)) {
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
 * @return An exclude_iterable iterator view object.
 */
template<LZ_CONCEPT_ITERABLE Iterable>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 exclude_iterable<iter_t<Iterable>, sentinel_t<Iterable>>
exclude(Iterable&& iterable, const diff_type<iter_t<Iterable>> from, const diff_type<iter_t<Iterable>> to) {
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
