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

        /**
         * @brief Creates an Unique iterator view object.
         * @details Use this iterator view to eventually get an iterator of unique values. The operator< and operator== must be
         * implemented.
         * @param begin The beginning of the sequence.
         * @param end The ending of the sequence.
         */
#ifdef LZ_HAS_EXECUTION
        Unique(Iterator begin, Iterator end, Compare compare, Execution e) :
            internal::BasicIteratorView<iterator>(iterator(begin, end, compare, e), iterator(end, end, compare, e))
        {
        }
#else
        Unique(Iterator begin, Iterator end, Compare compare) :
            internal::BasicIteratorView<iterator>(iterator(std::move(begin), end, compare), iterator(end, end, compare))
        {
        }
#endif

        Unique() = default;
    };

    // Start of group
    /**
     * @addtogroup ItFns
     * @{
     */

#ifdef LZ_HAS_EXECUTION
    /**
     * @brief Returns an Unique iterator view object.
     * @details Use this iterator view to eventually get an iterator of unique values. The operator< and operator== must be implemented.
     * @param begin The beginning of the sequence.
     * @param end The ending of the sequence.
     * @param execPolicy The execution policy. Must be one of `std::execution`'s tags. The sorting check, sorting and finding the adjacent
     * element are done using this policy.
     * @param sortFunc (Optional) The function used to sort the sequence. Use operator <(=) or operator >(=)
     * @return An Unique iterator view object, which can be used to iterate over in a `(for ... : uniqueRange(...))` fashion.
     */
    template<class Execution = std::execution::sequenced_policy, LZ_CONCEPT_RA_ITERATOR Iterator, class Compare = std::less<>>
    LZ_REQUIRES_LESS_THAN(Iterator, Iterator)
    Unique<Execution, Iterator, Compare>
	uniqueRange(Iterator begin, Iterator end, Compare compare = {}, Execution execPolicy = std::execution::seq) {
#ifndef LZ_HAS_CONCEPTS // If no concepts, use static assertion to notify
        static_assert(internal::IsRandomAccess<Iterator>::value, "The iterator to except must be a random access iterator or higher for "
                                                                 "std::sort");
#endif // end lz has concepts
        return Unique<Execution, Iterator, Compare>(std::move(begin), std::move(end), std::move(compare), execPolicy);
    }

    /**
     * @brief Returns an Unique iterator view object.
     * @details Use this iterator view to eventually get an iterator of unique values. The operator< and operator== must be implemented.
     * @param begin The beginning of the sequence.
     * @param end The ending of the sequence.
     * @param execPolicy The execution policy. Must be one of `std::execution`'s tags. The sorting check, sorting and finding the adjacent
     * element are done using this policy.
     * @param sortFunc (Optional) The function used to sort the sequence. Use operator <(=) or operator >(=)
     * @return An Unique iterator view object, which can be used to iterate over in a `(for ... : uniqueRange(...))` fashion.
     */
    template<class Execution = std::execution::sequenced_policy, LZ_CONCEPT_RA_ITERABLE Iterable,
    	class It = internal::IterTypeFromIterable<Iterable>, class Compare = std::less<>>
    LZ_REQUIRES_LESS_THAN(It, It)
    Unique<Execution, It, Compare> unique(Iterable&& iterable, Compare compare = {}, Execution execPolicy = std::execution::seq) {
        return uniqueRange(internal::begin(std::forward<Iterable>(iterable)), internal::end(std::forward<Iterable>(iterable)),
						   std::move(compare), execPolicy);
    }

#else // ^^^ has execution vvv !has execution
    /**
     * @brief Returns an Unique iterator view object.
     * @details Use this iterator view to eventually get an iterator of unique values. The operator< and operator== must be implemented.
     * @param begin The beginning of the sequence.
     * @param end The ending of the sequence.
     * @param sortFunc (Optional) The function used to sort the sequence. Use operator <(=) or operator >(=)
     * @return An Unique iterator view object, which can be used to iterate over in a `(for ... : uniqueRange(...))` fashion.
     */
    template<LZ_CONCEPT_RA_ITERATOR Iterator, class Compare = std::less<internal::ValueType<Iterator>>>
    Unique<Iterator, Compare> uniqueRange(Iterator begin, Iterator end, Compare compare = {}) {
        static_assert(internal::IsRandomAccess<Iterator>::value, "The iterator to except must be a random access iterator "
                                                                 "or higher for std::sort");
        return Unique<Iterator, Compare>(std::move(begin), std::move(end), std::move(compare));
    }

    /**
     * @brief Returns an Unique iterator view object.
     * @details Use this iterator view to eventually get an iterator of unique values. The operator< and operator== must be implemented.
     * @param iterable The iterable sequence.
     * @param sortFunc (Optional) The function used to sort the sequence. Use operator <(=) or operator >(=)
     * @return An Unique iterator view object, which can be used to iterate over in a `(for ... : unique(...))` fashion.
     */
    template<LZ_CONCEPT_RA_ITERABLE Iterable, class Compare = std::less<internal::ValueTypeIterable<Iterable>>>
    Unique<internal::IterTypeFromIterable<Iterable>, Compare> unique(Iterable&& iterable, Compare compare = {}) {
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