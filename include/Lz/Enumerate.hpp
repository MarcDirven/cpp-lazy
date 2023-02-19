#pragma once

#ifndef LZ_ENUMERATE_HPP
#    define LZ_ENUMERATE_HPP

#    include "detail/BasicIteratorView.hpp"
#    include "detail/EnumerateIterator.hpp"

namespace lz {

LZ_MODULE_EXPORT_SCOPE_BEGIN

template<LZ_CONCEPT_ITERATOR Iterator, LZ_CONCEPT_INTEGRAL IntType>
class Enumerate final : public internal::BasicIteratorView<internal::EnumerateIterator<Iterator, IntType>> {
public:
    using iterator = internal::EnumerateIterator<Iterator, IntType>;
    using const_iterator = iterator;

    using value_type = typename iterator::value_type;

    LZ_CONSTEXPR_CXX_20
    Enumerate(Iterator begin, Iterator end, const internal::DiffType<iterator> distance, const IntType start = 0) :
        internal::BasicIteratorView<iterator>(iterator(start, begin), iterator(static_cast<IntType>(distance), end)) {
    }

    constexpr Enumerate() = default;
};

/**
 * @addtogroup ItFns
 * @{
 */

/**
 * @brief Creates an Enumerate object from two iterators. This can be useful when an index and a
 * value type of a container is needed. If iterator_cat < random_access, then iterator_cat = forward_iterator.
 * @details Creates an Enumerate object. The enumerator consists of a `std::pair<Arithmetic, value_type&>`. The
 * elements of the enumerate iterator are by reference. The `std:::pair<Arithmetic, value_type&>::first` is the
 * counter index. The `std:::pair<Arithmetic, value_type&>::second` is the element of the iterator by reference.
 * Furthermore, the `operator*` of this iterator returns an std::pair by value.
 * @tparam Arithmetic The type of the iterator integer. By default, `int` is assumed. Can be any arithmetic type.
 * @param begin Beginning of the iterator.
 * @param end Ending of the iterator.
 * @param start The start of the counting index. 0 is assumed by default.
 * @return Enumerate iterator object from [begin, end).
 */
template<LZ_CONCEPT_ARITHMETIC Arithmetic = int, LZ_CONCEPT_ITERATOR Iterator>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 Enumerate<Iterator, Arithmetic>
enumerateRange(Iterator begin, Iterator end, const Arithmetic start = 0) {
#    ifndef LZ_HAS_CONCEPTS
    static_assert(std::is_arithmetic<Arithmetic>::value, "the template parameter Arithmetic is meant for arithmetics only");
#    endif
    return { std::move(begin), std::move(end), internal::sizeHint(begin, end), start };
}

/**
 * @brief Creates an Enumerate object from an iterable. This can be useful when an index and a value
 * type of a iterable is needed. If iterator_cat < random_access, then iterator_cat = forward_iterator.
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
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 Enumerate<internal::IterTypeFromIterable<Iterable>, IntType>
enumerate(Iterable&& iterable, const IntType start = 0) {
    return lz::enumerateRange(internal::begin(std::forward<Iterable>(iterable)), internal::end(std::forward<Iterable>(iterable)), start);
}

// End of group
/**
 * @}
 */

LZ_MODULE_EXPORT_SCOPE_END

} // namespace lz

#endif