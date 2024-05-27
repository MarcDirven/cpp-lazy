#pragma once

#ifndef LZ_TAKE_HPP
#define LZ_TAKE_HPP

#include "detail/BasicIteratorView.hpp"

namespace lz {

LZ_MODULE_EXPORT_SCOPE_BEGIN

// Start of group
/**
 * @defgroup ItFns Iterator free functions.
 * These are the iterator functions and can all be used to iterate over in a
 * `for (auto var : lz::someiterator(...))`-like fashion. Also, all objects contain a `toVector`,
 * `toVector<Allocator>`, `toArray<N>`, `to<container>. toMap, toUnorderedMap` (specifying its value type of the container is not
 *  necessary, so e.g. `to<std::list>()` will do), `begin()`, `end()` methods and `value_type` and `iterator`
 *  typedefs.
 * @{
 */

/**
 * @brief Returns a view to another view or container. Can be handy to slice, skip a few first or last elements using:
 * `auto range = lz::view(vec.begin() + 4, vec.end());`
 * @param begin The beginning of the 'view'.
 * @param end The ending of the 'view'.
 * @return A View object that can be converted to an arbitrary container or can be iterated over using
 * `for (auto... lz::takeRange(...))`.
 */
template<LZ_CONCEPT_ITERATOR Iterator>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 internal::BasicIteratorView<Iterator> view(Iterator begin, Iterator end) {
    return { begin, end };
}

#ifdef LZ_HAS_EXECUTION
/**
 * @brief Takes elements from an iterator from [begin, ...) while the function returns true. If the function
 * returns false, the iterator stops. Its `begin()` function returns an iterator.
 * @param begin The beginning of the iterator.
 * @param end The beginning of the iterator.
 * @param predicate A function that returns a bool and passes a value type in its argument. If the function returns
 * false, the iterator stops.
 * @param execution The execution policy. Must be one of `std::execution`'s tags. Performs the find using this execution.
 * @return A Take object that can be converted to an arbitrary container or can be iterated over using
 * `for (auto... lz::takeWhileRange(...))`.
 */
template<LZ_CONCEPT_ITERATOR Iterator, class Function, class Execution = std::execution::sequenced_policy>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 internal::BasicIteratorView<Iterator>
takeWhileRange(Iterator begin, Iterator end, Function predicate, Execution execution = std::execution::seq) {
    if constexpr (internal::isCompatibleForExecution<Execution, Iterator>()) {
        end = std::find_if_not(begin, end, std::move(predicate));
    }
    else {
        end = std::find_if_not(execution, begin, end, std::move(predicate));
    }
    return { std::move(begin), std::move(end) };
}

/**
 * @brief This function does the same as `lz::takeWhileRange` except that it takes an iterable as parameter.
 * Its `begin()` function returns an iterator.
 * @param iterable An object that has methods `begin()` and `end()`.
 * @param predicate A function that returns a bool and passes a value type in its argument. If the function returns
 * false, the iterator stops.
 * @param execution The execution policy. Must be one of `std::execution`'s tags. Performs the find using this execution.
 * @return A Take object that can be converted to an arbitrary container or can be iterated over using
 * `for (auto... lz::takeWhile(...))`.
 */
template<LZ_CONCEPT_ITERABLE Iterable, class Function, class Execution = std::execution::sequenced_policy>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 internal::BasicIteratorView<internal::IterTypeFromIterable<Iterable>>
takeWhile(Iterable&& iterable, Function predicate, Execution execution = std::execution::seq) {
    return takeWhileRange(internal::begin(std::forward<Iterable>(iterable)), internal::end(std::forward<Iterable>(iterable)),
                          std::move(predicate), execution);
}

/**
 * @brief Creates a Take iterator view object.
 * @details This iterator view object can be used to skip values while `predicate` returns true. After the `predicate` returns
 * false, no more values are being skipped.
 * @param begin The beginning of the sequence.
 * @param end The ending of the sequence.
 * @param predicate Function that must return `bool`, and take a `Iterator::value_type` as function parameter.
 * @param execution The execution policy. Must be one of std::execution::*
 * @return A Take iterator view object.
 */
template<LZ_CONCEPT_ITERATOR Iterator, class Function, class Execution = std::execution::sequenced_policy>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 internal::BasicIteratorView<Iterator>
dropWhileRange(Iterator begin, Iterator end, Function predicate, Execution execution = std::execution::seq) {
    if constexpr (internal::isCompatibleForExecution<Execution, Iterator>()) {
        static_cast<void>(execution);
        begin = std::find_if_not(std::move(begin), end, std::move(predicate));
    }
    else {
        begin = std::find_if_not(execution, std::move(begin), end, std::move(predicate));
    }
    return { std::move(begin), std::move(end) };
}

/**
 * @brief Creates a Take iterator view object.
 * @details This iterator view object can be used to skip values while `predicate` returns true. After the `predicate` returns
 * false, no more values are being skipped.
 * @param iterable The sequence with the values that can be iterated over.
 * @param predicate Function that must return `bool`, and take a `Iterator::value_type` as function parameter.
 * @param execution The execution policy. Must be one of std::execution::*
 * @return A Take iterator view object.
 */
template<LZ_CONCEPT_ITERABLE Iterable, class Function, class Execution = std::execution::sequenced_policy>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 internal::BasicIteratorView<internal::IterTypeFromIterable<Iterable>>
dropWhile(Iterable&& iterable, Function predicate, Execution execution = std::execution::seq) {
    return dropWhileRange(internal::begin(std::forward<Iterable>(iterable)), internal::end(std::forward<Iterable>(iterable)),
                          std::move(predicate), execution);
}
#else // ^^^ lz has execution vvv lz ! has execution
/**
 * @brief Takes elements from an iterator from [begin, ...) while the function returns true. If the function
 * returns false, the iterator stops. Its `begin()` function returns an iterator.
 * @param begin The beginning of the iterator.
 * @param end The beginning of the iterator.
 * @param predicate A function that returns a bool and passes a value type in its argument. If the function returns
 * false, the iterator stops.
 * @return A Take object that can be converted to an arbitrary container or can be iterated over using
 * `for (auto... lz::takeWhileRange(...))`.
 */
template<LZ_CONCEPT_ITERATOR Iterator, class Function>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 internal::BasicIteratorView<Iterator>
takeWhileRange(Iterator begin, Iterator end, Function predicate) {
    end = std::find_if_not(begin, end, std::move(predicate));
    return { std::move(begin), std::move(end) };
}

/**
 * @brief This function does the same as `lz::takeWhileRange` except that it takes an iterable as parameter.
 * Its `begin()` function returns an iterator.
 * @param iterable An object that has methods `begin()` and `end()`.
 * @param predicate A function that returns a bool and passes a value type in its argument. If the function returns
 * false, the iterator stops.
 * @return A Take object that can be converted to an arbitrary container or can be iterated over using
 * `for (auto... lz::takeWhile(...))`.
 */
template<LZ_CONCEPT_ITERABLE Iterable, class Function>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 internal::BasicIteratorView<internal::IterTypeFromIterable<Iterable>>
takeWhile(Iterable&& iterable, Function predicate) {
    return takeWhileRange(internal::begin(std::forward<Iterable>(iterable)), internal::end(std::forward<Iterable>(iterable)),
                          std::move(predicate));
}

/**
 * @brief Creates a Take iterator view object.
 * @details This iterator view object can be used to skip values while `predicate` returns true. After the `predicate` returns
 * false, no more values are being skipped.
 * @param begin The beginning of the sequence.
 * @param end The ending of the sequence.
 * @param predicate Function that must return `bool`, and take a `Iterator::value_type` as function parameter.
 * @return A Take iterator view object.
 */
template<LZ_CONCEPT_ITERATOR Iterator, class Function>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 internal::BasicIteratorView<Iterator>
dropWhileRange(Iterator begin, Iterator end, Function predicate) {
    begin = std::find_if_not(std::move(begin), end, std::move(predicate));
    return { std::move(begin), std::move(end) };
}

/**
 * @brief Creates a Take iterator view object.
 * @details This iterator view object can be used to skip values while `predicate` returns true. After the `predicate` returns
 * false, no more values are being skipped.
 * @param iterable The sequence with the values that can be iterated over.
 * @param predicate Function that must return `bool`, and take a `Iterator::value_type` as function parameter.
 * @return A Take iterator view object.
 */
template<LZ_CONCEPT_ITERABLE Iterable, class Function>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 internal::BasicIteratorView<internal::IterTypeFromIterable<Iterable>>
dropWhile(Iterable&& iterable, Function predicate) {
    return dropWhileRange(internal::begin(std::forward<Iterable>(iterable)), internal::end(std::forward<Iterable>(iterable)),
                          std::move(predicate));
}

#endif // LZ_HAS_EXECUTION
// End of group
/**
 * @}
 */

LZ_MODULE_EXPORT_SCOPE_END

} // namespace lz

#endif