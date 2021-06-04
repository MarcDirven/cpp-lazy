#pragma once

#ifndef LZ_GROUP_BY_HPP
#define LZ_GROUP_BY_HPP

#include "detail/GroupByIterator.hpp"

namespace lz {

#ifdef LZ_HAS_EXECUTION
template<LZ_CONCEPT_ITERATOR Iterator, class Comparer, class Execution>
class GroupBy : public internal::BasicIteratorView<internal::GroupByIterator<Iterator, Comparer, Execution>> {
    using iterator = internal::GroupByIterator<Iterator, Comparer, Execution>;
#else // ^^ LZ_HAS_EXECUTION vv !LZ_HAS_EXECUTION
template<class Iterator, class Comparer>
class GroupBy final : public internal::BasicIteratorView<internal::GroupByIterator<Iterator, Comparer>> {
    using iterator = internal::GroupByIterator<Iterator, Comparer>;
#endif
    using const_iterator = iterator;

  public:
#ifdef LZ_HAS_EXECUTION
    LZ_CONSTEXPR_CXX_20 GroupBy(Iterator begin, Iterator end, Comparer comparer, Execution execution) :
        internal::BasicIteratorView<iterator>(iterator(std::move(begin), end, comparer, execution),
                                              iterator(end, end, comparer, execution))
#else  // ^^ LZ_HAS_EXECUTION vv !LZ_HAS_EXECUTION
    GroupBy(Iterator begin, Iterator end, Comparer comparer) :
        internal::BasicIteratorView<iterator>(iterator(std::move(begin), end, comparer), iterator(end, end, comparer))
#endif // LZ_HAS_EXECUTION
    {
    }

    constexpr GroupBy() = default;
};

/**
 * @addtogroup ItFns
 * @{
 */

#ifdef LZ_HAS_EXECUTION
/**
 * Chops a sequence into chunks, where every chunk is grouped based on a grouping predicate.
 * @attention [begin, end) must be sorted in order to work properly.
 * @param begin The beginning of the sequence.
 * @param end The ending of the sequence.
 * @param execution The execution policy for finding, sorting and checking if the sequence is sorted.
 * @param comparer The comparer to make groups with. For e.g. if value_type is string, one can use
 * `[](string a, string b) { return a.length() == b.length() }` to make groups where sizes of the strings are equal.
 * @return A GroupBy iterator view object.
 */
template<LZ_CONCEPT_ITERATOR Iterator, class Comparer = std::equal_to<>, class Execution = std::execution::sequenced_policy>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 GroupBy<Iterator, Comparer, Execution>
groupByRange(Iterator begin, Iterator end, Comparer comparer = {}, Execution execution = std::execution::seq) {
    return GroupBy<Iterator, Comparer, Execution>(std::move(begin), std::move(end), std::move(comparer), execution);
}

/**
 * Chops a sequence into chunks, where every chunk is grouped based on a grouping predicate.
 * @param iterable The iterable to group into chunks.
 * @param execution The execution policy for finding, sorting and checking if the sequence is sorted.
 * @param comparer The comparer to make groups with. For e.g. if value_type is string, one can use
 * `[](string a, string b) { return a.length() == b.length() }` to make groups where sizes of the strings are equal.
 * @return A GroupBy iterator view object.
 */
template<LZ_CONCEPT_ITERABLE Iterable, class Comparer = std::equal_to<>, class Execution = std::execution::sequenced_policy>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 GroupBy<internal::IterTypeFromIterable<Iterable>, Comparer, Execution>
groupBy(Iterable&& iterable, Comparer comparer = {}, Execution execution = std::execution::seq) {
    return groupByRange(internal::begin(std::forward<Iterable>(iterable)), internal::end(std::forward<Iterable>(iterable)),
                        std::move(comparer), execution);
}
#else // ^^ LZ_HAS_EXECUTION vv !LZ_HAS_EXECUTION

/**
 * Chops a sequence into chunks, where every chunk is grouped based on a grouping predicate.
 * @attention [begin, end) must be sorted in order to work properly.
 * @param begin The beginning of the sequence.
 * @param end The ending of the sequence.
 * @param comparer The comparer to make groups with. For e.g. if value_type is string, one can use
 * `[](string a, string b) { return a.length() == b.length() }` to make groups where sizes of the strings are equal.
 * @return A GroupBy iterator view object.
 */
#ifdef LZ_HAS_CXX_11
template<class Iterator, class Comparer = std::equal_to<internal::ValueType<Iterator>>>
#else
template<class Iterator, class Comparer = std::equal_to<>>
#endif // LZ_HAS_CXX_11
GroupBy<Iterator, Comparer> groupByRange(Iterator begin, Iterator end, Comparer keySelector = {}) {
    return GroupBy<Iterator, Comparer>(std::move(begin), std::move(end), std::move(keySelector));
}

/**
 * Chops a sequence into chunks, where every chunk is grouped based on a grouping predicate.
 * @attention `iterable` must be sorted in order to work properly.
 * @param iterable The iterable to group into chunks.
 * @param comparer The comparer to make groups with. For e.g. if value_type is string, one can use
 * `[](string a, string b) { return a.length() == b.length() }` to make groups where sizes of the strings are equal.
 * @return A GroupBy iterator view object.
 */
#ifdef LZ_HAS_CXX_11
template<class Iterable, class Comparer = std::equal_to<internal::ValueType<internal::IterTypeFromIterable<Iterable>>>>
#else
template<class Iterable, class Comparer = std::equal_to<>>
#endif // LZ_HAS_CXX_11
GroupBy<internal::IterTypeFromIterable<Iterable>, Comparer> groupBy(Iterable&& iterable, Comparer comparer = {}) {
    return groupByRange(internal::begin(std::forward<Iterable>(iterable)), internal::end(std::forward<Iterable>(iterable)),
                        std::move(comparer));
}

#endif // end LZ_HAS_EXECUTION

// End of group
/**
 * @}
 */
} // namespace lz

#endif // LZ_GROUP_BY_HPP
