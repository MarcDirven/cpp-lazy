#pragma once

#ifndef LZ_EXCEPT_HPP
#define LZ_EXCEPT_HPP

#include "detail/BasicIteratorView.hpp"
#include "detail/ExceptIterator.hpp"


namespace lz {
#ifdef LZ_HAS_EXECUTION
	template<LZ_CONCEPT_ITERATOR Iterator, LZ_CONCEPT_ITERATOR IteratorToExcept, class Compare, class Execution>
	class Except final : public internal::BasicIteratorView<internal::ExceptIterator<Iterator, IteratorToExcept, Compare, Execution>> {
#else


	template<LZ_CONCEPT_ITERATOR Iterator, LZ_CONCEPT_ITERATOR IteratorToExcept, class Compare>
	class Except final : public internal::BasicIteratorView<internal::ExceptIterator<Iterator, IteratorToExcept, Compare>> {
#endif
	public:
#ifdef LZ_HAS_EXECUTION
		using iterator = internal::ExceptIterator<Iterator, IteratorToExcept, Compare, Execution>;
#else
		using iterator = internal::ExceptIterator<Iterator, IteratorToExcept, Compare>;
#endif
		using const_iterator = iterator;
		using value_type = typename iterator::value_type;

		/**
		 * Except constructor. Excepts all elements between [begin, end) contained by [toExceptBegin, toExceptEnd)
		 * @param begin The beginning of the iterator to skip.
		 * @param end The ending of the iterator to skip.
		 * @param toExceptBegin The beginning of the actual elements to except.
		 * @param toExceptEnd The ending of the actual elements to except.
		 */
#ifdef LZ_HAS_EXECUTION
		Except(Iterator begin, Iterator end, IteratorToExcept toExceptBegin, IteratorToExcept toExceptEnd, Compare compare,
			   Execution execPolicy) :
			internal::BasicIteratorView<iterator>(iterator(begin, end, toExceptBegin, toExceptEnd, compare, execPolicy),
												  iterator(end, end, toExceptBegin, toExceptEnd, compare, execPolicy))
		{}
#else // ^^^ has execution vvv ! has execution

		Except(Iterator begin, Iterator end, IteratorToExcept toExceptBegin, IteratorToExcept toExceptEnd, Compare compare) :
			internal::BasicIteratorView<iterator>(iterator(std::move(begin), end, std::move(toExceptBegin), toExceptEnd, compare),
												  iterator(end, end, toExceptEnd, toExceptEnd, compare)) {}

#endif // LZ_HAS_EXECUTION

		Except() = default;
	};

	/**
	 * @addtogroup ItFns
	 * @{
	 */

  #ifdef LZ_HAS_EXECUTION
	/**
	 * @brief Skips elements in [begin, end) that is contained by [toExceptBegin, toExceptEnd). [toExceptBegin, toExceptEnd) must be sorted
	 * manually before creating this view.
	 * @attention [toExceptBegin, toExceptEnd) must be sorted  manually before creating this view.
	 * @param execPolicy The std::execution::* policy.
	 * @param begin The beginning of the sequence to skip elements in.
	 * @param end The ending of the sequence to skip elements in.
	 * @param toExceptBegin The beginning of the sequence that may not be contained in [begin, end).
	 * @param toExceptEnd The ending of the sequence that may not be contained in [begin, end).
	 * @param compare Comparer for binary search (operator < is default) in IteratorToExcept.
	 * @return An Except view object.
	 */
	template<LZ_CONCEPT_ITERATOR Iterator, LZ_CONCEPT_ITERATOR IteratorToExcept,
		class Compare = std::less<>, class Execution = std::execution::sequenced_policy>
	Except<Iterator, IteratorToExcept, Compare, Execution>
	exceptRange(Iterator begin, Iterator end, IteratorToExcept toExceptBegin, IteratorToExcept toExceptEnd, Compare compare = {},
				Execution execPolicy = std::execution::seq) {
		return Except<Iterator, IteratorToExcept, Compare, Execution>(std::move(begin), std::move(end), std::move(toExceptBegin),
																	   std::move(toExceptEnd), std::move(compare), execPolicy);
	}

	/**
	 * @brief Skips elements iterable that is contained by toExcept. ToExcept must be sorted manually before creating this view.
	 * @attention ToExcept must be sorted manually before creating this view.
	 * @param execPolicy The std::execution::* policy.
	 * @param iterable Sequence to iterate over.
	 * @param toExcept Sequence that contains items that must be skipped in `iterable`.
	 * @param compare Comparer for binary search (operator < is default) in IterableToExcept
	 * @return An Except view object.
	 */
	template<LZ_CONCEPT_ITERABLE Iterable, LZ_CONCEPT_ITERABLE IterableToExcept, class Compare = std::less<>,
		class Execution = std::execution::sequenced_policy>
	Except<internal::IterTypeFromIterable<Iterable>, internal::IterTypeFromIterable<IterableToExcept>, Compare, Execution>
	except(Iterable&& iterable, IterableToExcept&& toExcept, Compare compare = {}, Execution execPolicy = std::execution::seq) {
		return exceptRange(internal::begin(std::forward<Iterable>(iterable)), internal::end(std::forward<Iterable>(iterable)),
						   internal::begin(std::forward<IterableToExcept>(toExcept)),
						   internal::end(std::forward<IterableToExcept>(toExcept)), std::move(compare), execPolicy);
	}
  #else // ^^^ has execution vvv ! has execution

	/**
	 * @brief Skips elements in [begin, end) that is contained by [toExceptBegin, toExceptEnd). [toExceptBegin, toExceptEnd) must be sorted
	 * manually before creating this view.
	 * @attention [toExceptBegin, toExceptEnd) must be sorted  manually before creating this view.
	 * @param begin The beginning of the sequence to skip elements in.
	 * @param end The ending of the sequence to skip elements in.
	 * @param toExceptBegin The beginning of the sequence that may not be contained in [begin, end).
	 * @param toExceptEnd The ending of the sequence that may not be contained in [begin, end).
	 * @param compare Comparer for binary search (operator < is default) in IteratorToExcept.
	 * @return An Except view object.
	 */
	template<LZ_CONCEPT_ITERATOR Iterator, LZ_CONCEPT_ITERATOR IteratorToExcept,
		class Compare = std::less<internal::ValueType<Iterator>>>
	Except<Iterator, IteratorToExcept, Compare>
	exceptRange(Iterator begin, Iterator end, IteratorToExcept toExceptBegin, IteratorToExcept toExceptEnd, Compare compare = {}) {
		return Except<Iterator, IteratorToExcept, Compare>(std::move(begin), std::move(end), std::move(toExceptBegin),
														   std::move(toExceptEnd), std::move(compare));
	}

	/**
	 * @brief Skips elements iterable that is contained by toExcept. ToExcept must be sorted manually before creating this view.
	 * @attention ToExcept must be sorted manually before creating this view.
	 * @param iterable Sequence to iterate over.
	 * @param toExcept Sequence that contains items that must be skipped in `iterable`.
	 * @param compare Comparer for binary search (operator < is default) in IterableToExcept
	 * @return An Except view object.
	 */
	template<LZ_CONCEPT_ITERABLE Iterable, LZ_CONCEPT_ITERABLE IterableToExcept,
		class Compare = std::less<internal::ValueTypeIterable<Iterable>>>
	Except<internal::IterTypeFromIterable<Iterable>, internal::IterTypeFromIterable<IterableToExcept>, Compare>
	except(Iterable&& iterable, IterableToExcept&& toExcept, Compare compare = {}) {
		return exceptRange(internal::begin(std::forward<Iterable>(iterable)), internal::end(std::forward<Iterable>(iterable)),
						   internal::begin(std::forward<IterableToExcept>(toExcept)),
						   internal::end(std::forward<IterableToExcept>(toExcept)), std::move(compare));
	}

  #endif // LZ_HAS_EXECUTION

	// End of group
	/**
	 * @}
	 */
} // end namespace lz

#endif // end LZ_EXCEPT_HPP