#pragma once

#ifndef LZ_ZIP_LONGEST_HPP
#    define LZ_ZIP_LONGEST_HPP

#    include "detail/BasicIteratorView.hpp"
#    include "detail/ZipLongestIterator.hpp"

namespace lz {

LZ_MODULE_EXPORT_SCOPE_BEGIN

template<LZ_CONCEPT_ITERATOR... Iterators>
class ZipLongest final
    : public internal::BasicIteratorView<internal::ZipLongestIterator<
          internal::IsRandomAccessTag<typename std::common_type<internal::IterCat<Iterators>...>::type>::value, Iterators...>> {
    using IterCat = typename std::common_type<internal::IterCat<Iterators>...>::type;

    static_assert(sizeof...(Iterators) > 0, "Cannot create zip longest object with 0 iterators");

public:
    using iterator = internal::ZipLongestIterator<internal::IsRandomAccessTag<IterCat>::value, Iterators...>;
    using const_iterator = iterator;

    using value_type = typename iterator::value_type;

    template<class I = IterCat>
    LZ_CONSTEXPR_CXX_20 ZipLongest(std::tuple<Iterators...> begin, std::tuple<Iterators...> end,
                                   internal::EnableIf<!internal::IsRandomAccessTag<I>::value, int> = 0) :
        internal::BasicIteratorView<iterator>(iterator(begin, end), iterator(end, begin)) {
    }

    template<class I = IterCat>
    LZ_CONSTEXPR_CXX_20 ZipLongest(std::tuple<Iterators...> begin, std::tuple<Iterators...> end,
                                   internal::EnableIf<internal::IsRandomAccessTag<I>::value, int> = 0) :
        internal::BasicIteratorView<iterator>(iterator(begin, begin, end), iterator(begin, end, end)) {
    }

    constexpr ZipLongest() = default;
};

// Start of group
/**
 * @addtogroup ItFns
 * @{
 */

/**
 * @brief Zip iterator that zips multiple iterators.
 *
 * @details Zip iterator that zips multiple iterators, except the difference beteween zipLongest and zip is that zip longest stops
 * when the longest iterator is at its end. Other iterators that have been 'exhausted' will be filled with an empty Optional
 * object, rather than a valid Optional object. This is std::optional (if post C++ 17) or lz::internal::Optional (if pre C++).
 * lz::internal::Optional contains most of the basic funcitonality std::optional does. This iterator is a forward iterator if one of
 * the iterators passed is bidirectional or lower, random access otherwise.
 *
 * @note All values retrieved from the iterator are by value.
 * @param begin Beginning of the sequence
 * @param end Ending of the sequence
 * @return ZipLongest object that contains the iterator.
 */
template<LZ_CONCEPT_ITERATOR... Iterators>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 ZipLongest<Iterators...>
zipLongestRange(std::tuple<Iterators...> begin, std::tuple<Iterators...> end) {
    return { std::move(begin), std::move(end) };
}

/**
 * @brief Zip iterator that zips multiple iterators.
 *
 * @details Zip iterator that zips multiple iterators, except the difference beteween zipLongest and zip is that zip longest stops
 * when the longest iterator is at its end. Other iterators that have been 'exhausted' will be filled with an empty Optional
 * object, rather than a valid Optional object. This is std::optional (if post C++ 17) or lz::internal::Optional (if pre C++).
 * lz::internal::Optional contains most of the basic funcitonality std::optional does. This iterator is a forward iterator if one of
 * the iterators passed is bidirectional or lower, random access otherwise.
 * 
 * @note All values retrieved from the iterator are by value.
 * @param iterables The iterables objects
 * @return ZipLongest object that contains the iterator. 
 */
template<LZ_CONCEPT_ITERABLE... Iterables>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 ZipLongest<internal::IterTypeFromIterable<Iterables>...> zipLongest(Iterables&&... iterables) {
    auto begin = std::make_tuple(internal::begin(std::forward<Iterables>(iterables))...);
    auto end = std::make_tuple(internal::end(std::forward<Iterables>(iterables))...);
    return lz::zipLongestRange(std::move(begin), std::move(end));
}

// End of group
/**
 * @}
 */

LZ_MODULE_EXPORT_SCOPE_END

} // namespace lz

#endif // LZ_ZIP_LONGEST_HPP