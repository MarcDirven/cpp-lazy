#pragma once

#ifndef LZ_DROP_WHILE_HPP
#define LZ_DROP_WHILE_HPP

#include "Lz/detail/basic_iterable.hpp"

namespace lz {

LZ_MODULE_EXPORT_SCOPE_BEGIN

/**
 * @brief Creates a Take iterator view object.
 * @details This iterator view object can be used to skip values while `predicate` returns true. After the
 * `predicate` returns false, no more values are being skipped.
 * @param iterable The sequence with the values that can be iterated over.
 * @param predicate Function that must return `bool`, and take a `Iterator::value_type` as function parameter.
 * @return A Take iterator view object.
 */
template<LZ_CONCEPT_ITERABLE Iterable, class UnaryPredicate>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 detail::basic_iterable<iter_t<Iterable>, sentinel_t<Iterable>>
drop_while(Iterable&& iterable, UnaryPredicate predicate) {
    auto begin = lz::find_if_not(iterable, std::move(predicate));
    return { std::move(begin), detail::end(std::forward<Iterable>(iterable)) };
}

LZ_MODULE_EXPORT_SCOPE_END

} // namespace lz

#endif // LZ_DROP_WHILE_HPP