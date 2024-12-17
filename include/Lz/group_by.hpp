#pragma once

#ifndef LZ_GROUP_BY_HPP
#define LZ_GROUP_BY_HPP

#include "detail/basic_iterable.hpp"
#include "detail/iterators/group_by.hpp"

namespace lz {

LZ_MODULE_EXPORT_SCOPE_BEGIN
template<class Iterator, class S, class BinaryPredicate>
class group_by_iterable final
    : public detail::basic_iterable<detail::group_by_iterator<Iterator, S, BinaryPredicate>, default_sentinel> {
    using iterator = detail::group_by_iterator<Iterator, S, BinaryPredicate>;
    using const_iterator = iterator;

public:
    group_by_iterable(Iterator begin, S end, BinaryPredicate binary_predicate) :
        detail::basic_iterable<iterator, default_sentinel>(
            iterator(std::move(begin), std::move(end), std::move(binary_predicate))) {
    }

    constexpr group_by_iterable() = default;
};

/**
 * @addtogroup ItFns
 * @{
 */

/**
 * Chops a sequence into chunks, where every chunk is grouped based on a grouping predicate.
 * @attention `iterable` must be sorted in order to work properly.
 * @param iterable The iterable to group into chunks.
 * @param comparer The comparer to make groups with. For e.g. if value_type is string, one can use
 * `[](string a, string b) { return a.length() == b.length() }` to make groups where sizes of the strings are equal.
 * @return A group_by_iterable iterator view object.
 */
template<class Iterable, class BinaryPredicate = MAKE_BIN_PRED(std::equal_to, value_iterable_t<Iterable>)>
group_by_iterable<iter_t<Iterable>, sentinel_t<Iterable>, BinaryPredicate>
group_by(Iterable&& iterable, BinaryPredicate binary_predicate = {}) {
    return { detail::begin(std::forward<Iterable>(iterable)), detail::end(std::forward<Iterable>(iterable)),
             std::move(binary_predicate) };
}

// End of group
/**
 * @}
 */

LZ_MODULE_EXPORT_SCOPE_END

} // namespace lz

#endif // LZ_GROUP_BY_HPP
