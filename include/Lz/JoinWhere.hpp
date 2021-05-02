#pragma once

#ifndef LZ_LEFT_JOIN_HPP
#define LZ_LEFT_JOIN_HPP

#include "detail/BasicIteratorView.hpp"
#include "detail/JoinWhereIterator.hpp"

namespace lz {
	template<class IterA, class IterB, class SelectorA, class SelectorB, class ResultSelector>
	class JoinWhere final :
		public internal::BasicIteratorView<internal::JoinWhereIterator<IterA, IterB, SelectorA, SelectorB, ResultSelector>> {

	public:
		using iterator = internal::JoinWhereIterator<IterA, IterB, SelectorA, SelectorB, ResultSelector>;
		using const_iterator = iterator;
		using value_type = typename iterator::value_type;

	public:
		LZ_CONSTEXPR_CXX_20 JoinWhere(IterA iterA, IterA endA, IterB iterB, IterB endB, SelectorA a, SelectorB b,
									  ResultSelector resultSelector) :
			internal::BasicIteratorView<iterator>(iterator(std::move(iterA), endA, std::move(iterB), endB, a, b, resultSelector),
												  iterator(endA, endA, endB, endB, a, b, resultSelector)) {}

		constexpr JoinWhere() = default;
	};


	/**
	 * Performs an SQL-like join where the result of the function `a` is compared with `b, and returns `resultSelector` if those are equal.
	 * The selector for a must be a function with a parameter of type = `*iterA`.
	 * The selector for b must be a function with a parameter of type = `*iterB`.
	 * The selector for the result must be a function with parameters: `fn(decltype(*iterA), decltype(*iterB)`. It may return anything.
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
	LZ_CONSTEXPR_CXX_20 JoinWhere<IterA, IterB, SelectorA, SelectorB, ResultSelector>
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
	 * @attention iterableB must be sorted in order to work correctly.
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
	LZ_CONSTEXPR_CXX_20 JoinWhere<internal::IterTypeFromIterable<IterableA>, internal::IterTypeFromIterable<IterableB>, SelectorA,
		SelectorB, ResultSelector>
	joinWhere(IterableA&& iterableA, IterableB&& iterableB, SelectorA a, SelectorB b, ResultSelector resultSelector) {
		return joinWhere(internal::begin(std::forward<IterableA>(iterableA)), internal::end(std::forward<IterableA>(iterableA)),
						 internal::begin(std::forward<IterableB>(iterableB)), internal::end(std::forward<IterableB>(iterableB)),
						 std::move(a), std::move(b), std::move(resultSelector));
	}
}

#endif // LZ_LEFT_JOIN_HPP
