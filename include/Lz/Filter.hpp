#pragma once

#include <type_traits>
#include <algorithm>
#include <functional>
#include <vector>
#include <array>

#include <Lz/detail/BasicIteratorView.hpp>
#include <Lz/detail/FilterIterator.hpp>


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

    private:
        iterator _begin{};
        iterator _end{};

    public:
        /**
         * @brief The filter constructor.
         * @param begin Beginning of the iterator.
         * @param end End of the iterator.
         * @param function A function with parameter the value type of the iterable and must return a bool.
         */
        Filter(const Iterator begin, const Iterator end, const Function function) :
            _begin(begin, end, function),
            _end(end, end, function) {
        }

        /**
        * @brief Returns the beginning of the filter iterator object.
        * @return A forward iterator FilterIterator.
        */
        iterator begin() const override {
            return _begin;
        }

        /**
        * @brief Returns the ending of the filter iterator object.
        * @return A forward iterator FilterIterator.
        */
        iterator end() const override {
            return _end;
        }
    };

    /**
     * @addtogroup ItFns
     * @{
     */

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
    template<class Execution = std::execution::sequenced_policy, class Function, LZ_CONCEPT_ITERATOR Iterator>
    Filter<Execution, Iterator, Function>
    filterRange(const Iterator begin, const Iterator end, const Function& predicate, const Execution execution = std::execution::seq) {
        static_assert(std::is_same<internal::FunctionReturnType<Function, typename std::iterator_traits<Iterator>::value_type>, bool>::value,
                      "function must return bool");
        internal::verifyIteratorAndPolicies(execution, begin);
        return Filter<Execution, Iterator, Function>(begin, end, predicate, execution);
    }
#else
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
    Filter<Iterator, Function> filterRange(const Iterator begin, const Iterator end, const Function& predicate) {
        static_assert(std::is_same<internal::FunctionReturnType<Function, typename std::iterator_traits<Iterator>::value_type>, bool>::value,
                      "function must return bool");
        return Filter<Iterator, Function>(begin, end, predicate);
    }

#ifdef LZ_HAS_EXECUTION
    template<class Execution = std::execution::sequenced_policy, class Function, LZ_CONCEPT_ITERABLE Iterable>
    Filter<Execution, internal::IterType<Iterable>, Function>
    filter(Iterable&& iterable, const Function& predicate, const Execution execPolicy = std::execution::seq) {
        return filterRange(std::begin(iterable), std::end(iterable), predicate, execPolicy);
    }
#else
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
    Filter<internal::IterType<Iterable>, Function> filter(Iterable&& iterable, const Function& predicate) {
        return filterRange(std::begin(iterable), std::end(iterable), predicate);
    }

    // End of group
    /**
     * @}
     */
}