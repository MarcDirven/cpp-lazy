#pragma once

#ifndef LZ_ENUMERATE_HPP
#define LZ_ENUMERATE_HPP

#include "detail/BasicIterable.hpp"
#include "detail/iterators/EnumerateIterator.hpp"

namespace lz {

LZ_MODULE_EXPORT_SCOPE_BEGIN

template<LZ_CONCEPT_ITERATOR Iterator, class S, LZ_CONCEPT_INTEGRAL IntType>
class Enumerate final : public detail::BasicIterable<detail::EnumerateIterator<Iterator, S, IntType>,
                                                         typename detail::EnumerateIterator<Iterator, S, IntType>::Sentinel> {

public:
    using iterator = detail::EnumerateIterator<Iterator, S, IntType>;
    using const_iterator = iterator;

    using value_type = typename iterator::value_type;

    constexpr Enumerate() = default;

private:
    LZ_CONSTEXPR_CXX_20
    Enumerate(Iterator begin, Iterator end, std::random_access_iterator_tag /* unused */, const IntType start = 0) :
        detail::BasicIterable<iterator, iterator>(iterator(start, begin),
                                                      iterator(static_cast<IntType>(detail::sizeHint(begin, end)), end)) {
    }

    LZ_CONSTEXPR_CXX_20 Enumerate(Iterator begin, S end, std::forward_iterator_tag /* unused */, const IntType start = 0) :
        detail::BasicIterable<iterator, S>(iterator(start, std::move(begin)), std::move(end)) {
    }

public:
    LZ_CONSTEXPR_CXX_20 Enumerate(Iterator begin, S end, const IntType start = 0) :
        Enumerate(std::move(begin), std::move(end), IterCat<iterator>{}, start) {
    }
};

/**
 * @addtogroup ItFns
 * @{
 */

/**
 * @brief Creates an Enumerate object from an iterable. This can be useful when an index and a value
 * type of an iterable is needed.
 * @details Creates an Enumerate object. The enumerator consists of a `std::pair<IntType, value_type&>`. The
 * elements of the enumerate iterator are by reference. The `std:::pair<IntType, value_type&>::first` is the
 * counter index. The `std:::pair<IntType, value_type&>::second` is the element of the iterator by reference.
 * Furthermore, the `operator*` of this iterator returns an std::pair by value.
 * @tparam IntType The type of the iterator integer. By default, `int` is assumed. Can be any arithmetic type.
 * @param iterable An iterable, e.g. a container / object with `begin()` and `end()` methods.
 * @param start The start of the counting index. 0 is assumed by default.
 * @return Enumerate iterator object. One can iterate over this using `for (auto pair : lz::enumerate(..))`
 */
template<LZ_CONCEPT_ARITHMETIC IntType = int, LZ_CONCEPT_ITERABLE Iterable>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 Enumerate<IterT<Iterable>, SentinelT<Iterable>, IntType>
enumerate(Iterable&& iterable, const IntType start = 0) {
    return { detail::begin(std::forward<Iterable>(iterable)), detail::end(std::forward<Iterable>(iterable)), start };
}

// End of group
/**
 * @}
 */

LZ_MODULE_EXPORT_SCOPE_END

} // namespace lz

#endif