#pragma once

#ifndef LZ_ZIP_LONGEST_HPP
#define LZ_ZIP_LONGEST_HPP

#include "detail/basic_iterable.hpp"
#include "detail/iterators/zip_longest.hpp"

namespace lz {

LZ_MODULE_EXPORT_SCOPE_BEGIN

template<LZ_CONCEPT_ITERATOR... Iterators>
class zip_longest_iterable final
    : public detail::basic_iterable<
        detail::zip_longest_iterator<detail::is_ra<detail::common_type<iter_cat<Iterators>...>>::value, Iterators...>> {

    using ic = detail::common_type<iter_cat<Iterators>...>;

    static_assert(sizeof...(Iterators) > 0, "Cannot create zip longest object with 0 iterators");
    // TODO write std::forward_iterator_tags overload instead of enable_if
public:
    using iterator = detail::zip_longest_iterator<detail::is_ra<IterCat>::value, Iterators...>;
    using const_iterator = iterator;

    using value_type = typename iterator::value_type;

    template<class I = ic>
    LZ_CONSTEXPR_CXX_20 zip_longest_iterable(std::tuple<Iterators...> begin, std::tuple<Iterators...> end,
                                             detail::enable_if<!detail::is_ra<I>::value, int> = 0) :
        detail::basic_iterable<iterator>(iterator(begin, end), iterator(end, begin)) {
    }

    template<class I = ic>
    LZ_CONSTEXPR_CXX_20 zip_longest_iterable(std::tuple<Iterators...> begin, std::tuple<Iterators...> end,
                                             detail::enable_if<detail::is_ra<I>::value, int> = 0) :
        detail::basic_iterable<iterator>(iterator(begin, begin, end), iterator(begin, end, end)) {
    }

    constexpr zip_longest_iterable() = default;
};

// Start of group
/**
 * @addtogroup ItFns
 * @{
 */

/**
 * @brief Zip iterator that zips multiple iterators.
 *
 * @details Zip iterator that zips multiple iterators, except the difference beteween zip_longest and zip is that zip longest stops
 * when the longest iterator is at its end. Other iterators that have been 'exhausted' will be filled with an empty optional
 * object, rather than a valid optional object. This is std::optional (if post C++ 17) or lz::detail::optional (if pre C++).
 * lz::detail::optional contains most of the basic funcitonality std::optional does. This iterator is a forward iterator if one of
 * the iterators passed is bidirectional or lower, random access otherwise.
 *
 * @note All values retrieved from the iterator are by value.
 * @param iterables The iterables objects
 * @return zip_longest_iterable object that contains the iterator.
 */
template<LZ_CONCEPT_ITERABLE... Iterables>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 zip_longest_iterable<iter<Iterables>...> zip_longest(Iterables&&... iterables) {
    auto begin = std::make_tuple(detail::begin(std::forward<Iterables>(iterables))...);
    auto end = std::make_tuple(detail::end(std::forward<Iterables>(iterables))...);
    return {std::move(begin), std::move(end)};
}

// End of group
/**
 * @}
 */

LZ_MODULE_EXPORT_SCOPE_END

} // namespace lz

#endif // LZ_ZIP_LONGEST_HPP