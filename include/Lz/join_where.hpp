#pragma once

#ifndef LZ_JOIN_WHERE_HPP
#define LZ_JOIN_WHERE_HPP

#include "Lz/detail/basic_iterable.hpp"
#include "Lz/detail/iterators/join_where.hpp"

namespace lz {

LZ_MODULE_EXPORT_SCOPE_BEGIN

template<class IterA, class SA, class IterB, class SB, class SelectorA, class SelectorB, class ResultSelector>
class join_where_iterable final
    : public detail::basic_iterable<detail::join_where_iterator<IterA, SA, IterB, SB, SelectorA, SelectorB, ResultSelector>,
                                    default_sentinel> {

    using iterator = detail::join_where_iterator<IterA, SA, IterB, SB, SelectorA, SelectorB, ResultSelector>;

public:
    using const_iterator = iterator;
    using value_type = typename iterator::value_type;

    LZ_CONSTEXPR_CXX_20
    join_where_iterable(IterA iter_a, SA end_a, IterB iter_b, SB end_b, SelectorA a, SelectorB b, ResultSelector result_selector) :
        detail::basic_iterable<iterator, default_sentinel>(iterator(std::move(iter_a), std::move(end_a), std::move(iter_b),
                                                                    std::move(end_b), std::move(a), std::move(b),
                                                                    std::move(result_selector))) {
    }

    constexpr join_where_iterable() = default;
};

/**
 * @addtogroup ItFns
 * @{
 */

/**
 * Performs an SQL-like join where the result of the function `a` is compared with `b` using `operator<`, and returns
 * `result_selector` if those are equal. The selector for a must be a function with a parameter of type = `decltype(iterable_a[n])`.
 * The selector for b must be a function with a parameter of type = `decltype(iterable_b[n])`. The selector for the result must be
 * a function with parameters: `fn(decltype(iterable_a[n]), decltype(iterable_b[n])`. It may return anything.
 * @attention iterable_b must be sorted in order to work correctly.
 * @param iterable_a The sequence to join with `iterable_b`.
 * @param iterable_b The sequence to join with `iterable_a`.
 * @param a A function that returns a key-like value to compare the result of `b` with.
 * @param b A function that returns a key-like value to compare the result of `a` with.
 * @param result_selector A function that takes two parameters as its arguments. The value type of iterable `iterable_a` and the
 * value type of iterable `iterable_b`. Once a match of `a == b` is found, this function will be called, and a result can be
 * returned, for e.g. `std::make_tuple(value_type_tA, value_type_tB)`.
 * @return A join where iterator view object, which can be used to iterate over.
 */
template<class IterableA, class IterableB, class SelectorA, class SelectorB, class ResultSelector>
join_where_iterable<iter_t<IterableA>, sentinel_t<IterableA>, iter_t<IterableB>, sentinel_t<IterableB>, SelectorA, SelectorB,
                    ResultSelector>
join_where(IterableA&& iterable_a, IterableB&& iterable_b, SelectorA a, SelectorB b, ResultSelector result_selector) {
    return { detail::begin(std::forward<IterableA>(iterable_a)),
             detail::end(std::forward<IterableA>(iterable_a)),
             detail::begin(std::forward<IterableB>(iterable_b)),
             detail::end(std::forward<IterableB>(iterable_b)),
             std::move(a),
             std::move(b),
             std::move(result_selector) };
}

// End of group
/**
 * @}
 */

LZ_MODULE_EXPORT_SCOPE_END

} // namespace lz

#endif // LZ_JOIN_WHERE_HPP
