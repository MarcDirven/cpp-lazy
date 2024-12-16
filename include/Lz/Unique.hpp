#pragma once

#ifndef LZ_UNIQUE_HPP
#define LZ_UNIQUE_HPP

#include "detail/basic_iterable.hpp"
#include "detail/iterators/unique.hpp"

namespace lz {

LZ_MODULE_EXPORT_SCOPE_BEGIN

template<LZ_CONCEPT_ITERATOR Iterator, class S, class BinaryOp>
class unique_iterable final : public detail::basic_iterable<detail::unique_iterator<Iterator, S, BinaryOp>,
                                                            typename detail::unique_iterator<Iterator, S, BinaryOp::sentinel>> {
public:
    using iterator = detail::unique_iterator<Iterator, S, BinaryOp>;
    using const_iterator = iterator;
    using value_type = typename iterator::value_type;

private:
    unique_iterable(Iterator begin, S end, BinaryOp compare, std::forward_iterator_tag) :
        detail::basic_iterable<iterator, default_sentinel>(iterator(std::move(begin), std::move(end), std::move(compare))) {
    }

    unique_iterable(Iterator begin, Iterator end, BinaryOp compare, std::bidirectional_iterator_tag) :
        detail::basic_iterable<iterator>(iterator(begin, begin, end, compare), iterator(end, begin, end, compare)) {
    }

public:
    unique_iterable(Iterator begin, S end, BinaryOp compare) :
        unique_iterable(std::move(begin), std::move(end), std::move(compare), iter_cat<Iterator>{}) {
    }

    constexpr unique_iterable() = default;
};

// Start of group
/**
 * @addtogroup ItFns
 * @{
 */

/**
 * @brief Returns an unique_iterable iterator view object.
 * @attention `iterable` must be sorted in order to work correctly.
 * @details Use this iterator view to eventually get an iterator of unique_iterable values.
 * @param iterable The iterable sequence.
 * @param sortFunc (Optional) to find adjacent elements.
 * @return An unique_iterable iterator view object, which can be used to iterate over in a `(for ... : unique_iterable(...))`
 * fashion.
 */
#ifdef LZ_HAS_CXX_11
template<class Iterable, class Compare = std::less<value_type_tIterable<Iterable>>>
#else
template<class Iterable, class BinaryOp = std::less<>>
#endif // LZ_HAS_CXX_11
unique_iterable<iter<Iterable>, sentinel<Iterable>, BinaryOp> unique(Iterable&& iterable, BinaryOp compare = {}) {
    return { detail::begin(std::forward<Iterable>(iterable)), detail::end(std::forward<Iterable>(iterable)), std::move(compare) };
}

// End of group
/**
 * @}
 */

LZ_MODULE_EXPORT_SCOPE_END

} // end namespace lz

#endif // end LZ_UNIQUE_HPP