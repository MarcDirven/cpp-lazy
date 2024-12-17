#pragma once

#ifndef LZ_DROP_HPP
#define LZ_DROP_HPP

#include <Lz/detail/basic_iterable.hpp>

namespace lz {

LZ_MODULE_EXPORT_SCOPE_BEGIN

/**
 * @brief Drops the first `n` elements from the iterable.
 * @param iterable The iterable to drop elements from.
 * @param n The amount of elements to drop.
 * @return A basic_iterable containing the iterable with the first `n` elements dropped.
 */
template<LZ_CONCEPT_ITERABLE Iterable>
LZ_NODISCARD LZ_CONSTEXPR_CXX_14 detail::basic_iterable<iter_t<Iterable>, sentinel_t<Iterable>>
drop(Iterable&& iterable, const diff_iterable_t<Iterable> n) {
    auto next = std::next(std::begin(iterable), n);
    return { std::move(next), detail::end(std::forward<Iterable>(iterable)) };
}

LZ_MODULE_EXPORT_SCOPE_END

} // namespace lz

#endif // LZ_DROP_HPP