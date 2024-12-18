#pragma once

#ifndef LZ_ENUMERATE_HPP
#define LZ_ENUMERATE_HPP

#include "Lz/detail/basic_iterable.hpp"
#include "Lz/detail/iterators/enumerate.hpp"

namespace lz {

LZ_MODULE_EXPORT_SCOPE_BEGIN

template<LZ_CONCEPT_ITERATOR Iterator, class S, LZ_CONCEPT_INTEGRAL IntType>
class enumerate_iterable final
    : public detail::basic_iterable<detail::enumerate_iterator<Iterator, S, IntType>,
                                    typename detail::enumerate_iterator<Iterator, S, IntType>::sentinel> {

public:
    using iterator = detail::enumerate_iterator<Iterator, S, IntType>;
    using const_iterator = iterator;

    using value_type = typename iterator::value_type;

    constexpr enumerate_iterable() = default;

private:
    LZ_CONSTEXPR_CXX_20
    enumerate_iterable(Iterator begin, Iterator end, std::random_access_iterator_tag /* unused */, const IntType start = 0) :
        detail::basic_iterable<iterator, iterator>(iterator(start, begin), iterator(static_cast<IntType>(end - begin), end)) {
    }

    LZ_CONSTEXPR_CXX_20
    enumerate_iterable(Iterator begin, S end, std::forward_iterator_tag /* unused */, const IntType start = 0) :
        detail::basic_iterable<iterator, S>(iterator(start, std::move(begin)), std::move(end)) {
    }

public:
    LZ_CONSTEXPR_CXX_20 enumerate_iterable(Iterator begin, S end, const IntType start = 0) :
        enumerate_iterable(std::move(begin), std::move(end), iter_cat_t<iterator>{}, start) {
    }
};

/**
 * @addtogroup ItFns
 * @{
 */

/**
 * @brief Creates an enumerate object from an iterable. This can be useful when an index and a value
 * type of an iterable is needed.
 * @details Creates an enumerate object. The enumerator consists of a `std::pair<IntType, value_type&>`. The
 * elements of the enumerate iterator are by reference. The `std:::pair<IntType, value_type&>::first` is the
 * counter index. The `std:::pair<IntType, value_type&>::second` is the element of the iterator by reference.
 * Furthermore, the `operator*` of this iterator returns an std::pair by value.
 * @tparam IntType The type of the iterator integer. By default, `int` is assumed. Can be any arithmetic type.
 * @param iterable An iterable, e.g. a container / object with `begin()` and `end()` methods.
 * @param start The start of the counting index. 0 is assumed by default.
 * @return enumerate iterator object. One can iterate over this using `for (auto pair : lz::enumerate(..))`
 */
template<LZ_CONCEPT_ARITHMETIC IntType = int, LZ_CONCEPT_ITERABLE Iterable>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 enumerate_iterable<iter_t<Iterable>, sentinel_t<Iterable>, IntType>
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