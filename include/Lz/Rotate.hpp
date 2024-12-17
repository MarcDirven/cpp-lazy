#pragma once

#ifndef LZ_ROTATE_HPP
#define LZ_ROTATE_HPP

#include "detail/basic_iterable.hpp"
#include "detail/iterators/rotate.hpp"

namespace lz {

LZ_MODULE_EXPORT_SCOPE_BEGIN

template<class Iterator, class S>
class rotate_iterable final : public detail::basic_iterable<detail::rotate_iterator<Iterator, S>,
                                                            typename detail::rotate_iterator<Iterator, S>::sentinel> {

public:
    using iterator = detail::rotate_iterator<Iterator, S>;
    using const_iterator = iterator;

private:
    rotate_iterable(Iterator begin, S end, Iterator start, std::bidirectional_iterator_tag) :
        detail::basic_iterable<iterator>(iterator(begin, end, start, false), iterator(begin, end, start, true)) {
    }

    rotate_iterable(Iterator begin, S end, Iterator start, std::forward_iterator_tag) :
        detail::basic_iterable<iterator, Iterator>(iterator(std::move(begin), std::move(end), start, false), start) {
    }

public:
    rotate_iterable(Iterator begin, S end, Iterator start) :
        rotate_iterable(std::move(begin), std::move(end), std::move(start), iter_cat_t<Iterator>{}) {
    }

    constexpr rotate_iterable() = default;
};

/**
 * @brief Rotates a range. Starts at `start`, `begin` must denote the begin of the sequence, `end` the end of the sequence.
 * @details It ratates a range, meaning:
 * auto range = {1, 2, 3, 4, 5};
 * auto rotated = lz::rotate(range.begin() + 2, range.begin(), range.end()); // -> {3, 4, 5, 1, 2}
 * @param start The start of the range to start the rotation (essentialy begin + some value)
 * @param begin The beginning of the range (container.begin())
 * @param end The ending of the range (container.end())
 * @return rotate_iterable object, which is a range of [start, start)
 */
template<LZ_CONCEPT_ITERABLE Iterable, LZ_CONCEPT_ITERATOR Iterator>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 rotate_iterable<Iterator, sentinel_t<Iterable>> rotate(Iterable&& iterable, Iterator start) {
    static_assert(std::is_same<Iterator, decltype(std::begin(iterable))>::value, "Iterators must be the same type");
    return { detail::begin(std::forward<Iterable>(iterable)), detail::end(std::forward<Iterable>(iterable)), std::move(start) };
}

LZ_MODULE_EXPORT_SCOPE_END

} // namespace lz

#endif // LZ_ROTATE_HPP
