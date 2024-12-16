#pragma once

#ifndef LZ_SLICE_HPP
#define LZ_SLICE_HPP

#include "Lz/Take.hpp"

namespace lz {

LZ_MODULE_EXPORT_SCOPE_BEGIN

/**
 * @brief Slices the iterable from `from` to `to`.
 * @param iterable The iterable to slice.
 * @param from The beginning of the slice.
 * @param to The end of the slice.
 * @return A view containing the iterable sliced from `from` to `to`.
 */
template<LZ_CONCEPT_ITERABLE Iterable>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 TakeN<iter<Iterable>>
slice(Iterable&& iterable, const diff_iterable_t<Iterable> from, const diff_iterable_t<Iterable> to) {
    auto next = std::next(detail::begin(std::forward<Iterable>(iterable)), from);
    return TakeN<iter<Iterable>>(std::move(next), to - from);
}

LZ_MODULE_EXPORT_SCOPE_END

} // namespace lz

#endif // LZ_SLICE_HPP