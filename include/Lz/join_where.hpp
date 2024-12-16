#pragma once

#ifndef LZ_JOIN_WHERE_HPP
#define LZ_JOIN_WHERE_HPP

#include "detail/BasicIterable.hpp"
#include "detail/iterators/JoinWhereIterator.hpp"

namespace lz {

LZ_MODULE_EXPORT_SCOPE_BEGIN

template<class IterA, class SA, class IterB, class SB, class SelectorA, class SelectorB, class ResultSelector>
class JoinWhere final
    : public detail::basic_iterable<detail::join_where_iterator<IterA, SA, IterB, SB, SelectorA, SelectorB, ResultSelector>,
                                    default_sentinel> {

    using iterator = detail::join_where_iterator<IterA, SA, IterB, SB, SelectorA, SelectorB, ResultSelector>;

public:
    using const_iterator = iterator;
    using value_type = typename iterator::value_type;

    LZ_CONSTEXPR_CXX_20
    JoinWhere(IterA iterA, SA endA, IterB iterB, SB endB, SelectorA a, SelectorB b, ResultSelector resultSelector) :
        detail::basic_iterable<iterator, default_sentinel>(iterator(std::move(iterA), std::move(endA), std::move(iterB),
                                                                    std::move(endB), std::move(a), std::move(b),
                                                                    std::move(resultSelector))) {
    }

    constexpr JoinWhere() = default;
};

/**
 * @addtogroup ItFns
 * @{
 */

/**
 * Performs an SQL-like join where the result of the function `a` is compared with `b` using `operator<`, and returns
 * `resultSelector` if those are equal. The selector for a must be a function with a parameter of type = `decltype(iterableA[n])`.
 * The selector for b must be a function with a parameter of type = `decltype(iterableB[n])`. The selector for the result must be
 * a function with parameters: `fn(decltype(iterableA[n]), decltype(iterableB[n])`. It may return anything.
 * @attention iterableB must be sorted in order to work correctly.
 * @param iterableA The sequence to join with `iterableB`.
 * @param iterableB The sequence to join with `iterableA`.
 * @param a A function that returns a key-like value to compare the result of `b` with.
 * @param b A function that returns a key-like value to compare the result of `a` with.
 * @param resultSelector A function that takes two parameters as its arguments. The value type of iterable `iterableA` and the
 * value type of iterable `iterableB`. Once a match of `a == b` is found, this function will be called, and a result can be
 * returned, for e.g. `std::make_tuple(value_type_tA, value_type_tB)`.
 * @return A join where iterator view object, which can be used to iterate over.
 */
template<class IterableA, class IterableB, class SelectorA, class SelectorB, class ResultSelector>
JoinWhere<iter<IterableA>, sentinel<IterableA>, iter<IterableB>, sentinel<IterableB>, SelectorA, SelectorB, ResultSelector>
joinWhere(IterableA&& iterableA, IterableB&& iterableB, SelectorA a, SelectorB b, ResultSelector resultSelector) {
    return { detail::begin(std::forward<IterableA>(iterableA)),
             detail::end(std::forward<IterableA>(iterableA)),
             detail::begin(std::forward<IterableB>(iterableB)),
             detail::end(std::forward<IterableB>(iterableB)),
             std::move(a),
             std::move(b),
             std::move(resultSelector) };
}

// End of group
/**
 * @}
 */

LZ_MODULE_EXPORT_SCOPE_END

} // namespace lz

#endif // LZ_JOIN_WHERE_HPP
