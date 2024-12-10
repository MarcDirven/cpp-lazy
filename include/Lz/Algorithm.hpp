#ifndef LZ_ALGORITHM_HPP
#define LZ_ALGORITHM_HPP

#include "Lz/detail/Algorithm.hpp"
#include "Lz/detail/CompilerChecks.hpp"
#include "Lz/detail/Concepts.hpp"
#include "Lz/detail/Traits.hpp"

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
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 bool hasOne(Iterable&& iterable) {
    auto it = std::begin(iterable);
    return !lz::empty(iterable) && ++it == iterable.end();
}

/**
 * Returns whether the sequence holds >= 2 elements.
 * @param iterable The sequence to check whether it has many (>= 2) elements.
 * @return True if the amount of values are >= 2, false otherwise.
 */
template<LZ_CONCEPT_ITERABLE Iterable>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 bool hasMany(Iterable&& iterable) {
    return !lz::empty(iterable) && !hasOne(iterable);
}

/**
 * @brief Gets the first element of the sequence. The sequence cannot be empty.
 *
 * @param iterable The iterable to get the first value of.
 * @return (A reference to) the first element of the sequence.
 */
template<LZ_CONCEPT_ITERABLE Iterable>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 RefType<IterT<Iterable>> front(Iterable&& iterable) {
    LZ_ASSERT(!lz::empty(iterable), "sequence cannot be empty in order to get the first element");
    return *std::begin(iterable);
}

/**
 * @brief Gets the last element of the sequence. The sequence cannot be empty. `iterable.begin()` must be (at least) a
 * bidirectional iterator.
 *
 * @param iterable The iterable to get the last value of.
 * @return (A reference to) the last element of the sequence.
 */
template<LZ_CONCEPT_ITERABLE Iterable>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 RefType<IterT<Iterable>> back(Iterable&& iterable) {
    LZ_ASSERT(!lz::empty(iterable), "sequence cannot be empty in order to get the first element");
    auto end = std::end(iterable);
    return *--end;
}

/**
 * Gets the first element of the sequence, or `defaultValue` if the sequence is empty.
 * @param iterable The iterable to get the first value of, or `value` in case it is empty.
 * @param defaultValue The value to return if `iterable` is empty.
 * @return Either the first element of `iterable` or `value` if the sequence is empty.
 */
template<LZ_CONCEPT_ITERABLE Iterable, class T>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 ValueTypeIterable<Iterable> frontOr(Iterable&& iterable, const T& defaultValue) {
    return lz::empty(iterable) ? static_cast<ValueTypeIterable<Iterable>>(defaultValue) : lz::front(iterable);
}

/**
 * This function returns the last element. If the sequence is empty, it returns `value`.
 * @param iterable The iterable to get the last value of, or `value` in case it is empty.
 * @param defaultValue The value to return if `iterable` is empty.
 * @return Either the last element of `iterable` or `value` if the sequence is empty.
 */
template<LZ_CONCEPT_ITERABLE Iterable, class T>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 ValueTypeIterable<Iterable> backOr(Iterable&& iterable, const T& defaultValue) {
    return lz::empty(iterable) ? static_cast<ValueTypeIterable<Iterable>>(defaultValue) : lz::back(iterable);
}

/**
 * @brief Gets the distance between two iterators. This function is O(n) for forward iterators, O(1) for random access iterators.
 *
 * @param first The beginning of the range
 * @param last The ending of the range
 * @return The distance between the two iterators
 */
template<class Iter, class S>
DiffType<Iter> distance(Iter first, S last) {
    return detail::distance(std::move(first), std::move(last));
}

/**
 * @brief Accumulates the values in the range [begin, end) using the binary operator @p binaryOp
 *
 * @param iterable The iterable to accumulate
 * @param init The initial value to start accumulating with
 * @param binaryOp The binary operator to accumulate the values with
 * @return The accumulated value
 */
template<class Iterable, class T, class BinaryOp = MAKE_BIN_OP(std::plus, ValueType<Iter>)>
T accumulate(Iterable&& iterable, T init, BinaryOp binaryOp = {}) {
    return detail::accumulate(std::begin(iterable), std::end(iterable), std::move(init), std::move(binaryOp));
}

/**
 * @brief Gets the maximum element in the range [begin, end) using the binary predicate @p predicate
 *
 * @param iterable The iterable to find the maximum element in
 * @param predicate The binary operator to find the maximum element with
 * @return The maximum element in the range, if the range is empty, the return value is `end`
 */
template<class Iterable, class BinaryPredicate = MAKE_BIN_OP(std::less, IterT<Iterable>)>
IterT<Iterable> maxElement(Iterable&& iterable, BinaryPredicate predicate = {}) {
    return detail::maxElement(std::begin(iterable), std::end(iterable), std::move(predicate));
}

/**
 * @brief Finds the minimum element in the range [begin, end) using the binary predicate @p predicate
 *
 * @param iterable The iterable to find the minimum element in
 * @param predicate The binary operator to find the minimum element with
 * @return The minimum element in the range or `end` if the range is empty
 */
template<class Iterable, class BinaryPredicate = MAKE_BIN_OP(std::less, ValueTypeIterable<Iterable>)>
IterT<Iterable> minElement(Iterable&& iterable, BinaryPredicate predicate = {}) {
    return detail::minElement(std::begin(iterable), std::end(iterable), std::move(predicate));
}

/**
 * @brief Finds the first element in the range [begin(iterable), end(iterable)) that satisfies the value @p value
 *
 * @param iterable The iterable to find the element in
 * @param value The value to find
 * @return The iterator to the first element that satisfies the value or `end(iterable)` if the element is not found
 */
template<class Iterable, class T>
IterT<Iterable> find(Iterable&& iterable, const T& value) {
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
IterT<Iterable> search(Iterable&& iterable, Iterable2&& iterable2) {
    return detail::search(std::begin(iterable), std::end(iterable), std::begin(iterable2), std::end(iterable2));
}

/**
 * @brief Finds the first element in the range [begin, end) that satisfies the predicate @p predicate
 *
 * @param iterable The iterable to find the element in
 * @param predicate The predicate to find the element with
 * @return Iterator to the first element that satisfies the predicate or `end(iterable)` if the element is not found
 */
template<class Iterable, class UnaryPredicate>
IterT<Iterable> findIf(Iterable&& iterable, UnaryPredicate predicate) {
    return detail::findIf(std::begin(iterable), std::end(iterable), std::move(predicate));
}

/**
 * @brief Finds the first element in the range [begin(iterable), end(iterable)) that does not satisfy the predicate @p predicate
 *
 * @param iterable The iterable to find the element in
 * @param predicate The predicate to find the element with
 * @return Iterator to the first element that does not satisfy the predicate or `end(iterable)` if the element is not found
 */
template<class Iterable, class UnaryPredicate>
IterT<Iterable> findIfNot(Iterable&& iterable, UnaryPredicate predicate) {
    return detail::findIfNot(std::begin(iterable), std::end(iterable), std::move(predicate));
}

/**
 * @brief Finds the first element in the range [begin(iterable), end(iterable)) that satisfies the value @p toFind. If the element
 * is found, it returns the value, otherwise it returns @p defaultValue
 * @param iterable The iterable to find the element in
 * @param toFind The value to find
 * @param defaultValue The value to return if the element is not found
 * @return The value @p toFind if it is found, otherwise @p defaultValue
 */
template<class Iterable, class T, class U>
ValueTypeIterable<Iterable> findFirstOrDefault(Iterable&& iterable, T&& toFind, U&& defaultValue) {
    return detail::findFirstOrDefault(std::begin(iterable), std::end(iterable), std::forward<T>(toFind),
                                      std::forward<U>(defaultValue));
}

/**
 * @brief Finds the first element in the range [begin(iterable), end(iterable)) that satisfies the predicate @p predicate. If the
 * element is found, it returns the value, otherwise it returns @p defaultValue
 * @param iterable The iterable to search.
 * @param predicate The search predicate in [begin, end). Must return bool.
 * @param defaultValue The value to return when no element matches predicate @p predicate in [begin, end)
 * @return The value if it is found, otherwise @p defaultValue
 */
template<class Iterable, class UnaryPredicate, class U>
ValueTypeIterable<Iterable> findFirstOrDefaultIf(Iterable&& iterable, UnaryPredicate predicate, U&& defaultValue) {
    return detail::findFirstOrDefaultIf(std::begin(iterable), std::end(iterable), std::move(predicate),
                                        std::forward<U>(defaultValue));
}

/**
 * @brief Reorders the elements in the range [begin(iterable), end(iterable)) in such a way that all elements for which the
 * predicate @p binaryPredicate returns true precede all elements for which predicate @p predicate returns false. Relative order
 * of the elements is not preserved
 *
 * @param iterable The iterable to partition
 * @param predicate The predicate to partition the elements with
 * @return Iterator to the first element of the second group.
 */
template<class Iterable, class UnaryPredicate>
IterT<Iterable> partition(Iterable&& iterable, UnaryPredicate predicate) {
    return detail::partition(std::begin(iterable), std::end(iterable), std::move(predicate));
}

/**
 * Gets the mean of a sequence.
 * @param iterable The iterable to calculate the mean of.
 * @param predicate The binary operator to calculate the mean with.
 * @return The mean of the container.
 */
template<class Iterable, class BinaryPredicate = MAKE_BIN_OP(std::plus, ValueTypeIterable<Iterable>)>
double mean(Iterable&& iterable, BinaryPredicate predicate = {}) {
    return detail::mean(std::begin(iterable), std::end(iterable), std::move(predicate));
}

/**
 * @brief Iterates over the range [begin(iterable), end(iterable)) and applies the unary predicate @p unaryPredicate to each
 *
 * @param iterable The iterable to iterate over
 * @param unaryPredicate The unary predicate to apply to each element
 */
template<class Iterable, class UnaryPredicate>
void forEach(Iterable&& iterable, UnaryPredicate unaryPredicate) {
    detail::forEach(std::begin(iterable), std::end(iterable), std::move(unaryPredicate));
}

/**
 * @brief Keeps iterating over @p iterable while @p predicate returns true. Essentially the same as lz::filter, however filter
 * is not generally used to alter variables in a range
 *
 * @param iterable A range
 * @param predicate Predicate that must either return true or false
 */
template<LZ_CONCEPT_ITERABLE Iterable, class UnaryPredicate>
void forEachWhile(Iterable&& iterable, UnaryPredicate predicate) {
    static_assert(std::is_convertible<detail::Decay<decltype(predicate(*iterable.begin()))>, bool>::value,
                  "Predicate must return boolean like value");
    detail::forEachWhile(detail::begin(std::forward<Iterable>(iterable)), detail::end(std::forward<Iterable>(iterable)),
                         std::move(predicate));
}

/**
 * @brief Finds the first element in the range [begin(iterable), end(iterable)) that satisfies the value @p toFind. If the
 * element is found, it returns the value, otherwise it returns @p defaultValue
 *
 * @param iterable The iterable to find the element in
 * @param toFind The value to find
 * @param defaultValue The value to return if the element is not found
 * @return The value if it is found, otherwise @p defaultValue
 */
template<class Iterable, class T, class U>
ValueTypeIterable<Iterable> findFirstOrDefault(Iterable&& iterable, const T& toFind, const U& defaultValue) {
    return detail::findFirstOrDefault(std::begin(iterable), std::end(iterable), toFind, defaultValue);
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
 * @param op The operation to transform the values with
 */
template<class Iterable, class OutputIterator, class UnaryOp>
void transform(Iterable&& iterable, OutputIterator output, UnaryOp op) {
    detail::transform(std::begin(iterable), std::end(iterable), std::move(output), std::move(op));
}

/**
 * Use this function to check if two lz iterators are the same.
 * @param a An iterable, its underlying value type should have an operator== with `b`
 * @param b An iterable, its underlying value type should have an operator== with `a`
 * @return true if both are equal, false otherwise.
 */
template<class IterableA, class IterableB, class BinaryPredicate = MAKE_BIN_OP(std::equal_to, ValueTypeIterable<IterableA>)>
bool equal(const IterableA& a, const IterableB& b, BinaryPredicate predicate = {}) {
    return detail::equal(std::begin(a), std::end(a), std::begin(b), std::end(b), std::move(predicate));
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
 * @brief Checks if the range [begin(iterable), end(iterable)) contains an element that satisfies the predicate @p predicate
 *
 * @param iterable The iterable to check
 * @param predicate The predicate to check for
 * @return `true` if the predicate is satisfied, `false` otherwise
 */
template<class Iterable, class UnaryPredicate>
bool containsIf(Iterable&& iterable, UnaryPredicate predicate) {
    return detail::findIf(std::begin(iterable), std::end(iterable), std::move(predicate)) != std::end(iterable);
}

/**
 * @brief
 *
 * @param iterable
 * @param value
 * @param predicate
 * @return IterT<Iterable>
 */
template<class Iterable, class T, class BinaryPredicate = MAKE_BIN_OP(std::equal_to, ValueTypeIterable<Iterable>)>
IterT<Iterable> lowerBound(Iterable&& iterable, const T& value, BinaryPredicate predicate) {
    return detail::lowerBound(std::begin(iterable), std::end(iterable), value, std::move(predicate));
}

template<class Iterable, class T, class BinaryPredicate = MAKE_BIN_OP(std::equal_to, ValueTypeIterable<Iterable>)>
bool binarySearch(Iterable&& iterable, const T& value, BinaryPredicate predicate) {
    return detail::binarySearch(std::begin(iterable), std::end(iterable), value, std::move(predicate));
}

LZ_MODULE_EXPORT_SCOPE_END
} // namespace lz

#endif