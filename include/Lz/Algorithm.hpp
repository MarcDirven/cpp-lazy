#pragma once

#ifndef LZ_ALGORITHM_HPP
#define LZ_ALGORITHM_HPP

#include "Lz/detail/algorithm.hpp"
#include "Lz/detail/compiler_checks.hpp"
#include "Lz/detail/concepts.hpp"
#include "Lz/detail/traits.hpp"

namespace lz {

LZ_MODULE_EXPORT_SCOPE_BEGIN

/**
 * Checks whether [begin, end) is empty.
 * @param begin The beginning of the sequence.
 * @param end The end of the sequence.
 * @return True if it is empty, false otherwise.
 */
template<LZ_CONCEPT_ITERABLE Iterable>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 bool empty(Iterable&& iterable) {
    return iterable.begin() == iterable.end();
}

/**
 * Returns whether the sequence holds exactly one element.
 * @param iterable The sequence to check whether is has exactly one element.
 * @return True if it has one element exactly, false otherwise.
 */
template<LZ_CONCEPT_ITERABLE Iterable>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 bool has_one(Iterable&& iterable) {
    auto it = std::begin(iterable);
    return !lz::empty(iterable) && ++it == iterable.end();
}

/**
 * Returns whether the sequence holds >= 2 elements.
 * @param iterable The sequence to check whether it has many (>= 2) elements.
 * @return True if the amount of values are >= 2, false otherwise.
 */
template<LZ_CONCEPT_ITERABLE Iterable>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 bool has_many(Iterable&& iterable) {
    return !lz::empty(iterable) && !has_one(iterable);
}

/**
 * @brief Gets the first element of the sequence. The sequence cannot be empty.
 *
 * @param iterable The iterable to get the first value of.
 * @return (A reference to) the first element of the sequence.
 */
template<LZ_CONCEPT_ITERABLE Iterable>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 ref<iter<Iterable>> front(Iterable&& iterable) {
    LZ_ASSERT(!lz::empty(iterable), "sequence cannot be empty in order to get the first element");
    return *std::begin(iterable);
}

/**
 * @brief Gets the end element of the sequence. The sequence cannot be empty. `iterable.begin()` must be (at least) a
 * bidirectional iterator.
 *
 * @param iterable The iterable to get the end value of.
 * @return (A reference to) the end element of the sequence.
 */
template<LZ_CONCEPT_ITERABLE Iterable>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 ref<iter<Iterable>> back(Iterable&& iterable) {
    LZ_ASSERT(!lz::empty(iterable), "sequence cannot be empty in order to get the first element");
    auto end = std::end(iterable);
    return *--end;
}

/**
 * Gets the first element of the sequence, or `default_value` if the sequence is empty.
 * @param iterable The iterable to get the first value of, or `value` in case it is empty.
 * @param default_value The value to return if `iterable` is empty.
 * @return Either the first element of `iterable` or `value` if the sequence is empty.
 */
template<LZ_CONCEPT_ITERABLE Iterable, class T>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 value_type_iterable<Iterable> front_or(Iterable&& iterable, const T& default_value) {
    return lz::empty(iterable) ? static_cast<value_type_iterable<Iterable>>(default_value) : lz::front(iterable);
}

/**
 * This function returns the end element. If the sequence is empty, it returns `value`.
 * @param iterable The iterable to get the end value of, or `value` in case it is empty.
 * @param default_value The value to return if `iterable` is empty.
 * @return Either the end element of `iterable` or `value` if the sequence is empty.
 */
template<LZ_CONCEPT_ITERABLE Iterable, class T>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 value_type_iterable<Iterable> back_or(Iterable&& iterable, const T& default_value) {
    return lz::empty(iterable) ? static_cast<value_type_iterable<Iterable>>(default_value) : lz::back(iterable);
}

/**
 * @brief Gets the distance between two iterators. This function is O(n) for forward iterators, O(1) for random access iterators.
 *
 * @param begin The beginning of the range
 * @param end The ending of the range
 * @return The distance between the two iterators
 */
template<class Iter, class S>
diff_type<Iter> distance(Iter begin, S end) {
    return detail::distance(std::move(begin), std::move(end));
}

/**
 * @brief Accumulates the values in the range [begin, end) using the binary operator @p binary_op
 *
 * @param iterable The iterable to accumulate
 * @param init The initial value to start accumulating with
 * @param binary_op The binary operator to accumulate the values with
 * @return The accumulated value
 */
template<class Iterable, class T, class BinaryOp = MAKE_BIN_OP(std::plus, value_type<Iter>)>
T accumulate(Iterable&& iterable, T init, BinaryOp binary_op = {}) {
    return detail::accumulate(std::begin(iterable), std::end(iterable), std::move(init), std::move(binary_op));
}

/**
 * @brief Gets the maximum element in the range [begin, end) using the binary binary_predicate @p binary_predicate
 *
 * @param iterable The iterable to find the maximum element in
 * @param binary_predicate The binary operator to find the maximum element with
 * @return The maximum element in the range, if the range is empty, the return value is `end`
 */
template<class Iterable, class BinaryPredicate = MAKE_BIN_OP(std::less, iter<Iterable>)>
iter<Iterable> max_element(Iterable&& iterable, BinaryPredicate binary_predicate = {}) {
    return detail::max_element(std::begin(iterable), std::end(iterable), std::move(binary_predicate));
}

/**
 * @brief Finds the minimum element in the range [begin, end) using the binary binary_predicate @p binary_predicate
 *
 * @param iterable The iterable to find the minimum element in
 * @param binary_predicate The binary operator to find the minimum element with
 * @return The minimum element in the range or `end` if the range is empty
 */
template<class Iterable, class BinaryPredicate = MAKE_BIN_OP(std::less, value_type_iterable<Iterable>)>
iter<Iterable> min_element(Iterable&& iterable, BinaryPredicate binary_predicate = {}) {
    return detail::min_element(std::begin(iterable), std::end(iterable), std::move(binary_predicate));
}

/**
 * @brief Finds the first element in the range [begin, end) that satisfies the binary_predicate @p binary_predicate
 *
 * @param iterable The iterable to find the element in
 * @param unary_predicate The binary_predicate to find the element with
 * @return Iterator to the first element that satisfies the binary_predicate or `end(iterable)` if the element is not found
 */
template<class Iterable, class UnaryPredicate>
iter<Iterable> find_if(Iterable&& iterable, UnaryPredicate unary_predicate) {
    return detail::find_if(std::begin(iterable), std::end(iterable), std::move(unary_predicate));
}

/**
 * @brief Finds the first element in the range [begin(iterable), end(iterable)) that satisfies the value @p value
 *
 * @param iterable The iterable to find the element in
 * @param value The value to find
 * @return The iterator to the first element that satisfies the value or `end(iterable)` if the element is not found
 */
template<class Iterable, class T>
iter<Iterable> find(Iterable&& iterable, const T& value) {
    return detail::find(std::begin(iterable), std::end(iterable), value);
}

/**
 * @brief Searches for the first occurrence of the range [begin(iterable2), end(iterable2)) in the range [begin(iterable),
 * end(iterable))
 *
 * @param iterable The iterable to search in
 * @param iterable2 The iterable to search for
 * @return The iterator to the first occurrence of the range [begin(iterable2), end(iterable2)) in the range [begin(iterable),
 * end(iterable)) or `end(iterable)` if the range is not found
 */
template<class Iterable, class Iterable2>
iter<Iterable> search(Iterable&& iterable, Iterable2&& iterable2) {
    return detail::search(std::begin(iterable), std::end(iterable), std::begin(iterable2), std::end(iterable2));
}

/**
 * @brief Finds the first element in the range [begin(iterable), end(iterable)) that does not satisfy the unary_predicate @p
 * unary_predicate
 *
 * @param iterable The iterable to find the element in
 * @param unary_predicate The unary_predicate to find the element with
 * @return Iterator to the first element that does not satisfy the unary_predicate or `end(iterable)` if the element is not found
 */
template<class Iterable, class UnaryPredicate>
iter<Iterable> find_if_not(Iterable&& iterable, UnaryPredicate unary_predicate) {
    return detail::find_if_not(std::begin(iterable), std::end(iterable), std::move(unary_predicate));
}

/**
 * @brief Finds the first element in the range [begin(iterable), end(iterable)) that satisfies the value @p toFind. If the element
 * is found, it returns the value, otherwise it returns @p default_value
 * @param iterable The iterable to find the element in
 * @param toFind The value to find
 * @param default_value The value to return if the element is not found
 * @return The value @p toFind if it is found, otherwise @p default_value
 */
template<class Iterable, class T, class U>
value_type_iterable<Iterable> find_or_default(Iterable&& iterable, T&& toFind, U&& default_value) {
    return detail::find_or_default(std::begin(iterable), std::end(iterable), std::forward<T>(toFind),
                                   std::forward<U>(default_value));
}

/**
 * @brief Finds the first element in the range [begin(iterable), end(iterable)) that satisfies the unary_predicate @p
 * unary_predicate. If the element is found, it returns the value, otherwise it returns @p default_value
 * @param iterable The iterable to search.
 * @param unary_predicate The search unary_predicate in [begin(iterable), end(iterable)). Must return bool.
 * @param default_value The value to return when no element matches unary_predicate @p unary_predicate in [begin(iterable),
 * end(iterable))
 * @return The value if it is found, otherwise @p default_value
 */
template<class Iterable, class UnaryPredicate, class U>
value_type_iterable<Iterable> find_or_default_if(Iterable&& iterable, UnaryPredicate unary_predicate, U&& default_value) {
    return detail::find_or_default_if(std::begin(iterable), std::end(iterable), std::move(binary_predicate),
                                      std::forward<U>(default_value));
}

/**
 * @brief Reorders the elements in the range [begin(iterable), end(iterable)) in such a way that all elements for which the
 * unary_predicate @p unary_predicate returns true precede all elements for which unary_predicate @p unary_predicate returns
 * false. Relative order of the elements is not preserved
 *
 * @param iterable The iterable to partition
 * @param unary_predicate The unary_predicate to partition the elements with
 * @return Iterator to the first element of the second group.
 */
template<class Iterable, class UnaryPredicate>
iter<Iterable> partition(Iterable&& iterable, UnaryPredicate unary_predicate) {
    return detail::partition(std::begin(iterable), std::end(iterable), std::move(unary_predicate));
}

/**
 * Gets the mean of a sequence.
 * @param iterable The iterable to calculate the mean of.
 * @param binary_op The binary operator to calculate the mean with.
 * @return The mean of the container.
 */
template<class Iterable, class BinaryOp = MAKE_BIN_OP(std::plus, value_type_iterable<Iterable>)>
double mean(Iterable&& iterable, BinaryOp binary_op = {}) {
    return detail::mean(std::begin(iterable), std::end(iterable), std::move(binary_op));
}

/**
 * @brief Iterates over the range [begin(iterable), end(iterable)) and applies the unary binary_predicate @p unary_op to
 * each
 *
 * @param iterable The iterable to iterate over
 * @param unary_op The unary binary_predicate to apply to each element
 */
template<class Iterable, class UnaryOp>
void for_each(Iterable&& iterable, UnaryOp unary_op) {
    detail::for_each(std::begin(iterable), std::end(iterable), std::move(unary_op));
}

/**
 * @brief Keeps iterating over @p iterable while @p binary_predicate returns true. Essentially the same as lz::filter, however
 * filter is not generally used to alter variables in a range
 *
 * @param iterable A range
 * @param unary_op Predicate that must either return true or false
 */
template<LZ_CONCEPT_ITERABLE Iterable, class UnaryOp>
void for_each_while(Iterable&& iterable, UnaryOp unary_op) {
    static_assert(std::is_convertible<detail::decay<decltype(unary_op(*iterable.begin()))>, bool>::value,
                  "Predicate must return boolean like value");
    detail::for_each_while(detail::begin(std::forward<Iterable>(iterable)), detail::end(std::forward<Iterable>(iterable)),
                           std::move(unary_op));
}

/**
 * @brief Copies the range [begin(iterable), end(iterable)) to the range [output, output + (end - begin))
 *
 * @param iterable The iterable to copy
 * @param output The output iterator to copy the values to
 */
template<class Iterable, class OutputIterator>
void copy(Iterable&& iterable, OutputIterator output) {
    detail::copy(std::begin(iterable), std::end(iterable), std::move(output));
}

/**
 * @brief Transforms the range [begin(iterable), end(iterable)) to the range [output, output + (end - begin))
 *
 * @param iterable The iterable to transform
 * @param output The output iterator to transform the values to
 * @param unary_op The operation to transform the values with
 */
template<class Iterable, class OutputIterator, class UnaryOp>
void transform(Iterable&& iterable, OutputIterator output, UnaryOp unary_op) {
    detail::transform(std::begin(iterable), std::end(iterable), std::move(output), std::move(unary_op));
}

/**
 * Use this function to check if two lz iterators are the same.
 * @param a An iterable, its underlying value type should have an operator== with `b`
 * @param b An iterable, its underlying value type should have an operator== with `a`
 * @return true if both are equal, false otherwise.
 */
template<class IterableA, class IterableB, class BinaryPredicate = MAKE_BIN_OP(std::equal_to, value_type_iterable<IterableA>)>
bool equal(const IterableA& a, const IterableB& b, BinaryPredicate binary_predicate = {}) {
    return detail::equal(std::begin(a), std::end(a), std::begin(b), std::end(b), std::move(binary_predicate));
}

/**
 * @brief Checks if the range [begin(iterable), end(iterable)) contains the value @p value
 *
 * @param iterable The iterable to check
 * @param value The value to check for
 * @return `true` if the value is found, `false` otherwise
 */
template<class Iterable, class T>
bool contains(Iterable&& iterable, const T& value) {
    return detail::find(std::begin(iterable), std::end(iterable), value) != std::end(iterable);
}

/**
 * @brief Checks if the range [begin(iterable), end(iterable)) contains an element that satisfies the unary_predicate @p
 * unary_predicate
 *
 * @param iterable The iterable to check
 * @param unary_predicate The unary_predicate to check for
 * @return `true` if the unary_predicate is satisfied, `false` otherwise
 */
template<class Iterable, class UnaryPredicate>
bool contains_if(Iterable&& iterable, UnaryPredicate unary_predicate) {
    return detail::find_if(std::begin(iterable), std::end(iterable), std::move(unary_predicate)) != std::end(iterable);
}

/**
 * @brief Searches for the first element in the partitioned range [begin(iterable), end(iterable)) which is not ordered before
 * @p value.
 *
 * @param iterable The iterable to check
 * @param value The value to check for
 * @param binary_predicate The to use when comparing the values
 * @return Iteartor to the first element that satisfies the value or `end(iterable)` if the element is not found
 */
template<class Iterable, class T, class BinaryPredicate = MAKE_BIN_OP(std::equal_to, value_type_iterable<Iterable>)>
iter<Iterable> lower_bound(Iterable&& iterable, const T& value, BinaryPredicate binary_predicate) {
    return detail::lower_bound(std::begin(iterable), std::end(iterable), value, std::move(binary_predicate));
}

/**
 * @brief Searches for the first occurrence of the value @p value in the range [begin(iterable), end(iterable)) using the binary
 * search algorithm. @p iterable must be sorted beforehand.
 * @param iterable The iterable to search in
 * @param value The value to search for
 * @param binary_predicate Predicate to search the value with
 * @return true if the value is found, false otherwise
 */
template<class Iterable, class T, class BinaryPredicate = MAKE_BIN_OP(std::equal_to, value_type_iterable<Iterable>)>
bool binary_search(Iterable&& iterable, const T& value, BinaryPredicate binary_predicate) {
    return detail::binary_search(std::begin(iterable), std::end(iterable), value, std::move(binary_predicate));
}

/**
 * @brief Checks if all elements in the range [begin(iterable), end(iterable)) satisfy the unary_predicate @p unary_predicate
 *
 * @param iterable The iterable to check
 * @param unary_predicate The unary_predicate to call whether all elements satisfy this condition
 * @return true if all elements satisfy the unary_predicate, false otherwise
 */
template<class Iterable, class UnaryPredicate>
bool all_of(Iterable&& iterable, UnaryPredicate unary_predicate) {
    return detail::all_of(std::begin(iterable), std::end(iterable), std::move(unary_predicate));
}

/**
 * @brief Checks if any element in the range [begin(iterable), end(iterable)) satisfies the unary_predicate @p unary_predicate
 *
 * @param iterable The iterable to check
 * @param unary_predicate The unary_predicate to call whether any element satisfies this condition
 * @return true if any element satisfies the unary_predicate, false otherwise
 */
template<class Iterable, class UnaryPredicate>
bool any_of(Iterable&& iterable, UnaryPredicate unary_predicate) {
    return detail::any_of(std::begin(iterable), std::end(iterable), std::move(unary_predicate));
}

/**
 * @brief Checks if none of the elements in the range [begin(iterable), end(iterable)) satisfy the unary_predicate @p
 * unary_predicate
 *
 * @param iterable The iterable to check
 * @param unary_predicate The unary_predicate to call whether none of the elements satisfy this condition
 * @return true if none of the elements satisfy the unary_predicate, false otherwise
 */
template<class Iterable, class UnaryPredicate>
bool none_of(Iterable&& iterable, UnaryPredicate unary_predicate) {
    return detail::none_of(std::begin(iterable), std::end(iterable), std::move(unary_predicate));
}

/**
 * @brief Finds the first occurrence of two adjacent elements in the range [begin(iterable), end(iterable)) that satisfy the
 * binary binary_predicate @p binary_predicate
 *
 * @param iterable The iterable to search in
 * @param binary_predicate The binary binary_predicate to search with
 * @return Iterator to the first occurrence of two adjacent elements that satisfy the binary binary_predicate or `end(iterable)`
 * if the elements are not found
 */
template<class Iterable, class BinaryPredicate>
iter<Iterable> adjacent_find(Iterable&& iterable, BinaryPredicate binary_predicate) {
    return detail::adjacent_find(std::begin(iterable), std::end(iterable), std::move(binary_predicate));
}

LZ_MODULE_EXPORT_SCOPE_END
} // namespace lz

#endif