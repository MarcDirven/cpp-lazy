#pragma once

#ifndef LZ_ZIP_HPP
#define LZ_ZIP_HPP

#include "detail/BasicIterable.hpp"
#include "detail/iterators/ZipIterator.hpp"

namespace lz {
namespace detail {
template<class Tuple, std::size_t... Is>
Tuple createEndSmallestIterator(const Tuple& begin, Tuple end, index_sequence<Is...>) {
    const std::ptrdiff_t lengths[] = { static_cast<std::ptrdiff_t>(
        lz::detail::size_hint(std::get<Is>(begin), std::get<Is>(end)))... };
    const auto smallestLength = *std::min_element(std::begin(lengths), std::end(lengths));
    // If we use begin + smallestLength, we get compile errors for non random access iterators. However, we know that we are
    // dealing with a random access iterator, so std::next does a + internally. It is implemented this way to prevent more
    // enable_if's from appearing
    return Tuple{ std::next(std::get<Is>(begin), smallestLength)... };
}
} // namespace detail

LZ_MODULE_EXPORT_SCOPE_BEGIN

template<LZ_CONCEPT_ITERATOR... Iterators>
class Zip final : public detail::basic_iterable<detail::zip_iterator<Iterators...>> {
public:
    using iterator = detail::zip_iterator<Iterators...>;
    using const_iterator = iterator;

    using value_type = typename iterator::value_type;

    LZ_CONSTEXPR_CXX_20 Zip(std::tuple<Iterators...> begin, std::tuple<Iterators...> end) :
        detail::basic_iterable<iterator>(iterator(std::move(begin)), iterator(std::move(end))) {
    }

    constexpr Zip() = default;
};

// Start of group
/**
 * @addtogroup ItFns
 * @{
 */

/**
 * @brief This function can be used to iterate over multiple containers. It stops at its smallest container.
 * Its `begin()` function returns an iterator. The operators `<, <=, >, >=` will return true
 * if one of the containers returns true with its corresponding `operator<`/`operator<=`/`operator>`/`operator>=`.
 * @details The tuple that is returned by `operator*` returns a `std::tuple` by value and its elements by
 * reference e.g. `std::tuple<Args&...>`. So it is possible to alter the values in the container/iterable),
 * unless the iterator is const, making it a const reference.
 * @param begin The beginning of the sequence
 * @param end The ending of the sequence
 * @return A Take object that can be converted to an arbitrary container or can be iterated over using
 * `for (auto tuple :  lz::zip(...))`.
 */
template<LZ_CONCEPT_ITERATOR... Iterators>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 Zip<Iterators...> zipRange(std::tuple<Iterators...> begin, std::tuple<Iterators...> end) {
    static_assert(sizeof...(Iterators) > 0, "Cannot create zip object with 0 iterators");
    using CommonIterTag = detail::common_type<IterCat<Iterators>...>;
    if LZ_CONSTEXPR_IF (detail::is_ra<CommonIterTag>::value) {
        end = detail::createEndSmallestIterator(begin, std::move(end), detail::make_index_sequence<sizeof...(Iterators)>());
    }
    return { std::move(begin), std::move(end) };
}

/**
 * @brief This function can be used to iterate over multiple containers. It stops at its smallest container.
 * Its `begin()` function returns an iterator. The operators `<, <=, >, >=` will return true
 * if one of the containers returns true with its corresponding `operator<`/`operator<=`/`operator>`/`operator>=`.
 * @details The tuple that is returned by `operator*` returns a `std::tuple` by value and its elements by
 * reference e.g. `std::tuple<Args&...>`. So it is possible to alter the values in the container/iterable),
 * unless the iterator is const, making it a const reference.
 * @param iterables The iterables to iterate simultaneously over.
 * @return A Take object that can be converted to an arbitrary container or can be iterated over using
 * `for (auto tuple :  lz::zip(...))`.
 */
template<LZ_CONCEPT_ITERABLE... Iterables>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 Zip<iter<Iterables>...> zip(Iterables&&... iterables) {
    auto begin = std::make_tuple(detail::begin(std::forward<Iterables>(iterables))...);
    auto end = std::make_tuple(detail::end(std::forward<Iterables>(iterables))...);
    return lz::zipRange(std::move(begin), std::move(end));
}

// End of group
/**
 * @}
 */
} // namespace lz

LZ_MODULE_EXPORT_SCOPE_END

#endif