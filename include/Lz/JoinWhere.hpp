#pragma once

#ifndef LZ_LEFT_JOIN_HPP
#define LZ_LEFT_JOIN_HPP

#include "detail/BasicIteratorView.hpp"
#include "detail/JoinWhereIterator.hpp"

namespace lz {
#ifdef LZ_HAS_EXECUTION
    template<class IterA, class IterB, class SelectorA, class SelectorB, class ResultSelector, class Execution>
    class JoinWhere final :
        public internal::BasicIteratorView<internal::JoinWhereIterator<IterA, IterB, SelectorA, SelectorB, ResultSelector,Execution>> {

    public:
        using iterator = internal::JoinWhereIterator<IterA, IterB, SelectorA, SelectorB, ResultSelector, Execution>;
#else
    template<class IterA, class IterB, class SelectorA, class SelectorB, class ResultSelector>
    class JoinWhere final :
        public internal::BasicIteratorView<internal::JoinWhereIterator<IterA, IterB, SelectorA, SelectorB, ResultSelector>> {

    public:
        using iterator = internal::JoinWhereIterator<IterA, IterB, SelectorA, SelectorB, ResultSelector>;
#endif
        using const_iterator = iterator;
        using value_type = typename iterator::value_type;

    public:
#ifdef LZ_HAS_EXECUTION
        JoinWhere(IterA iterA, IterA endA, IterB iterB, IterB endB, SelectorA a, SelectorB b, ResultSelector resultSelector,
                 Execution execution):
            internal::BasicIteratorView<iterator>(iterator(iterA, endA, iterB, endB, a, b, resultSelector, execution),
                                                  iterator(endA, endA, endB, endB, a, b, resultSelector, execution))
        {}
#else
        JoinWhere(IterA iterA, IterA endA, IterB iterB, IterB endB, SelectorA a, SelectorB b, ResultSelector resultSelector):
            internal::BasicIteratorView<iterator>(iterator(std::move(iterA), endA, std::move(iterB), endB, a, b, resultSelector),
                                                  iterator(endA, endA, endB, endB, a, b, resultSelector))
        {}
#endif

        JoinWhere() = default;
    };

#ifdef LZ_HAS_EXECUTION
    /**
     * Performs an SQL-like join where the result of the function `a` is compared with `b, and returns `resultSelector` if those are equal.
     * The selector for a must be a function with a parameter of type = `*iterA`.
     * The selector for b must be a function with a parameter of type = `*iterB`.
     * The selector for the result must be a function with parameters: `fn(decltype(*iterA), decltype(*iterB)`. It may return anything.
     * @param iterA The beginning of the sequence A to join.
     * @param endA The ending of the sequence A to join.
     * @param iterB The beginning of the sequence B to join.
     * @param endB The ending of the sequence B to join.
     * @param a A function that returns a key-like value to compare the result of `b` with.
     * @param b A function that returns a key-like value to compare the result of `a` with.
     * @param resultSelector A function that takes two parameters as its arguments. The value type of iterator a and the value type
     * of iterator b. Once a match of `a == b` is found, this function will be called, and a result can be returned, for e.g.
     * `std::make_tuple(valueTypeA, valueTypeB)`.
     * @param execution The execution policy to check whether the sequence is sorted and to sort it if not.
     * @return A join where iterator view object, which can be used to iterate over.
     */
    template<LZ_CONCEPT_ITERATOR IterA, LZ_CONCEPT_ITERATOR IterB, class SelectorA, class SelectorB, class ResultSelector,
        class Execution = std::execution::sequenced_policy>
    JoinWhere<IterA, IterB, SelectorA, SelectorB, ResultSelector, Execution>
    joinWhere(IterA iterA, IterA endA, IterB iterB, IterB endB, SelectorA a, SelectorB b, ResultSelector resultSelector,
             Execution execution = std::execution::seq) {
        return JoinWhere<IterA, IterB, SelectorA, SelectorB, ResultSelector, Execution>(
            std::move(iterA), std::move(endA), std::move(iterB), std::move(endB), std::move(a), std::move(b), std::move(resultSelector),
            execution);
    }

    /**
     * Performs an SQL-like join where the result of the function `a` is compared with `b, and returns `resultSelector` if those are equal.
     * The selector for a must be a function with a parameter of type = `iterableA[n]`.
     * The selector for b must be a function with a parameter of type = `iterableB[n]`.
     * The selector for the result must be a function with parameters: `fn(decltype(iterableA[n]), decltype(iterableB[n])`.
     *  It may return anything.
     * @param iterableA The sequence to join with `iterableB`.
     * @param iterableB The sequence to join with `iterableA`.
     * @param a A function that returns a key-like value to compare the result of `b` with.
     * @param b A function that returns a key-like value to compare the result of `a` with.
     * @param resultSelector A function that takes two parameters as its arguments. The value type of iterable `iterableA` and the
     * value type of iterable `iterableB`. Once a match of `a == b` is found, this function will be called, and a result can be returned,
     * for e.g. `std::make_tuple(valueTypeA, valueTypeB)`.
     * @param execution The execution policy to check whether the sequence is sorted and to sort it if not.
     * @return A join where iterator view object, which can be used to iterate over.
     */
    template<LZ_CONCEPT_ITERABLE IterableA, LZ_CONCEPT_ITERABLE IterableB, class SelectorA, class SelectorB, class ResultSelector,
        class Execution = std::execution::sequenced_policy>
    JoinWhere<internal::IterTypeFromIterable<IterableA>, internal::IterTypeFromIterable<IterableB>,
        SelectorA, SelectorB, ResultSelector, Execution>
    joinWhere(IterableA&& iterableA, IterableB&& iterableB, SelectorA a, SelectorB b, ResultSelector resultSelector,
             Execution execution = std::execution::seq) {
        return joinWhere(internal::begin(std::forward<IterableA>(iterableA)), internal::end(std::forward<IterableA>(iterableA)),
                        internal::begin(std::forward<IterableB>(iterableB)), internal::end(std::forward<IterableB>(iterableB)),
                        std::move(a), std::move(b), std::move(resultSelector), execution);
    }
#else // ^^^ lz has cxx 17 vvv ! has cxx 17
    /**
     * Performs an SQL-like join where the result of the function `a` is compared with `b, and returns `resultSelector` if those are equal.
     * The selector for a must be a function with a parameter of type = `*iterA`.
     * The selector for b must be a function with a parameter of type = `*iterB`.
     * The selector for the result must be a function with parameters: `fn(decltype(*iterA), decltype(*iterB)`. It may return anything.
     * [iterB, endB) will be sorted. Make sure this is the shortest sequence for optimal performance.
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
        return JoinWhere<IterA, IterB, SelectorA, SelectorB, ResultSelector>(
            std::move(iterA), std::move(endA), std::move(iterB), std::move(endB), std::move(a), std::move(b), std::move(resultSelector));
    }

    /**
     * Performs an SQL-like join where the result of the function `a` is compared with `b, and returns `resultSelector` if those are equal.
     * The selector for a must be a function with a parameter of type = `iterableA[n]`.
     * The selector for b must be a function with a parameter of type = `iterableB[n]`.
     * The selector for the result must be a function with parameters: `fn(decltype(iterableA[n]), decltype(iterableB[n])`.
     * It may return anything.
     * iterableB will be sorted. Make sure this is the shortest sequence for optimal performance.
     * @param iterableA The sequence to join with `iterableB`.
     * @param iterableB The sequence to join with `iterableA`.
     * @param a A function that returns a key-like value to compare the result of `b` with.
     * @param b A function that returns a key-like value to compare the result of `a` with.
     * @param resultSelector A function that takes two parameters as its arguments. The value type of iterable `iterableA` and the
     * value type of iterable `iterableB`. Once a match of `a == b` is found, this function will be called, and a result can be returned,
     * for e.g. `std::make_tuple(valueTypeA, valueTypeB)`.
     * @return A join where iterator view object, which can be used to iterate over.
     */
    template<class IterableA, class IterableB, class SelectorA, class SelectorB, class ResultSelector>
    JoinWhere<internal::IterTypeFromIterable<IterableA>, internal::IterTypeFromIterable<IterableB>, SelectorA, SelectorB, ResultSelector>
    joinWhere(IterableA&& iterableA, IterableB&& iterableB, SelectorA a, SelectorB b, ResultSelector resultSelector) {
        return joinWhere(internal::begin(std::forward<IterableA>(iterableA)), internal::end(std::forward<IterableA>(iterableA)),
            			 internal::begin(std::forward<IterableB>(iterableB)), internal::end(std::forward<IterableB>(iterableB)),
            			 std::move(a), std::move(b), std::move(resultSelector));
    }
#endif

}
#endif // LZ_LEFT_JOIN_HPP
