#pragma once

#ifndef LZ_UNIQUE_HPP
#define LZ_UNIQUE_HPP

#include "detail/BasicIterable.hpp"
#include "detail/iterators/UniqueIterator.hpp"

namespace lz {

LZ_MODULE_EXPORT_SCOPE_BEGIN

#ifdef LZ_HAS_EXECUTION
template<class Execution, LZ_CONCEPT_ITERATOR Iterator, class Compare>
class Unique final : public detail::BasicIterable<detail::UniqueIterator<Execution, Iterator, Compare>> {
#else
template<LZ_CONCEPT_ITERATOR Iterator, class Compare>
class Unique final : public detail::BasicIterable<detail::UniqueIterator<Iterator, Compare>> {
#endif
public:
#ifdef LZ_HAS_EXECUTION
    using iterator = detail::UniqueIterator<Execution, Iterator, Compare>;
#else
    using iterator = detail::UniqueIterator<Iterator, Compare>;
#endif
    using const_iterator = iterator;
    using value_type = typename iterator::value_type;

#ifdef LZ_HAS_EXECUTION
    LZ_CONSTEXPR_CXX_20 Unique(Iterator begin, Iterator end, Compare compare, Execution e) :
        detail::BasicIterable<iterator>(iterator(std::move(begin), end, compare, e), iterator(end, end, compare, e)) {
    }
#else
    Unique(Iterator begin, Iterator end, Compare compare) :
        detail::BasicIterable<iterator>(iterator(std::move(begin), end, compare), iterator(end, end, compare)) {
    }
#endif

    constexpr Unique() = default;
};

// Start of group
/**
 * @addtogroup ItFns
 * @{
 */

#ifdef LZ_HAS_EXECUTION
/**
 * @brief Returns an Unique iterator view object.
 * @attention [begin, end) must be sorted in order to work properly.
 * @details Use this iterator view to eventually get an iterator of unique values.
 * @param begin The beginning of the sequence.
 * @param end The ending of the sequence.
 * @param execPolicy The execution policy. Must be one of `std::execution`'s tags. Finding the adjacent
 * element are done using this policy.
 * @param sortFunc (Optional) to find adjacent elements.
 * @return An Unique iterator view object, which can be used to iterate over in a `(for ... : uniqueRange(...))` fashion.
 */
template<class Execution = std::execution::sequenced_policy, LZ_CONCEPT_ITERATOR Iterator, class Compare = std::less<>>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 Unique<Execution, Iterator, Compare>
uniqueRange(Iterator begin, Iterator end, Compare compare = {}, Execution execPolicy = std::execution::seq) {
    return { std::move(begin), std::move(end), std::move(compare), execPolicy };
}

/**
 * @brief Returns an Unique iterator view object.
 * @attention [begin, end) must be sorted in order to work properly.
 * @details Use this iterator view to eventually get an iterator of unique values.
 * @param begin The beginning of the sequence.
 * @param end The ending of the sequence.
 * @param execPolicy The execution policy. Must be one of `std::execution`'s tags. Finding the adjacent
 * element are done using this policy.
 * @param sortFunc (Optional) to find adjacent elements.
 * @return An Unique iterator view object, which can be used to iterate over in a `(for ... : uniqueRange(...))` fashion.
 */
template<class Execution = std::execution::sequenced_policy, LZ_CONCEPT_ITERABLE Iterable, class It = IterT<Iterable>,
         class Compare = std::less<>>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 Unique<Execution, It, Compare>
unique(Iterable&& iterable, Compare compare = {}, Execution execPolicy = std::execution::seq) {
    return uniqueRange(detail::begin(std::forward<Iterable>(iterable)), detail::end(std::forward<Iterable>(iterable)),
                       std::move(compare), execPolicy);
}
#else // ^^^ has execution vvv !has execution
/**
 * @brief Returns an Unique iterator view object.
 * @attention [begin, end) must be sorted in order to work properly.
 * @details Use this iterator view to eventually get an iterator of unique values.
 * @param begin The beginning of the sequence.
 * @param end The ending of the sequence.
 * @param sortFunc (Optional) to find adjacent elements.
 * @return An Unique iterator view object, which can be used to iterate over in a `(for ... : uniqueRange(...))` fashion.
 */
#ifdef LZ_HAS_CXX_11
template<class Iterator, class Compare = std::less<ValueType<Iterator>>>
#else
template<class Iterator, class Compare = std::less<>>
#endif // LZ_HAS_CXX_11
Unique<Iterator, Compare> uniqueRange(Iterator begin, Iterator end, Compare compare = {}) {
    return { std::move(begin), std::move(end), std::move(compare) };
}

/**
 * @brief Returns an Unique iterator view object.
 * @attention `iterable` must be sorted in order to work correctly.
 * @details Use this iterator view to eventually get an iterator of unique values.
 * @param iterable The iterable sequence.
 * @param sortFunc (Optional) to find adjacent elements.
 * @return An Unique iterator view object, which can be used to iterate over in a `(for ... : unique(...))` fashion.
 */
#ifdef LZ_HAS_CXX_11
template<class Iterable, class Compare = std::less<ValueTypeIterable<Iterable>>>
#else
template<class Iterable, class Compare = std::less<>>
#endif // LZ_HAS_CXX_11
Unique<IterT<Iterable>, Compare> unique(Iterable&& iterable, Compare compare = {}) {
    return uniqueRange(detail::begin(std::forward<Iterable>(iterable)), detail::end(std::forward<Iterable>(iterable)),
                       std::move(compare));
}

#endif // LZ_HAS_EXECUTION

// End of group
/**
 * @}
 */

LZ_MODULE_EXPORT_SCOPE_END

} // end namespace lz

#endif // end LZ_UNIQUE_HPP