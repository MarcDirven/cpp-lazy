#pragma once

#ifndef LZ_VIEW_HPP
#define LZ_VIEW_HPP

#include "Lz/detail/basic_iterable.hpp"

namespace lz {

using detail::basic_iterable;

// Start of group
/**
 * @addtogroup ItFns
 * @{
 */

/**
 * @brief Returns an iterable to another iterable or container. Can be handy to slice, skip a few first or last elements using:
 * `auto range = lz::to_iterable(vec.begin() + 4, vec.end());`
 * @param begin The beginning of the 'iterable'.
 * @param end The ending of the 'iterable'.
 * @return An iterable object that can be converted to an arbitrary container or can be iterated over using
 * `for (auto... lz::take(...))`.
 */
template<LZ_CONCEPT_ITERATOR Iterator, class S>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 basic_iterable<Iterator, S> to_iterable(Iterator begin, S end) {
    return { begin, end };
}

LZ_MODULE_EXPORT_SCOPE_END
// End of group
/**
 * @}
 */
} // namespace lz

#endif // LZ_VIEW_HPP