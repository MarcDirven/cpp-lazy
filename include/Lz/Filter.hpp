#pragma once

#ifndef LZ_FILTER_HPP
#define LZ_FILTER_HPP

#include "detail/BasicIteratorView.hpp"
#include "detail/FilterIterator.hpp"

namespace lz {

LZ_MODULE_EXPORT_SCOPE_BEGIN

#ifdef LZ_HAS_EXECUTION

template<LZ_CONCEPT_ITERATOR Iterator, class UnaryPredicate, class Execution>
class Filter final : public internal::BasicIteratorView<internal::FilterIterator<Iterator, UnaryPredicate, Execution>> {
#else
template<LZ_CONCEPT_ITERATOR Iterator, class UnaryPredicate>
class Filter final : public internal::BasicIteratorView<internal::FilterIterator<Iterator, UnaryPredicate>> {
#endif
public:
#ifdef LZ_HAS_EXECUTION
    using iterator = internal::FilterIterator<Iterator, UnaryPredicate, Execution>;
#else
    using iterator = internal::FilterIterator<Iterator, UnaryPredicate>;
#endif
    using const_iterator = iterator;
    using value_type = typename iterator::value_type;

#ifdef LZ_HAS_EXECUTION
    LZ_CONSTEXPR_CXX_20 Filter(Iterator begin, Iterator end, UnaryPredicate function, Execution execution) :
        internal::BasicIteratorView<iterator>(iterator(begin, begin, end, function, execution),
                                              iterator(end, begin, end, function, execution)) {
    }
#else
    Filter(Iterator begin, Iterator end, UnaryPredicate function) :
        internal::BasicIteratorView<iterator>(iterator(begin, begin, end, function), iterator(end, begin, end, function)) {
    }
#endif

    constexpr Filter() = default;
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
template<LZ_CONCEPT_ITERATOR Iterator, class UnaryPredicate, class Execution = std::execution::sequenced_policy>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 Filter<Iterator, UnaryPredicate, Execution>
filterRange(Iterator begin, Iterator end, UnaryPredicate predicate, Execution execution = std::execution::seq) {
    static_assert(std::is_convertible<decltype(predicate(*begin)), bool>::value,
                  "function must return type that can be converted to bool");
    static_cast<void>(internal::isCompatibleForExecution<Execution, Iterator>());
    return { std::move(begin), std::move(end), std::move(predicate), execution };
}

template<LZ_CONCEPT_ITERABLE Iterable, class UnaryPredicate, class Execution = std::execution::sequenced_policy>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 Filter<internal::IterTypeFromIterable<Iterable>, UnaryPredicate, Execution>
filter(Iterable&& iterable, UnaryPredicate predicate, Execution execPolicy = std::execution::seq) {
    return filterRange(internal::begin(std::forward<Iterable>(iterable)), internal::end(std::forward<Iterable>(iterable)),
                       std::move(predicate), execPolicy);
}

#else  // ^^^ has execution vvv ! has execution
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
template<class Iterator, class UnaryPredicate>
Filter<Iterator, UnaryPredicate> filterRange(Iterator begin, Iterator end, UnaryPredicate predicate) {
    static_assert(std::is_convertible<decltype(predicate(*begin)), bool>::value,
                  "function return type must be convertible to a bool");
    return { std::move(begin), std::move(end), std::move(predicate) };
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
template<class Iterable, class UnaryPredicate>
Filter<internal::IterTypeFromIterable<Iterable>, UnaryPredicate> filter(Iterable&& iterable, UnaryPredicate predicate) {
    return filterRange(internal::begin(std::forward<Iterable>(iterable)), internal::end(std::forward<Iterable>(iterable)),
                       std::move(predicate));
}
#endif // LZ_HAS_EXECUTION
// End of group
/**
 * @}
 */

LZ_MODULE_EXPORT_SCOPE_END

} // namespace lz

#endif // end LZ_FILTER_HPP