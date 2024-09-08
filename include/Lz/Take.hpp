#pragma once

#ifndef LZ_TAKE_HPP
#define LZ_TAKE_HPP

#include "detail/BasicIteratorView.hpp"
#include "detail/iterators/TakeNIterator.hpp"

namespace lz {

LZ_MODULE_EXPORT_SCOPE_BEGIN

template<class Iterator>
class TakeN : public detail::BasicIteratorView<detail::TakeNIterator<Iterator>> {
public:
    using iterator = detail::TakeNIterator<Iterator>;
    using const_iterator = iterator;
    using value_type = typename iterator::value_type;

private:
    template<class IterCat>
    LZ_CONSTEXPR_CXX_20 TakeN(Iterator begin, typename iterator::difference_type n, IterCat /*unused*/) :
        detail::BasicIteratorView<iterator>(iterator(begin, 0), iterator(begin, n)) {
    }

    LZ_CONSTEXPR_CXX_20 TakeN(Iterator begin, typename iterator::difference_type n, std::random_access_iterator_tag /*unused*/) :
        detail::BasicIteratorView<iterator>(iterator(begin, 0), iterator(begin + n, n)) {
    }

public:
    LZ_CONSTEXPR_CXX_20 TakeN(Iterator begin, typename iterator::difference_type n) :
        TakeN(std::move(begin), n, typename std::iterator_traits<Iterator>::iterator_category{}) {
    }

    constexpr TakeN() = default;
};

// Start of group
/**
 * @defgroup ItFns Iterator free functions.
 * These are the iterator functions and can all be used to iterate over in a
 * `for (auto var : lz::someiterator(...))`-like fashion. Also, all objects contain a `toVector`,
 * `toVector<Allocator>`, `toArray<N>`, `to<container>. toMap, toUnorderedMap` (specifying its value type of the container is not
 *  necessary, so e.g. `to<std::list>()` will do), `begin()`, `end()` methods and `value_type` and `iterator`
 *  typedefs.
 * @{
 */

/**
 * @brief Takes the first n elements from the iterator
 * @param begin The begin iterator.
 * @param n The amount of elements to take.
 * @return A TakeN object containing the first `n` elements.
 */
template<LZ_CONCEPT_ITERATOR Iterator>
LZ_NODISCARD LZ_CONSTEXPR_CXX_14 detail::EnableIf<!detail::HasBeginFunc<Iterator>::value, TakeN<Iterator>>
take(Iterator begin, typename std::iterator_traits<Iterator>::difference_type n) {
    return { std::move(begin), n };
}

// clang-format off
/**
 * @brief Takes the first `n` elements from the iterable.
 * @param iterable The iterable to take elements from.
 * @param n The amount of elements to take.
 * @return A TakeN object containing the first `n` elements.
 */
template<LZ_CONCEPT_ITERABLE Iterable>
LZ_NODISCARD
LZ_CONSTEXPR_CXX_14 detail::EnableIf<detail::HasBeginFunc<Iterable>::value, TakeN<detail::IterTypeFromIterable<Iterable>>>
take(Iterable&& iterable, const detail::DiffTypeIterable<Iterable> n) {
    return take(detail::begin(std::forward<Iterable>(iterable)), n);
}

/**
 * @brief Drops the first `n` elements from the iterator.
 * @param begin The begin iterator.
 * @param end The end iterator.
 * @param n The amount of elements to drop.
 * @return A BasicIteratorView containing the iterator with the first `n` elements dropped.
 */
template<LZ_CONCEPT_ITERATOR Iterator>
LZ_NODISCARD
LZ_CONSTEXPR_CXX_14 detail::EnableIf<!detail::HasBeginFunc<Iterator>::value, detail::BasicIteratorView<Iterator>>
dropRange(Iterator begin, Iterator end, const detail::DiffType<Iterator> n) {
    return { std::next(std::move(begin), n), std::move(end) };
}

/**
 * @brief Drops the first `n` elements from the iterable.
 * @param iterable The iterable to drop elements from.
 * @param n The amount of elements to drop.
 * @return A BasicIteratorView containing the iterable with the first `n` elements dropped.
 */
template<LZ_CONCEPT_ITERABLE Iterable>
LZ_NODISCARD
LZ_CONSTEXPR_CXX_14 
detail::EnableIf<detail::HasBeginFunc<Iterable>::value, detail::BasicIteratorView<detail::IterTypeFromIterable<Iterable>>>
drop(Iterable&& iterable, const detail::DiffTypeIterable<Iterable> n) {
    return dropRange(detail::begin(std::forward<Iterable>(iterable)), detail::end(std::forward<Iterable>(iterable)), n);
}
// clang-format on

/**
 * @brief Slices the iterator from `from` to `to`.
 * @param begin The begin iterator.
 * @param end The end iterator.
 * @param from The beginning of the slice.
 * @param to The end of the slice.
 * @return A view containing the iterator sliced from `from` to `to`.
 */
template<LZ_CONCEPT_ITERATOR Iterator>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 detail::EnableIf<!detail::HasBeginFunc<Iterator>::value, TakeN<Iterator>>
slice(Iterator begin, const detail::DiffType<Iterator> from, const detail::DiffType<Iterator> to) {
    LZ_ASSERT(to > from, "to must be greater than from");
    return take(std::next(std::move(begin), from), to - from);
}

// clang-format off
/**
 * @brief Slices the iterable from `from` to `to`.
 * @param iterable The iterable to slice.
 * @param from The beginning of the slice.
 * @param to The end of the slice.
 * @return A view containing the iterable sliced from `from` to `to`.
 */
template<LZ_CONCEPT_ITERABLE Iterable>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20
detail::EnableIf<detail::HasBeginFunc<Iterable>::value, TakeN<detail::IterTypeFromIterable<Iterable>>>
slice(Iterable&& iterable, const detail::DiffTypeIterable<Iterable> from, const detail::DiffTypeIterable<Iterable> to) {
    return slice(detail::begin(std::forward<Iterable>(iterable)), from, to);
}
// clang-format on

// End of group
/**
 * @}
 */

LZ_MODULE_EXPORT_SCOPE_END

} // namespace lz

#endif