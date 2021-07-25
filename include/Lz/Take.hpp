#pragma once

#ifndef LZ_TAKE_HPP
#    define LZ_TAKE_HPP

#    include "detail/BasicIteratorView.hpp"

namespace lz {
template<LZ_CONCEPT_ITERATOR Iterator>
class Take final : public internal::BasicIteratorView<Iterator> {
public:
    using iterator = Iterator;
    using const_iterator = Iterator;

    using value_type = internal::ValueType<Iterator>;

    LZ_CONSTEXPR_CXX_20 Take(Iterator begin, Iterator end) :
        internal::BasicIteratorView<iterator>(begin, end) {
    }

    constexpr Take() = default;
};

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
 * @brief This function takes a range between two iterators from [begin, end). Its `begin()` function returns a
 * an iterator. If MSVC and the type is an STL iterator, pass a pointer iterator, not an actual
 * iterator object.
 * @param begin The beginning of the 'view'.
 * @param end The ending of the 'view'.
 * @return A Take object that can be converted to an arbitrary container or can be iterated over using
 * `for (auto... lz::takeRange(...))`.
 */
template<LZ_CONCEPT_ITERATOR Iterator>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 Take<Iterator>
takeRange(Iterator begin, Iterator end, const internal::DiffType<Iterator> amount) {
    using lz::distance;
    using lz::next;
    using std::distance;
    using std::next;
    LZ_ASSERT(amount <= distance(begin, end), "cannot access elements after end");
    static_cast<void>(end);
    return Take<Iterator>(begin, next(begin, amount));
}

/**
 * @brief This function takes an iterable and slices `amount` from the beginning of the array. Essentially it is
 * equivalent to [`iterable.begin(), iterable.begin() + amount`). Its `begin()` function returns a random
 * access iterator.
 * @param iterable An iterable with method `begin()`.
 * @param amount The amount of elements to take from the beginning of the `iterable`.
 * @return A Take object that can be converted to an arbitrary container or can be iterated over using
 * `for (auto... lz::take(...))`.
 */
template<LZ_CONCEPT_ITERABLE Iterable, class IterType = internal::IterTypeFromIterable<Iterable>>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 Take<IterType> take(Iterable&& iterable, const internal::DiffType<IterType> amount) {
    return takeRange(internal::begin(std::forward<Iterable>(iterable)), internal::end(std::forward<Iterable>(iterable)), amount);
}

/**
 * Drops an amount of items, starting from begin.
 * @param begin The beginning of the sequence.
 * @param end The ending of the sequence.
 * @param amount The amount of items to drop, which is equivalent to next(begin, amount)
 * @return A Take iterator where the first `amount` items have been dropped.
 */
template<LZ_CONCEPT_ITERATOR Iterator>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 Take<Iterator>
dropRange(Iterator begin, Iterator end, const internal::DiffType<Iterator> amount) {
    using lz::distance;
    using lz::next;
    using std::distance;
    using std::next;
    return takeRange(next(begin, amount), end, distance(begin, end) - amount);
}

/**
 * Drops an amount of items, starting from begin.
 * @param iterable The iterable to drop from.
 * @param amount The amount of items to drop, which is equivalent to next(begin, amount)
 * @return A Take iterator where the first `amount` items have been dropped.
 */
template<LZ_CONCEPT_ITERABLE Iterable, class IterType = internal::IterTypeFromIterable<Iterable>>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 Take<IterType> drop(Iterable&& iterable, const internal::DiffType<IterType> amount) {
    return dropRange(internal::begin(std::forward<Iterable>(iterable)), internal::end(std::forward<Iterable>(iterable)), amount);
}

/**
 * @brief This function slices an iterable. It is equivalent to [`begin() + from, begin() + to`).
 * Its `begin()` function returns an iterator.
 * @param iterable An iterable with method `begin()`.
 * @param from The offset from the beginning of the iterable.
 * @param to The offset from the beginning to take. `from` must be higher than `to`.
 * @return A Take object that can be converted to an arbitrary container or can be iterated over using
 * `for (auto... lz::slice(...))`.
 */
template<LZ_CONCEPT_ITERABLE Iterable, class IterType = internal::IterTypeFromIterable<Iterable>>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 Take<internal::IterTypeFromIterable<Iterable>>
slice(Iterable&& iterable, const internal::DiffType<IterType> from, const internal::DiffType<IterType> to) {
    using lz::next;
    using std::next;
    LZ_ASSERT(to >= from, "parameter `to` cannot be more than `from`");
    auto begin = internal::begin(std::forward<Iterable>(iterable));
    begin = next(std::move(begin), from);
    return takeRange(begin, internal::end(std::forward<Iterable>(iterable)), to - from);
}

#    ifdef LZ_HAS_EXECUTION
/**
 * @brief Takes elements from an iterator from [begin, ...) while the function returns true. If the function
 * returns false, the iterator stops. Its `begin()` function returns an iterator.
 * If MSVC and the type is an STL iterator, pass a pointer iterator, not an actual iterator object.
 * @param begin The beginning of the iterator.
 * @param end The beginning of the iterator.
 * @param predicate A function that returns a bool and passes a value type in its argument. If the function returns
 * false, the iterator stops.
 * @param execution The execution policy. Must be one of `std::execution`'s tags. Performs the find using this execution.
 * @return A Take object that can be converted to an arbitrary container or can be iterated over using
 * `for (auto... lz::takeWhileRange(...))`.
 */
template<LZ_CONCEPT_ITERATOR Iterator, class Function, class Execution = std::execution::sequenced_policy>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 Take<Iterator>
takeWhileRange(Iterator begin, Iterator end, Function predicate, Execution execution = std::execution::seq) {
    if constexpr (internal::checkForwardAndPolicies<Execution, Iterator>()) {
        end = std::find_if_not(begin, end, predicate);
    }
    else {
        end = std::find_if_not(execution, begin, end, predicate);
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
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 Take<internal::IterTypeFromIterable<Iterable>>
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
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 Take<Iterator>
dropWhileRange(Iterator begin, Iterator end, Function predicate, Execution execution = std::execution::seq) {
    using lz::distance;
    using std::distance;
    using ValueType = internal::ValueType<Iterator>;
    if constexpr (internal::checkForwardAndPolicies<Execution, Iterator>()) {
        static_cast<void>(execution);
        begin = std::find_if_not(std::move(begin), end, std::move(predicate));
    }
    else {
        begin = std::find_if_not(execution, std::move(begin), end, std::move(predicate));
    }
    return takeRange(begin, end, distance(begin, end));
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
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 Take<internal::IterTypeFromIterable<Iterable>>
dropWhile(Iterable&& iterable, Function predicate, Execution execution = std::execution::seq) {
    return dropWhileRange(internal::begin(std::forward<Iterable>(iterable)), internal::end(std::forward<Iterable>(iterable)),
                          std::move(predicate), execution);
}
#    else // ^^^ lz has execution vvv lz ! has execution
/**
 * @brief Takes elements from an iterator from [begin, ...) while the function returns true. If the function
 * returns false, the iterator stops. Its `begin()` function returns an iterator.
 * If MSVC and the type is an STL iterator, pass a pointer iterator, not an actual iterator object.
 * @param begin The beginning of the iterator.
 * @param end The beginning of the iterator.
 * @param predicate A function that returns a bool and passes a value type in its argument. If the function returns
 * false, the iterator stops.
 * @return A Take object that can be converted to an arbitrary container or can be iterated over using
 * `for (auto... lz::takeWhileRange(...))`.
 */
template<LZ_CONCEPT_ITERATOR Iterator, class Function>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 Take<Iterator> takeWhileRange(Iterator begin, Iterator end, Function predicate) {
    end = std::find_if_not(begin, end, predicate);
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
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 Take<internal::IterTypeFromIterable<Iterable>>
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
Take<Iterator> dropWhileRange(Iterator begin, Iterator end, Function predicate) {
    using lz::distance;
    using std::distance;
    begin = std::find_if_not(std::move(begin), end, std::move(predicate));
    return takeRange(begin, end, distance(begin, end));
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
Take<internal::IterTypeFromIterable<Iterable>> dropWhile(Iterable&& iterable, Function predicate) {
    return dropWhileRange(internal::begin(std::forward<Iterable>(iterable)), internal::end(std::forward<Iterable>(iterable)),
                          std::move(predicate));
}

#    endif // LZ_HAS_EXECUTION
// End of group
/**
 * @}
 */
} // namespace lz

#endif