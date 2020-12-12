#pragma once

#ifndef LZ_FILTER_HPP
#define LZ_FILTER_HPP

#include "detail/BasicIteratorView.hpp"
#include "detail/FilterIterator.hpp"


namespace lz {
#ifdef LZ_HAS_EXECUTION
    template<class Execution, LZ_CONCEPT_ITERATOR Iterator, class Function>
    class Filter final : public internal::BasicIteratorView<internal::FilterIterator<Execution, Iterator, Function>> {
#else
    template<LZ_CONCEPT_ITERATOR Iterator, class Function>
    class Filter final : public internal::BasicIteratorView<internal::FilterIterator<Iterator, Function>> {
#endif
    public:
#ifdef LZ_HAS_EXECUTION
        using iterator = internal::FilterIterator<Execution, Iterator, Function>;
#else
        using iterator = internal::FilterIterator<Iterator, Function>;
#endif
        using const_iterator = iterator;
        using value_type = typename iterator::value_type;

        /**
         * @brief The filter constructor.
         * @param begin Beginning of the iterator.
         * @param end End of the iterator.
         * @param execution The execution policy.
         * @param function A function with parameter the value type of the iterable and must return a bool.
         */
#ifdef LZ_HAS_EXECUTION
        Filter(Iterator begin, Iterator end, Function function, const Execution execution) :
            internal::BasicIteratorView<iterator>(iterator(begin, end, function, execution),
                                                  iterator(end, end, function, execution))
        {
        }
#else
        /**
         * @brief The filter constructor.
         * @param begin Beginning of the iterator.
         * @param end End of the iterator.
         * @param function A function with parameter the value type of the iterable and must return a bool.
         */
        Filter(Iterator begin, Iterator end, Function function) :
            internal::BasicIteratorView<iterator>(iterator(begin, end, function), iterator(end, end, function))
        {
        }
#endif

        Filter() = default;
    };

    /**
     * @addtogroup ItFns
     * @{
     */

#ifdef LZ_HAS_EXECUTION
    /**
     * @brief Returns a forward filter iterator. If the `predicate` returns false, it is excluded.
     * @details I.e. `lz::filter({1, 2, 3, 4, 5}, [](int i){ return i % 2 == 0; });` will eventually remove all
     * elements that are not even.
     * @param begin The beginning of the range.
     * @param end The ending of the range.
     * @param predicate A function that must return a bool, and needs a value type of the container as parameter.
     * @param execution The execution policy. Must be one of `std::execution`'s tags. Performs the find using this execution.
     * @return A filter object from [begin, end) that can be converted to an arbitrary container or can be iterated
     * over.
     */
    template<class Execution = std::execution::sequenced_policy, class Function, LZ_CONCEPT_ITERATOR Iterator>
    Filter<Execution, Iterator, Function>
    filterRange(Iterator begin, Iterator end, Function predicate, const Execution execution = std::execution::seq) {
        static_assert(std::is_same<internal::FunctionReturnType<Function, internal::ValueType<Iterator>>, bool>::value,
                      "function must return bool");
        static_cast<void>(internal::checkForwardAndPolicies<Execution, Iterator>());
        return Filter<Execution, Iterator, Function>(std::move(begin), std::move(end), std::move(predicate), execution);
    }

    template<class Execution = std::execution::sequenced_policy, class Function, LZ_CONCEPT_ITERABLE Iterable>
    Filter<Execution, internal::IterTypeFromIterable<Iterable>, Function>
    filter(Iterable&& iterable, Function predicate, const Execution execPolicy = std::execution::seq) {
        return filterRange(internal::begin(std::forward<Iterable>(iterable)), internal::end(std::forward<Iterable>(iterable)),
                           std::move(predicate), execPolicy);
    }

#else // ^^^ has execution vvv ! has execution
    /**
     * @brief Returns a forward filter iterator. If the `predicate` returns false, it is excluded.
     * @details I.e. `lz::filter({1, 2, 3, 4, 5}, [](int i){ return i % 2 == 0; });` will eventually remove all
     * elements that are not even.
     * @param begin The beginning of the range.
     * @param end The ending of the range.
     * @param predicate A function that must return a bool, and needs a value type of the container as parameter.
     * @return A filter object from [begin, end) that can be converted to an arbitrary container or can be iterated
     * over.
     */
    template<class Function, LZ_CONCEPT_ITERATOR Iterator>
    Filter<Iterator, Function> filterRange(Iterator begin, Iterator end, Function predicate) {
        static_assert(std::is_convertible<internal::FunctionReturnType<Function, internal::ValueType<Iterator>>, bool>::value,
                      "function return type must be convertible to a bool");
        return Filter<Iterator, Function>(std::move(begin), std::move(end), std::move(predicate));
    }

    /**
     * @brief Returns a forward filter iterator. If the `predicate` returns false, the value it is excluded.
     * @details I.e. `lz::filter({1, 2, 3, 4, 5}, [](int i){ return i % 2 == 0; });` will eventually remove all
     * elements that are not even.
     * @param iterable An iterable, e.g. a container / object with `begin()` and `end()` methods.
     * @param predicate A function that must return a bool, and needs a value type of the container as parameter.
     * @return A filter iterator that can be converted to an arbitrary container or can be iterated
     * over using `for (auto... lz::filter(...))`.
     */
    template<class Function, LZ_CONCEPT_ITERABLE Iterable>
    Filter<internal::IterTypeFromIterable<Iterable>, Function> filter(Iterable&& iterable, Function predicate) {
        return filterRange(internal::begin(std::forward<Iterable>(iterable)), internal::end(std::forward<Iterable>(iterable)),
                           std::move(predicate));
    }
#endif // end lz has execution
    // End of group
    /**
     * @}
     */
}

#endif // end LZ_FILTER_HPP