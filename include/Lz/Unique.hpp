#pragma once

#ifndef LZ_UNIQUE_HPP
#define LZ_UNIQUE_HPP


#include "detail/BasicIteratorView.hpp"
#include "detail/UniqueIterator.hpp"


namespace lz {
#ifdef LZ_HAS_EXECUTION


	template<class Execution, LZ_CONCEPT_ITERATOR Iterator, class Compare>
	class Unique final : public internal::BasicIteratorView<internal::UniqueIterator<Execution, Iterator, Compare>> {
#else


		template<LZ_CONCEPT_ITERATOR Iterator, class Compare>
		class Unique final : public internal::BasicIteratorView<internal::UniqueIterator<Iterator, Compare>> {
#endif
	public:
#ifdef LZ_HAS_EXECUTION
		using iterator = internal::UniqueIterator<Execution, Iterator, Compare>;
#else
		using iterator = internal::UniqueIterator<Iterator, Compare>;
#endif
		using const_iterator = iterator;
		using value_type = typename iterator::value_type;

#ifdef LZ_HAS_EXECUTION

		constexpr Unique(Iterator begin, Iterator end, Compare compare, Execution e) :
			internal::BasicIteratorView<iterator>(iterator(std::move(begin), end, compare, e), iterator(end, end, compare, e)) {
		}

#else

		constexpr Unique(Iterator begin, Iterator end, Compare compare) :
			internal::BasicIteratorView<iterator>(iterator(std::move(begin), end, compare), iterator(end, end, compare)) {
		}

#endif

		constexpr Unique() = default;
	};

	// Start of group
	/**
	 * @addtogroup ItFns
	 * @{
	 */

  #ifdef LZ_HAS_EXECUTION

	/**
	 * @brief Returns an Unique iterator view object.
	 * @attention [begin, end) must be sorted in order to work properly.
	 * @details Use this iterator view to eventually get an iterator of unique values.
	 * @param begin The beginning of the sequence.
	 * @param end The ending of the sequence.
	 * @param execPolicy The execution policy. Must be one of `std::execution`'s tags. Finding the adjacent
	 * element are done using this policy.
	 * @param sortFunc (Optional) to find adjacent elements.
	 * @return An Unique iterator view object, which can be used to iterate over in a `(for ... : uniqueRange(...))` fashion.
	 */
	template<class Execution = std::execution::sequenced_policy, LZ_CONCEPT_ITERATOR Iterator, class Compare = std::less<>>
	constexpr Unique<Execution, Iterator, Compare>
	uniqueRange(Iterator begin, Iterator end, Compare compare = {}, Execution execPolicy = std::execution::seq) {
		return Unique<Execution, Iterator, Compare>(std::move(begin), std::move(end), std::move(compare), execPolicy);
	}

	/**
	 * @brief Returns an Unique iterator view object.
	 * @attention [begin, end) must be sorted in order to work properly.
	 * @details Use this iterator view to eventually get an iterator of unique values.
	 * @param begin The beginning of the sequence.
	 * @param end The ending of the sequence.
	 * @param execPolicy The execution policy. Must be one of `std::execution`'s tags. Finding the adjacent
	 * element are done using this policy.
	 * @param sortFunc (Optional) to find adjacent elements.
	 * @return An Unique iterator view object, which can be used to iterate over in a `(for ... : uniqueRange(...))` fashion.
	 */
	template<class Execution = std::execution::sequenced_policy, LZ_CONCEPT_ITERABLE Iterable,
		class It = internal::IterTypeFromIterable<Iterable>, class Compare = std::less<>>
	constexpr Unique<Execution, It, Compare>
	unique(Iterable&& iterable, Compare compare = {}, Execution execPolicy = std::execution::seq) {
		return uniqueRange(internal::begin(std::forward<Iterable>(iterable)), internal::end(std::forward<Iterable>(iterable)),
						   std::move(compare), execPolicy);
	}

  #else // ^^^ has execution vvv !has execution

	/**
	 * @brief Returns an Unique iterator view object.
	 * @attention [begin, end) must be sorted in order to work properly.
	 * @details Use this iterator view to eventually get an iterator of unique values.
	 * @param begin The beginning of the sequence.
	 * @param end The ending of the sequence.
	 * @param sortFunc (Optional) to find adjacent elements.
	 * @return An Unique iterator view object, which can be used to iterate over in a `(for ... : uniqueRange(...))` fashion.
	 */
	template<class Iterator, class Compare = std::less<internal::ValueType<Iterator>>>
	constexpr Unique<Iterator, Compare> uniqueRange(Iterator begin, Iterator end, Compare compare = {}) {
		return Unique<Iterator, Compare>(std::move(begin), std::move(end), std::move(compare));
	}

	/**
	 * @brief Returns an Unique iterator view object.
	 * @attention `iterable` must be sorted in order to work correctly.
	 * @details Use this iterator view to eventually get an iterator of unique values.
	 * @param iterable The iterable sequence.
	 * @param sortFunc (Optional) to find adjacent elements.
	 * @return An Unique iterator view object, which can be used to iterate over in a `(for ... : unique(...))` fashion.
	 */
	template<class Iterable, class Compare = std::less<internal::ValueTypeIterable<Iterable>>>
	constexpr Unique<internal::IterTypeFromIterable<Iterable>, Compare> unique(Iterable&& iterable, Compare compare = {}) {
		return uniqueRange(internal::begin(std::forward<Iterable>(iterable)), internal::end(std::forward<Iterable>(iterable)),
						   std::move(compare));
	}

  #endif // LZ_HAS_EXECUTION

	// End of group
	/**
	 * @}
	 */
} // end namespace lz

#endif // end LZ_UNIQUE_HPP