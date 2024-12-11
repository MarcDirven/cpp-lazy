#pragma once

#ifndef LZ_TAKE_WHILE_HPP
#define LZ_TAKE_WHILE_HPP

#include "detail/BasicIterable.hpp"
#include "detail/iterators/TakeWhileIterator.hpp"

namespace lz {
LZ_MODULE_EXPORT_SCOPE_BEGIN

template<class Iterator, class UnaryPredicate>
class TakeWhile : public detail::BasicIterable<detail::TakeWhileIterator<Iterator, UnaryPredicate>> {
public:
    using iterator = detail::TakeWhileIterator<Iterator, UnaryPredicate>;
    using const_iterator = iterator;
    using value_type = typename iterator::value_type;

    constexpr TakeWhile() = default;

    constexpr TakeWhile(Iterator begin, Iterator end, UnaryPredicate predicate) :
        detail::BasicIterable<iterator>(iterator(std::move(begin), end, predicate), iterator(end, end, predicate)) {
    }
};

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
template<LZ_CONCEPT_ITERATOR Iterator, class UnaryPredicate>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 TakeWhile<Iterator, UnaryPredicate>
takeWhileRange(Iterator begin, Iterator end, UnaryPredicate predicate) {
    return { std::move(begin), std::move(end), std::move(predicate) };
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
template<LZ_CONCEPT_ITERABLE Iterable, class UnaryPredicate>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 TakeWhile<IterT<Iterable>, UnaryPredicate>
takeWhile(Iterable&& iterable, UnaryPredicate predicate) {
    return takeWhileRange(detail::begin(std::forward<Iterable>(iterable)), detail::end(std::forward<Iterable>(iterable)),
                          std::move(predicate));
}

#ifdef LZ_HAS_EXECUTION
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
template<LZ_CONCEPT_ITERATOR Iterator, class UnaryPredicate, class Execution = std::execution::sequenced_policy>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 detail::BasicIterable<Iterator>
dropWhileRange(Iterator begin, Iterator end, UnaryPredicate predicate, Execution execution = std::execution::seq) {
    if constexpr (detail::isCompatibleForExecution<Execution, Iterator>()) {
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
template<LZ_CONCEPT_ITERABLE Iterable, class UnaryPredicate, class Execution = std::execution::sequenced_policy>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 detail::BasicIterable<IterT<Iterable>>
dropWhile(Iterable&& iterable, UnaryPredicate predicate, Execution execution = std::execution::seq) {
    return dropWhileRange(detail::begin(std::forward<Iterable>(iterable)), detail::end(std::forward<Iterable>(iterable)),
                          std::move(predicate), execution);
}
#else // ^^^ lz has execution vvv lz ! has execution

/**
 * @brief Creates a Take iterator view object.
 * @details This iterator view object can be used to skip values while `predicate` returns true. After the `predicate` returns
 * false, no more values are being skipped.
 * @param begin The beginning of the sequence.
 * @param end The ending of the sequence.
 * @param predicate Function that must return `bool`, and take a `Iterator::value_type` as function parameter.
 * @return A Take iterator view object.
 */
template<LZ_CONCEPT_ITERATOR Iterator, class UnaryPredicate>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 detail::BasicIterable<Iterator>
dropWhileRange(Iterator begin, Iterator end, UnaryPredicate predicate) {
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
template<LZ_CONCEPT_ITERABLE Iterable, class UnaryPredicate>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 detail::BasicIterable<IterT<Iterable>>
dropWhile(Iterable&& iterable, UnaryPredicate predicate) {
    return dropWhileRange(detail::begin(std::forward<Iterable>(iterable)), detail::end(std::forward<Iterable>(iterable)),
                          std::move(predicate));
}

#endif // LZ_HAS_EXECUTION
LZ_MODULE_EXPORT_SCOPE_END
} // namespace lz

#endif // LZ_TAKE_WHILE_HPP