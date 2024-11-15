#pragma once

#ifndef LZ_VIEW_HPP
#define LZ_VIEW_HPP

#include "Lz/detail/BasicIteratorView.hpp"

namespace lz {
template<LZ_CONCEPT_ITERATOR Iterator>
using View = detail::BasicIteratorView<Iterator>;

// Start of group
/**
 * @addtogroup ItFns
 * @{
 */

/**
 * @brief Returns a view to another view or container. Can be handy to slice, skip a few first or last elements using:
 * `auto range = lz::view(vec.begin() + 4, vec.end());`
 * @param begin The beginning of the 'view'.
 * @param end The ending of the 'view'.
 * @return A View object that can be converted to an arbitrary container or can be iterated over using
 * `for (auto... lz::takeRange(...))`.
 */
template<LZ_CONCEPT_ITERATOR Iterator>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 detail::BasicIteratorView<Iterator> view(Iterator begin, Iterator end) {
    return { begin, end };
}

LZ_MODULE_EXPORT_SCOPE_END
// End of group
/**
 * @}
 */
} // namespace lz

#endif // LZ_VIEW_HPP