#pragma once


#include "detail/BasicIteratorView.hpp"
#include "detail/ExceptIterator.hpp"


namespace lz {
    template<class Iterator, class IteratorToExcept>
    class Except final : public detail::BasicIteratorView<detail::ExceptIterator<Iterator, IteratorToExcept>> {
    public:
        using iterator = detail::ExceptIterator<Iterator, IteratorToExcept>;
        using const_iterator = iterator;
        using value_type = typename iterator::value_type;

    private:
        mutable detail::ExceptIteratorHelper<Iterator, IteratorToExcept> _iteratorHelper;
        Iterator _begin{};
        Iterator _end{};

    public:
        /**
         * Except constructor. Excepts all elements between [begin, end) contained by [toExceptBegin, toExceptEnd)
         * @param begin The beginning of the iterator to skip.
         * @param end The ending of the iterator to skip.
         * @param toExceptBegin The beginning of the actual elements to except.
         * @param toExceptEnd The ending of the actual elements to except.
         */
        Except(const Iterator begin, const Iterator end, const IteratorToExcept toExceptBegin, const IteratorToExcept toExceptEnd) :
            _iteratorHelper{toExceptBegin, toExceptEnd, end, false},
            _begin(begin),
            _end(end) {}

        Except() = default;

        /**
         * Returns an iterator to the beginning.
         * @return An iterator to the beginning.
         */
        iterator begin() const override {
            _iteratorHelper.isSorted = std::is_sorted(_iteratorHelper.toExceptBegin, _iteratorHelper.toExceptEnd);
            return iterator(_begin, _end, &_iteratorHelper);
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

    /**
     * @brief This function returns a view to the random access ExceptIterator.
     * @details This iterator can be used to 'remove'/'except' elements in range from [`begin`, `end`) contained by
     * [`toExceptBegin`, `toExceptEnd).
     * @tparam Iterator Is automatically deduced.
     * @tparam IteratorToExcept Is automatically deduced.
     * @param begin The beginning of the iterator to except elements from contained by [`toExceptBegin`, `toExceptEnd).
     * @param end The ending of the iterator to except elements from contained by [`toExceptBegin`, `toExceptEnd).
     * @param toExceptBegin The beginning of the iterator, containing items that must be removed from [`begin`, `end`).
     * @param toExceptEnd The ending of the iterator, containing items that must be removed from [`begin`, `end`).
     * @return An Except view object.
     */
    template<class Iterator, class IteratorToExcept>
    Except<Iterator, IteratorToExcept> exceptrange(const Iterator begin, const Iterator end, const IteratorToExcept toExceptBegin,
                                                   const IteratorToExcept toExceptEnd) {
        return Except<Iterator, IteratorToExcept>(begin, end, toExceptBegin, toExceptEnd);
    }

    /**
     * @brief This function returns a view to the random access ExceptIterator.
     * @details This iterator can be used to 'remove'/'except' elements in `iterable` contained by `toExcept`. If `toExcept` is sorted
     * and has an `operator<`
     * @tparam Iterable Is automatically deduced.
     * @tparam IterableToExcept Is automatically deduced.
     * @param iterable The iterable to except elements from contained by `toExcept`.
     * @param toExcept The iterable containing items that must be removed from [`begin`, `end`).
     * @return An Except view object.
     */
    template<class Iterable, class IterableToExcept>
    auto except(Iterable&& iterable, IterableToExcept&& toExcept) ->
    Except<decltype(std::begin(iterable)), decltype(std::begin(toExcept))> {
        return exceptrange(std::begin(iterable), std::end(iterable), std::begin(toExcept), std::end(toExcept));
    }

    // End of group
    /**
     * @}
     */
}