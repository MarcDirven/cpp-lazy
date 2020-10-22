#pragma once

#ifndef LZ_UNIQUE_HPP
#define LZ_UNIQUE_HPP


#include "detail/BasicIteratorView.hpp"
#include "detail/UniqueIterator.hpp"


namespace lz {
#ifdef LZ_HAS_EXECUTION
    template<class Execution, LZ_CONCEPT_ITERATOR Iterator>
    class Unique final : public internal::BasicIteratorView<internal::UniqueIterator<Execution, Iterator>> {
#else
    template<LZ_CONCEPT_ITERATOR Iterator>
    class Unique final : public internal::BasicIteratorView<internal::UniqueIterator<Iterator>> {
#endif
    public:
#ifdef LZ_HAS_EXECUTION
        using iterator = internal::UniqueIterator<Execution, Iterator>;
#else
        using iterator = internal::UniqueIterator<Iterator>;
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
        Unique(Iterator begin, Iterator end, const Execution e) :
            internal::BasicIteratorView<iterator>(iterator(begin, end, e), iterator(end, end, e))
        {
        }
#else
        Unique(Iterator begin, Iterator end) :
            internal::BasicIteratorView<iterator>(iterator(begin, end), iterator(end, end))
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
     * @return An Unique iterator view object, which can be used to iterate over in a `(for ... : uniqueRange(...))` fashion.
     */
    template<class Execution = std::execution::sequenced_policy, LZ_CONCEPT_RA_ITERATOR Iterator>
    LZ_REQUIRES_LESS_THAN(Iterator, Iterator)
    Unique<Execution, Iterator> uniqueRange(Iterator begin, Iterator end, const Execution execPolicy = std::execution::seq) {
#ifndef LZ_HAS_CONCEPTS // If no concepts, use static assertion to notify
        static_assert(internal::IsRandomAccess<Iterator>::value, "The iterator to except must be a random access iterator or higher for "
                                                                 "std::sort");
#endif // end lz has concepts
        internal::verifyIteratorAndPolicies<Execution, Iterator>();
        return Unique<Execution, Iterator>(std::move(begin), std::move(end), execPolicy);
    }

    /**
     * @brief Returns an Unique iterator view object.
     * @details Use this iterator view to eventually get an iterator of unique values. The operator< and operator== must be implemented.
     * @param begin The beginning of the sequence.
     * @param end The ending of the sequence.
     * @param execPolicy The execution policy. Must be one of `std::execution`'s tags. The sorting check, sorting and finding the adjacent
     * element are done using this policy.
     * @return An Unique iterator view object, which can be used to iterate over in a `(for ... : uniqueRange(...))` fashion.
     */
    template<class Execution = std::execution::sequenced_policy, LZ_CONCEPT_RA_ITERABLE Iterable, class It = internal::IterTypeFromIterable<Iterable>>
    LZ_REQUIRES_LESS_THAN(It, It)
    Unique<Execution, It> unique(Iterable&& iterable, const Execution execPolicy = std::execution::seq) {
        return uniqueRange(std::begin(iterable), std::end(iterable), execPolicy);
    }

#else // ^^^ has execution vvv !has execution
    /**
     * @brief Returns an Unique iterator view object.
     * @details Use this iterator view to eventually get an iterator of unique values. The operator< and operator== must be implemented.
     * @param begin The beginning of the sequence.
     * @param end The ending of the sequence.
     * @return An Unique iterator view object, which can be used to iterate over in a `(for ... : uniqueRange(...))` fashion.
     */
    template<LZ_CONCEPT_RA_ITERATOR Iterator>
    Unique<Iterator> uniqueRange(Iterator begin, Iterator end) {
        static_assert(internal::IsRandomAccess<Iterator>::value, "The iterator to except must be a random access iterator"
                                                                 "or higher for std::sort");
        return Unique<Iterator>(std::move(begin), std::move(end));
    }

    /**
     * @brief Returns an Unique iterator view object.
     * @details Use this iterator view to eventually get an iterator of unique values. The operator< and operator== must be implemented.
     * @param iterable The iterable sequence.
     * @return An Unique iterator view object, which can be used to iterate over in a `(for ... : unique(...))` fashion.
     */
    template<LZ_CONCEPT_RA_ITERABLE Iterable>
    Unique<internal::IterTypeFromIterable<Iterable>> unique(Iterable&& iterable) {
        return uniqueRange(std::begin(iterable), std::end(iterable));
    }
#endif // end has execution
    // End of group
    /**
     * @}
     */
} // end namespace lz

#endif // end LZ_UNIQUE_HPP