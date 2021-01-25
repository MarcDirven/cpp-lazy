#pragma once

#ifndef LZ_EXCEPT_HPP
#define LZ_EXCEPT_HPP

#include "detail/BasicIteratorView.hpp"
#include "detail/ExceptIterator.hpp"


namespace lz {
#ifdef LZ_HAS_EXECUTION
    template<class Execution, LZ_CONCEPT_ITERATOR Iterator, LZ_CONCEPT_ITERATOR IteratorToExcept, class Compare>
    class Except final : public internal::BasicIteratorView<internal::ExceptIterator<Execution, Iterator, IteratorToExcept, Compare>> {
#else
    template<LZ_CONCEPT_ITERATOR Iterator, LZ_CONCEPT_ITERATOR IteratorToExcept, class Compare>
    class Except final : public internal::BasicIteratorView<internal::ExceptIterator<Iterator, IteratorToExcept, Compare>> {
#endif
    public:
#ifdef LZ_HAS_EXECUTION
        using iterator = internal::ExceptIterator<Execution, Iterator, IteratorToExcept, Compare>;
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
        Except(Iterator begin, Iterator end, IteratorToExcept toExceptBegin, IteratorToExcept toExceptEnd,
			   Compare compare, Execution execPolicy) :
            internal::BasicIteratorView<iterator>(iterator(begin, end, toExceptBegin, toExceptEnd, compare, execPolicy),
                                                  iterator(end, end, toExceptBegin, toExceptEnd, compare, execPolicy))
        {}
#else // ^^^ has execution vvv ! has execution
        Except(Iterator begin, Iterator end, IteratorToExcept toExceptBegin, IteratorToExcept toExceptEnd, Compare compare) :
            internal::BasicIteratorView<iterator>(iterator(begin, end, toExceptBegin, toExceptEnd, compare),
                                                  iterator(end, end, toExceptEnd, toExceptEnd, compare))
    	{}
#endif // end has execution

        Except() = default;
    };

    /**
     * @addtogroup ItFns
     * @{
     */

#ifdef LZ_HAS_EXECUTION
     /**
      * @brief This function returns a view to the random access ExceptIterator.
      * @details This iterator can be used to 'remove'/'except' elements in range from [`begin`, `end`) contained by
      * [`toExceptBegin`, `toExceptEnd). If elements are changed in `toExcept` after calling begin(), this iterator will not
      * be able to do it correctly. The second sequence will always be sorted.
      * @param begin The beginning of the iterator to except elements from contained by [`toExceptBegin`, `toExceptEnd).
      * @param end The ending of the iterator to except elements from contained by [`toExceptBegin`, `toExceptEnd).
      * @param toExceptBegin The beginning of the iterator, containing items that must be removed from [`begin`, `end`).
      * @param toExceptEnd The ending of the iterator, containing items that must be removed from [`begin`, `end`).
      * @param execPolicy The execution policy. Must be one of `std::execution`'s tags. The sorting check, the sorting and finding the
      * elements are done using this policy.
      * @return An Except view object.
      */
    template<class Execution = std::execution::sequenced_policy, LZ_CONCEPT_ITERATOR Iterator, LZ_CONCEPT_RA_ITERATOR IteratorToExcept,
        class Compare = std::less<>>
    LZ_REQUIRES_LESS_THAN(internal::ValueType<Iterator>, internal::ValueType<IteratorToExcept>)
    Except<Execution, Iterator, IteratorToExcept, Compare>
	exceptRange(Iterator begin, Iterator end, IteratorToExcept toExceptBegin, IteratorToExcept toExceptEnd, Compare compare = Compare(),
			    Execution execPolicy = std::execution::seq) {
#ifndef LZ_HAS_CONCEPTS // If no concepts, use static assertion to notify
        static_assert(internal::IsRandomAccess<IteratorToExcept>::value, "The iterator to except must be a random access iterator"
                                                                         "or higher for std::sort");
#endif // end lz has concepts
        return Except<Execution, Iterator, IteratorToExcept, Compare>(std::move(begin), std::move(end), std::move(toExceptBegin),
                                                             		  std::move(toExceptEnd), std::move(compare), execPolicy);
    }

    /**
     * @brief This function returns a view to the ExceptIterator.
     * @details This iterator can be used to 'remove'/'except' elements in range from [`begin`, `end`) contained by
     * [`toExceptBegin`, `toExceptEnd). If elements are changed in `toExcept` after calling begin(), this iterator will not
     * be able to do it correctly. The second sequence will always be sorted.
     * @param iterable The iterable to except elements from contained by `toExcept`.
     * @param toExcept The iterable containing items that must be removed from [`begin`, `end`).
	 * @param compare (Optional) The function used to sort the sequence. Use operator <(=) or operator >(=)
     * @param execPolicy The execution policy. Must be one of `std::execution`'s tags. The sorting check, the sorting and finding the
     * elements are done using this policy.
     * @return An Except view object.
     */
    template<class Execution = std::execution::sequenced_policy, LZ_CONCEPT_ITERABLE Iterable, LZ_CONCEPT_RA_ITERABLE IterableToExcept,
        class I1 = internal::IterTypeFromIterable<Iterable>, class I2 = internal::IterTypeFromIterable<IterableToExcept>,
		class Compare = std::less<>>
    LZ_REQUIRES_LESS_THAN(internal::ValueType<I1>, internal::ValueType<I2>)
    Except<Execution, I1, I2, Compare>
	except(Iterable&& iterable, IterableToExcept&& toExcept, Compare compare = Compare(), Execution execPolicy = std::execution::seq) {
        return exceptRange(internal::begin(std::forward<Iterable>(iterable)), internal::end(std::forward<Iterable>(iterable)),
                           internal::begin(std::forward<IterableToExcept>(toExcept)),
                           internal::end(std::forward<IterableToExcept>(toExcept)), std::move(compare), execPolicy);
    }
#else // ^^^ has execution vvv ! has execution
     /**
      * @brief This function returns a view to the ExceptIterator. If elements are changed in `toExcept` after
      * calling begin(), this iterator will not be able to do it correctly. This iterator has a complexity of O(N log N).
      * @details This iterator can be used to 'remove'/'except' elements in range from [`begin`, `end`) contained by
      * [`toExceptBegin`, `toExceptEnd). The second sequence will always be sorted.
      * @param begin The beginning of the iterator to except elements from contained by [`toExceptBegin`, `toExceptEnd).
      * @param end The ending of the iterator to except elements from contained by [`toExceptBegin`, `toExceptEnd).
      * @param toExceptBegin The beginning of the iterator, containing items that must be removed from [`begin`, `end`).
      * @param toExceptEnd The ending of the iterator, containing items that must be removed from [`begin`, `end`).
	  * @param compare (Optional) The function used to sort the sequence. Use operator <(=) or operator >(=)
      * @return An Except view object.
      */
    template<LZ_CONCEPT_ITERATOR Iterator, LZ_CONCEPT_RA_ITERATOR IteratorToExcept,
        class Compare = std::less<internal::ValueType<Iterator>>>
    Except<Iterator, IteratorToExcept, Compare>
	exceptRange(Iterator begin, Iterator end, IteratorToExcept toExceptBegin, IteratorToExcept toExceptEnd, Compare compare = Compare()) {
        static_assert(internal::IsRandomAccess<IteratorToExcept>::value, "The iterator to except must be a random access iterator"
                                                                         "or higher for std::sort");
        return Except<Iterator, IteratorToExcept, Compare>(std::move(begin), std::move(end), std::move(toExceptBegin),
														   std::move(toExceptEnd), std::move(compare));
    }

     /**
     * @brief This function returns a view to the random access ExceptIterator. If elements are changed in `toExcept` after
     * calling begin(), this iterator will not be able to do it correctly. This iterator has a complexity of O(N log N). The second
     * sequence will always be sorted.
     * @details This iterator can be used to 'remove'/'except' elements in `iterable` contained by `toExcept`. If `toExcept` is sorted
     * and has an `operator<`.
     * @param iterable The iterable to except elements from contained by `toExcept`.
     * @param toExcept The iterable containing items that must be removed from [`begin`, `end`).
	 * @param compare (Optional) The function used to sort the sequence. Use operator <(=) or operator >(=)
     * @return An Except view object.
     */
    template<LZ_CONCEPT_ITERABLE Iterable, LZ_CONCEPT_RA_ITERABLE IterableToExcept,
    	class Compare = std::less<internal::ValueType<internal::IterTypeFromIterable<Iterable>>>>
    Except<internal::IterTypeFromIterable<Iterable>, internal::IterTypeFromIterable<IterableToExcept>, Compare>
    except(Iterable&& iterable, IterableToExcept&& toExcept, Compare compare = Compare()) {
        return exceptRange(internal::begin(std::forward<Iterable>(iterable)), internal::end(std::forward<Iterable>(iterable)),
                           internal::begin(std::forward<IterableToExcept>(toExcept)),
                           internal::end(std::forward<IterableToExcept>(toExcept)), std::move(compare));
    }
#endif // end has execution

    // End of group
    /**
     * @}
     */
} // end namespace lz

#endif // end LZ_EXCEPT_HPP