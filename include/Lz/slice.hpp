#pragma once

#ifndef LZ_SLICE_HPP
#define LZ_SLICE_HPP

#include "Lz/take.hpp"

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
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 take_iterable<iter_t<Iterable>>
slice(Iterable&& iterable, const diff_iterable_t<Iterable> from, const diff_iterable_t<Iterable> to) {
    auto next = std::next(detail::begin(std::forward<Iterable>(iterable)), from);
    return take_iterable<iter_t<Iterable>>(std::move(next), to - from);
}

LZ_MODULE_EXPORT_SCOPE_END

} // namespace lz

#endif // LZ_SLICE_HPP