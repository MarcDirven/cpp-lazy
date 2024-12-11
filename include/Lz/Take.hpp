#pragma once

#ifndef LZ_TAKE_HPP
#define LZ_TAKE_HPP

#include "detail/BasicIterable.hpp"
#include "detail/iterators/TakeNIterator.hpp"

namespace lz {

LZ_MODULE_EXPORT_SCOPE_BEGIN

template<class Iterator, class S>
class TakeN : public detail::BasicIterable<detail::TakeNIterator<Iterator, S>,
                                               typename detail::TakeNIterator<Iterator, S>::Sentinel> {

public:
    using iterator = detail::TakeNIterator<Iterator, S>;
    using const_iterator = iterator;
    using value_type = typename iterator::value_type;

private:
    using TakeNSent = TakeNSentinel<typename iterator::difference_type>;
    template<class IterCat>
    LZ_CONSTEXPR_CXX_20 TakeN(Iterator begin, typename iterator::difference_type n, IterCat /*unused*/) :
        detail::BasicIterable<iterator, TakeNSent>(iterator(begin, 0), TakeNSent{ n }) {
    }

    LZ_CONSTEXPR_CXX_20 TakeN(Iterator begin, typename iterator::difference_type n, std::random_access_iterator_tag /*unused*/) :
        detail::BasicIterable<iterator>(iterator(begin, 0), iterator(begin + n, n)) {
    }

public:
    LZ_CONSTEXPR_CXX_20 TakeN(Iterator begin, typename iterator::difference_type n) :
        TakeN(std::move(begin), n, IterCat<Iterator>{}) {
    }

    constexpr TakeN() = default;
};

// Start of group
/**
 * @defgroup ItFns Iterator free functions.
 * These are the iterator functions and can all be used to iterate over in a
 * `for (auto var : lz::someiterator(...))`-like fashion. Also, all objects contain a `toVector`, `to<container>`, toMap,
 * toUnorderedMap`. These functions are used to convert the iterator to a container.
 * @{
 */

/**
 * @brief Takes the first `n` elements from the iterable.
 * @param iterable The iterable to take elements from.
 * @param n The amount of elements to take.
 * @return A TakeN object containing the first `n` elements.
 */
template<LZ_CONCEPT_ITERABLE Iterable>
LZ_NODISCARD LZ_CONSTEXPR_CXX_14 TakeN<IterT<Iterable>, SentinelT<Iterable>>
take(Iterable&& iterable, const DiffTypeIterable<Iterable> n) {
    return TakeN<IterT<Iterable>, SentinelT<Iterable>>(detail::begin(std::forward<Iterable>(iterable)), n);
}

/**
 * @brief Drops the first `n` elements from the iterable.
 * @param iterable The iterable to drop elements from.
 * @param n The amount of elements to drop.
 * @return A BasicIterable containing the iterable with the first `n` elements dropped.
 */
template<LZ_CONCEPT_ITERABLE Iterable>
LZ_NODISCARD LZ_CONSTEXPR_CXX_14 detail::BasicIterable<IterT<Iterable>, SentinelT<Iterable>>
drop(Iterable&& iterable, const DiffTypeIterable<Iterable> n) {
    auto next = std::next(std::begin(iterable), n);
    return { std::move(next), detail::end(std::forward<Iterable>(iterable)) };
}

/**
 * @brief Slices the iterable from `from` to `to`.
 * @param iterable The iterable to slice.
 * @param from The beginning of the slice.
 * @param to The end of the slice.
 * @return A view containing the iterable sliced from `from` to `to`.
 */
template<LZ_CONCEPT_ITERABLE Iterable>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 TakeN<IterT<Iterable>, SentinelT<Iterable>>
slice(Iterable&& iterable, const DiffTypeIterable<Iterable> from, const DiffTypeIterable<Iterable> to) {
    auto next = std::next(detail::begin(std::forward<Iterable>(iterable)), from);
    return TakeN<IterT<Iterable>, SentinelT<Iterable>>(std::move(next), to - from);
}

// End of group
/**
 * @}
 */

LZ_MODULE_EXPORT_SCOPE_END

} // namespace lz

#endif