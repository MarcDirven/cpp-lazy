#pragma once


#include <Lz/detail/BasicIteratorView.hpp>
#include <Lz/detail/ExceptIterator.hpp>


namespace lz {
#ifdef LZ_HAS_EXECUTION
    template<class Execution, LZ_CONCEPT_ITERATOR Iterator, LZ_CONCEPT_ITERATOR IteratorToExcept>
    class Except final : public internal::BasicIteratorView<internal::ExceptIterator<Execution, Iterator, IteratorToExcept>> {
#else
    template<LZ_CONCEPT_ITERATOR Iterator, LZ_CONCEPT_ITERATOR IteratorToExcept>
    class Except final : public internal::BasicIteratorView<internal::ExceptIterator<Iterator, IteratorToExcept>> {
#endif
    public:
#ifdef LZ_HAS_EXECUTION
        using iterator = internal::ExceptIterator<Execution, Iterator, IteratorToExcept>;
#else
        using iterator = internal::ExceptIterator<Iterator, IteratorToExcept>;
#endif
        using const_iterator = iterator;

        using value_type = typename iterator::value_type;

    private:
        Iterator _begin;
        Iterator _end;
        detail::ExceptIteratorHelper<Iterator, IteratorToExcept> _iteratorHelper;

    public:
        /**
         * Except constructor. Excepts all elements between [begin, end) contained by [toExceptBegin, toExceptEnd)
         * @param begin The beginning of the iterator to skip.
         * @param end The ending of the iterator to skip.
         * @param toExceptBegin The beginning of the actual elements to except.
         * @param toExceptEnd The ending of the actual elements to except.
         */
        Except(const Iterator begin, const Iterator end, const IteratorToExcept toExceptBegin,
               const IteratorToExcept toExceptEnd) :
            _begin(begin),
            _end(end),
            _iteratorHelper{toExceptBegin, toExceptEnd} {}

        /**
         * Returns an iterator to the beginning.
         * @return An iterator to the beginning.
         */
        iterator begin() const override {
            iterator begin(_begin, _end, &_iteratorHelper);
            begin.find();
            return begin;
        }

        /**
         * Returns an iterator to the ending.
         * @return An iterator to the ending.
         */
        iterator end() const override {
            return iterator(_end, _end, &_iteratorHelper);
        }
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
      * be able to do it correctly.
      * @param begin The beginning of the iterator to except elements from contained by [`toExceptBegin`, `toExceptEnd).
      * @param end The ending of the iterator to except elements from contained by [`toExceptBegin`, `toExceptEnd).
      * @param toExceptBegin The beginning of the iterator, containing items that must be removed from [`begin`, `end`).
      * @param toExceptEnd The ending of the iterator, containing items that must be removed from [`begin`, `end`).
      * @param execPolicy The execution policy. Must be one of `std::execution`'s tags. The sorting check, the sorting and finding the
      * elements are done using this policy.
      * @return An Except view object.
      */
    template<class Execution = std::execution::sequenced_policy, LZ_CONCEPT_ITERATOR Iterator, LZ_CONCEPT_ITERATOR IteratorToExcept>
    LZ_REQUIRES_LESS_THAN(internal::ValueTypeIterator<Iterator>, internal::ValueTypeIterator<IteratorToExcept>)
        Except<Execution, Iterator, IteratorToExcept> exceptRange(const Iterator begin, const Iterator end,
            const IteratorToExcept toExceptBegin, const IteratorToExcept toExceptEnd,
            const Execution execPolicy) {
        internal::verifyIteratorAndPolicies(execPolicy, begin);
        return Except<Execution, Iterator, IteratorToExcept>(begin, end, toExceptBegin, toExceptEnd, execPolicy);
    }
#else // ^^^ has execution vvv ! has execution
     /**
      * @brief This function returns a view to the random access ExceptIterator. If elements are changed in `toExcept` after
      * calling begin(), this iterator will not be able to do it correctly.
      * @details This iterator can be used to 'remove'/'except' elements in range from [`begin`, `end`) contained by
      * [`toExceptBegin`, `toExceptEnd).
      * @param begin The beginning of the iterator to except elements from contained by [`toExceptBegin`, `toExceptEnd).
      * @param end The ending of the iterator to except elements from contained by [`toExceptBegin`, `toExceptEnd).
      * @param toExceptBegin The beginning of the iterator, containing items that must be removed from [`begin`, `end`).
      * @param toExceptEnd The ending of the iterator, containing items that must be removed from [`begin`, `end`).
      * @return An Except view object.
      */
    template<LZ_CONCEPT_ITERATOR Iterator, LZ_CONCEPT_ITERATOR IteratorToExcept>
    LZ_REQUIRES_LESS_THAN(internal::ValueTypeIterator<Iterator>, internal::ValueTypeIterator<IteratorToExcept>)
        Except<Iterator, IteratorToExcept> exceptRange(const Iterator begin, const Iterator end, const IteratorToExcept toExceptBegin,
            const IteratorToExcept toExceptEnd) {
        return Except<Iterator, IteratorToExcept>(begin, end, toExceptBegin, toExceptEnd);
    }
#endif // end has execution

#ifdef LZ_HAS_EXECUTION
    /**
     * @brief This function returns a view to the random access ExceptIterator.
     * @details This iterator can be used to 'remove'/'except' elements in range from [`begin`, `end`) contained by
     * [`toExceptBegin`, `toExceptEnd). If elements are changed in `toExcept` after calling begin(), this iterator will not
     * be able to do it correctly.
     * @param begin The beginning of the iterator to except elements from contained by [`toExceptBegin`, `toExceptEnd).
     * @param end The ending of the iterator to except elements from contained by [`toExceptBegin`, `toExceptEnd).
     * @param toExceptBegin The beginning of the iterator, containing items that must be removed from [`begin`, `end`).
     * @param toExceptEnd The ending of the iterator, containing items that must be removed from [`begin`, `end`).
     * @return An Except view object.
     */
    template<class Execution = std::execution::sequenced_policy, LZ_CONCEPT_ITERABLE Iterable, LZ_CONCEPT_ITERABLE IterableToExcept,
        class I1 = internal::IterType<Iterable>, class I2 = internal::IterType<IterableToExcept>>
        LZ_REQUIRES_LESS_THAN(internal::ValueTypeIterator<I1>, internal::ValueTypeIterator<I2>)
        Except<Execution, I1, I2> except(Iterable&& iterable, IterableToExcept&& toExcept, const Execution execPolicy = std::execution::seq) {
        return exceptRange(std::begin(iterable), std::end(iterable), std::begin(toExcept), std::end(toExcept), execPolicy);
    }

    /**
     * @brief This function returns a view to the random access ExceptIterator.
     * @details This iterator can be used to 'remove'/'except' elements in `iterable` contained by `toExcept`. If `toExcept` is sorted
     * and has an `operator<`. If elements are changed in `toExcept` after calling begin(), this iterator will not be able to do it correctly.
     * @param iterable The iterable to except elements from contained by `toExcept`.
     * @param toExcept The iterable containing items that must be removed from [`begin`, `end`).
     * @return An Except view object.
     */
    template<LZ_CONCEPT_ITERABLE Iterable, LZ_CONCEPT_ITERABLE IterableToExcept,
        class I1 = internal::IterType<Iterable>, class I2 = internal::IterType<IterableToExcept>>
        LZ_REQUIRES_LESS_THAN(internal::ValueTypeIterator<I1>, internal::ValueTypeIterator<I2>)
        Except<I1, I2> except(Iterable&& iterable, IterableToExcept&& toExcept) {
        return exceptRange(std::begin(iterable), std::end(iterable), std::begin(toExcept), std::end(toExcept));
    }
}