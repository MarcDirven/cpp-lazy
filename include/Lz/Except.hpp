#pragma once

#ifndef LZ_EXCEPT_HPP
#define LZ_EXCEPT_HPP

#include "Lz/detail/basic_iterable.hpp"
#include "Lz/detail/iterators/except.hpp"

namespace lz {

LZ_MODULE_EXPORT_SCOPE_BEGIN

template<LZ_CONCEPT_ITERATOR Iterator, class S, LZ_CONCEPT_ITERATOR IteratorToExcept, class S2, class BinaryPredicate>
class except_iterable final
    : public detail::basic_iterable<detail::except_iterator<Iterator, S, IteratorToExcept, S2, BinaryPredicate>,
                                    default_sentinel> {

public:
    using iterator = detail::except_iterator<Iterator, S, IteratorToExcept, S2, BinaryPredicate>;

    using const_iterator = iterator;
    using value_type = typename iterator::value_type;

    except_iterable(Iterator begin, S end, IteratorToExcept to_except_begin, S2 to_except_end, BinaryPredicate binary_predicate) :
        detail::basic_iterable<iterator, default_sentinel>(iterator(std::move(begin), std::move(end), std::move(to_except_begin),
                                                                    std::move(to_except_end), std::move(binary_predicate))) {
    }

    constexpr except_iterable() = default;
};

/**
 * @addtogroup ItFns
 * @{
 */

/**
 * @brief Skips elements in @p iterable that is contained by @p to_except. @p to_except must be sorted manually before creating
 * this view.
 * @attention @p to_except must be sorted manually before creating this view.
 * @param iterable Sequence to iterate over.
 * @param to_except Sequence that contains items that must be skipped in `iterable`.
 * @param binary_predicate Comparer for binary search (operator < is default) in IterableToExcept
 * @return An except_iterable view object.
 */
template<LZ_CONCEPT_ITERATOR Iterable, class IterableToExcept,
         class BinaryPredicate = MAKE_BIN_PRED(std::less, value_type<iter_t<Iterable>>)>
except_iterable<iter_t<Iterable>, sentinel_t<Iterable>, iter_t<IterableToExcept>, sentinel_t<IterableToExcept>, BinaryPredicate>
except(Iterable&& iterable, IterableToExcept&& to_except, BinaryPredicate binary_predicate = {}) {
    return { detail::begin(std::forward<Iterable>(iterable)), detail::end(std::forward<Iterable>(iterable)),
             detail::begin(std::forward<IterableToExcept>(to_except)), detail::end(std::forward<IterableToExcept>(to_except)),
             std::move(binary_predicate) };
}

// End of group
/**
 * @}
 */

LZ_MODULE_EXPORT_SCOPE_END

} // end namespace lz

#endif // end LZ_EXCEPT_HPP