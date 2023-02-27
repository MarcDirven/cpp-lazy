#pragma once

#ifndef LZ_ROTATE_HPP
#    define LZ_ROTATE_HPP

#    include "detail/BasicIteratorView.hpp"
#    include "detail/RotateIterator.hpp"

namespace lz {

LZ_MODULE_EXPORT_SCOPE_BEGIN

template<class Iterator>
class Rotate : public internal::BasicIteratorView<internal::RotateIterator<Iterator>> {
public:
    using iterator = internal::RotateIterator<Iterator>;
    using const_iterator = iterator;

    Rotate(Iterator start, Iterator begin, Iterator end) :
        internal::BasicIteratorView<iterator>(iterator(start, begin, end, false), iterator(start, begin, end, true)) {
    }
};

/**
 * @brief Rotates a range. Starts at `start`, `begin` must denote the begin of the sequence, `end` the end of the sequence.
 * @details It ratates a range, meaning:
 * auto range = {1, 2, 3, 4, 5};
 * auto rotated = lz::rotate(range.begin() + 2, range.begin(), range.end()); // -> {3, 4, 5, 1, 2}
 * @param start The start of the range to start the rotation (essentialy begin + some value)
 * @param begin The beginning of the range (container.begin())
 * @param end The ending of the range (container.end())
 * @return Rotate object, which is a range of [start, start)
 */
template<LZ_CONCEPT_ITERATOR Iterator>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 Rotate<Iterator> rotate(Iterator start, Iterator begin, Iterator end) {
    return { std::move(start), std::move(begin), std::move(end) };
}

LZ_MODULE_EXPORT_SCOPE_END

} // namespace lz



#endif // LZ_ROTATE_HPP
