#pragma once

#ifndef LZ_JOIN_WHERE_HPP
#define LZ_JOIN_WHERE_HPP

#include "detail/BasicIterable.hpp"
#include "detail/iterators/JoinWhereIterator.hpp"

namespace lz {

LZ_MODULE_EXPORT_SCOPE_BEGIN

#ifdef LZ_HAS_EXECUTION
template<class IterA, class IterB, class SelectorA, class SelectorB, class ResultSelector, class Execution>
class JoinWhere final
    : public detail::BasicIterable<detail::JoinWhereIterator<IterA, IterB, SelectorA, SelectorB, ResultSelector, Execution>> {

public:
    using iterator = detail::JoinWhereIterator<IterA, IterB, SelectorA, SelectorB, ResultSelector, Execution>;
#else
template<class IterA, class IterB, class SelectorA, class SelectorB, class ResultSelector>
class JoinWhere final
    : public detail::BasicIterable<detail::JoinWhereIterator<IterA, IterB, SelectorA, SelectorB, ResultSelector>> {
    using iterator = detail::JoinWhereIterator<IterA, IterB, SelectorA, SelectorB, ResultSelector>;
#endif
public:
    using const_iterator = iterator;
    using value_type = typename iterator::value_type;

public:
#ifdef LZ_HAS_EXECUTION
    LZ_CONSTEXPR_CXX_20 JoinWhere(IterA iterA, IterA endA, IterB iterB, IterB endB, SelectorA a, SelectorB b,
                                  ResultSelector resultSelector, Execution execution) :
        detail::BasicIterable<iterator>(
            iterator(std::move(iterA), endA, std::move(iterB), endB, a, b, resultSelector, execution),
            iterator(endA, endA, endB, endB, a, b, resultSelector, execution)) {
    }
#else
    LZ_CONSTEXPR_CXX_20
    JoinWhere(IterA iterA, IterA endA, IterB iterB, IterB endB, SelectorA a, SelectorB b, ResultSelector resultSelector) :
        detail::BasicIterable<iterator>(iterator(std::move(iterA), endA, std::move(iterB), endB, a, b, resultSelector),
                                            iterator(endA, endA, endB, endB, a, b, resultSelector)) {
    }
#endif // LZ_HAS_EXECUTION

    constexpr JoinWhere() = default;
};

/**
 * @addtogroup ItFns
 * @{
 */

#ifdef LZ_HAS_EXECUTION
/**
 * Performs an SQL-like join where the result of the function `a` is compared with `b` using `operator<`, and returns
 * `resultSelector` if those are equal. The selector for a must be a function with a parameter of type = `*iterA`. The selector
 * for b must be a function with a parameter of type = `*iterB`. The selector for the result must be a function with parameters:
 * `fn(decltype(*iterA), decltype(*iterB)`. It may return anything.
 * @attention [iterB, endB) must be sorted in order to work correctly.
 * @param iterA The beginning of the sequence A to join.
 * @param endA The ending of the sequence A to join.
 * @param iterB The beginning of the sequence B to join.
 * @param endB The ending of the sequence B to join.
 * @param a A function that returns a key-like value to compare the result of `b` with.
 * @param b A function that returns a key-like value to compare the result of `a` with.
 * @param resultSelector A function that takes two parameters as its arguments. The value type of iterator a and the value type
 * of iterator b. Once a match of `a == b` is found, this function will be called, and a result can be returned, for e.g.
 * `std::make_tuple(valueTypeA, valueTypeB)`.
 * @param execution The execution policy. Must be any of std::execution::*
 * @return A join where iterator view object, which can be used to iterate over.
 */
template<class IterA, class IterB, class SelectorA, class SelectorB, class ResultSelector,
         class Execution = std::execution::sequenced_policy>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 JoinWhere<IterA, IterB, SelectorA, SelectorB, ResultSelector, Execution>
joinWhere(IterA iterA, IterA endA, IterB iterB, IterB endB, SelectorA a, SelectorB b, ResultSelector resultSelector,
          Execution execution = std::execution::seq) {
    // clang-format off
    return {
        std::move(iterA), std::move(endA), std::move(iterB), std::move(endB), std::move(a), std::move(b),
        std::move(resultSelector), execution
    };
    // clang-format on
}

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
 * returned, for e.g. `std::make_tuple(valueTypeA, valueTypeB)`.
 * @param execution The execution policy. Must be any of std::execution::*
 * @return A join where iterator view object, which can be used to iterate over.
 */
template<class IterableA, class IterableB, class SelectorA, class SelectorB, class ResultSelector,
         class Execution = std::execution::sequenced_policy>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 JoinWhere<IterT<IterableA>, IterT<IterableB>, SelectorA, SelectorB, ResultSelector, Execution>
joinWhere(IterableA&& iterableA, IterableB&& iterableB, SelectorA a, SelectorB b, ResultSelector resultSelector,
          Execution execution = std::execution::seq) {
    return joinWhere(detail::begin(std::forward<IterableA>(iterableA)), detail::end(std::forward<IterableA>(iterableA)),
                     detail::begin(std::forward<IterableB>(iterableB)), detail::end(std::forward<IterableB>(iterableB)),
                     std::move(a), std::move(b), std::move(resultSelector), execution);
}
#else
/**
 * Performs an SQL-like join where the result of the function `a` is compared with `b` using `operator<`, and returns
 * `resultSelector` if those are equal. The selector for a must be a function with a parameter of type = `*iterA`. The selector
 * for b must be a function with a parameter of type = `*iterB`. The selector for the result must be a function with parameters:
 * `fn(decltype(*iterA), decltype(*iterB)`. It may return anything.
 * @attention [iterB, endB) must be sorted in order to work correctly.
 * @param iterA The beginning of the sequence A to join.
 * @param endA The ending of the sequence A to join.
 * @param iterB The beginning of the sequence B to join.
 * @param endB The ending of the sequence B to join.
 * @param a A function that returns a key-like value to compare the result of `b` with.
 * @param b A function that returns a key-like value to compare the result of `a` with.
 * @param resultSelector A function that takes two parameters as its arguments. The value type of iterator a and the value type
 * of iterator b. Once a match of `a == b` is found, this function will be called, and a result can be returned, for e.g.
 * `std::make_tuple(valueTypeA, valueTypeB)`.
 * @return A join where iterator view object, which can be used to iterate over.
 */
template<class IterA, class IterB, class SelectorA, class SelectorB, class ResultSelector>
JoinWhere<IterA, IterB, SelectorA, SelectorB, ResultSelector>
joinWhere(IterA iterA, IterA endA, IterB iterB, IterB endB, SelectorA a, SelectorB b, ResultSelector resultSelector) {
    // clang-format off
    return {
        std::move(iterA), std::move(endA), std::move(iterB),  std::move(endB), std::move(a), std::move(b),
        std::move(resultSelector)
    };
    // clang-format on
}

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
 * returned, for e.g. `std::make_tuple(valueTypeA, valueTypeB)`.
 * @return A join where iterator view object, which can be used to iterate over.
 */
template<class IterableA, class IterableB, class SelectorA, class SelectorB, class ResultSelector>
JoinWhere<IterT<IterableA>, IterT<IterableB>, SelectorA, SelectorB, ResultSelector>
joinWhere(IterableA&& iterableA, IterableB&& iterableB, SelectorA a, SelectorB b, ResultSelector resultSelector) {
    return joinWhere(detail::begin(std::forward<IterableA>(iterableA)), detail::end(std::forward<IterableA>(iterableA)),
                     detail::begin(std::forward<IterableB>(iterableB)), detail::end(std::forward<IterableB>(iterableB)),
                     std::move(a), std::move(b), std::move(resultSelector));
}

#endif // LZ_HAS_EXECUTION

// End of group
/**
 * @}
 */

LZ_MODULE_EXPORT_SCOPE_END

} // namespace lz

#endif // LZ_JOIN_WHERE_HPP
