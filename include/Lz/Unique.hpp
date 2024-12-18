#pragma once

#ifndef LZ_UNIQUE_HPP
#define LZ_UNIQUE_HPP

#include "Lz/detail/basic_iterable.hpp"
#include "Lz/detail/iterators/unique.hpp"

namespace lz {

LZ_MODULE_EXPORT_SCOPE_BEGIN

template<LZ_CONCEPT_ITERATOR Iterator, class S, class BinaryOp>
class unique_iterable final : public detail::basic_iterable<detail::unique_iterator<Iterator, S, BinaryOp>,
                                                            typename detail::unique_iterator<Iterator, S, BinaryOp>::sentinel> {
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
        unique_iterable(std::move(begin), std::move(end), std::move(compare), iter_cat_t<Iterator>{}) {
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
 * @param compare The comparer. operator< is assumed by default.
 * @return An unique_iterable iterator view object, which can be used to iterate over in a `(for ... : unqiue(...))`
 * fashion.
 */
template<class Iterable, class BinaryOp = MAKE_BIN_PRED(std::less, value_iterable_t<Iterable>)>
unique_iterable<iter_t<Iterable>, sentinel_t<Iterable>, BinaryOp> unique(Iterable&& iterable, BinaryOp compare = {}) {
    return { detail::begin(std::forward<Iterable>(iterable)), detail::end(std::forward<Iterable>(iterable)), std::move(compare) };
}

// End of group
/**
 * @}
 */

LZ_MODULE_EXPORT_SCOPE_END

} // end namespace lz

#endif // end LZ_UNIQUE_HPP