#pragma once

#ifndef LZ_EXCEPT_HPP
#define LZ_EXCEPT_HPP

#include "detail/BasicIteratorView.hpp"
#include "detail/iterators/ExceptIterator.hpp"

namespace lz {

LZ_MODULE_EXPORT_SCOPE_BEGIN

template<LZ_CONCEPT_ITERATOR Iterator, class S, LZ_CONCEPT_ITERATOR IteratorToExcept, class SentinelToExcept, class Comparer>
class Except final
    : public detail::BasicIteratorView<detail::ExceptIterator<Iterator, S, IteratorToExcept, SentinelToExcept, Comparer>,
                                       DefaultSentinel> {

public:
    using iterator = detail::ExceptIterator<Iterator, S, IteratorToExcept, SentinelToExcept, Comparer>;

    using const_iterator = iterator;
    using value_type = typename iterator::value_type;

    Except(Iterator begin, S end, IteratorToExcept toExceptBegin, SentinelToExcept toExceptEnd, Comparer comparer) :
        detail::BasicIteratorView<iterator, DefaultSentinel>(
            iterator(std::move(begin), end, std::move(toExceptBegin), toExceptEnd, comparer)) {
    }

    constexpr Except() = default;
};

/**
 * @addtogroup ItFns
 * @{
 */

/**
 * @brief Skips elements in @p iterable that is contained by @p toExcept. @p toExcept must be sorted manually before creating this
 * view.
 * @attention @p toExcept must be sorted manually before creating this view.
 * @param iterable Sequence to iterate over.
 * @param toExcept Sequence that contains items that must be skipped in `iterable`.
 * @param comparer Comparer for binary search (operator < is default) in IterableToExcept
 * @return An Except view object.
 */
template<LZ_CONCEPT_ITERATOR Iterable, class IterableToExcept,
         class Comparer = MAKE_BIN_OP(std::less, ValueType<IterT<Iterable>>)>
Except<IterT<Iterable>, SentinelT<Iterable>, IterT<IterableToExcept>, SentinelT<IterableToExcept>, Comparer>
except(Iterable&& iterable, IterableToExcept&& toExcept, Comparer comparer = {}) {
    return { detail::begin(std::forward<Iterable>(iterable)), detail::end(std::forward<Iterable>(iterable)),
             detail::begin(std::forward<IterableToExcept>(toExcept)), detail::end(std::forward<IterableToExcept>(toExcept)),
             std::move(comparer) };
}

// End of group
/**
 * @}
 */

LZ_MODULE_EXPORT_SCOPE_END

} // end namespace lz


#endif // end LZ_EXCEPT_HPP