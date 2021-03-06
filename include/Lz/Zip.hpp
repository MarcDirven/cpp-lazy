#pragma once

#ifndef LZ_ZIP_HPP
#define LZ_ZIP_HPP

#include "detail/BasicIteratorView.hpp"
#include "detail/ZipIterator.hpp"

namespace lz {
template<LZ_CONCEPT_ITERATOR... Iterators>
class Zip final : public internal::BasicIteratorView<internal::ZipIterator<Iterators...>> {
public:
    using iterator = internal::ZipIterator<Iterators...>;
    using const_iterator = iterator;

    using value_type = typename iterator::value_type;

    LZ_CONSTEXPR_CXX_20 Zip(std::tuple<Iterators...> begin, std::tuple<Iterators...> end) :
        internal::BasicIteratorView<iterator>(iterator(std::move(begin)), iterator(std::move(end))) {
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
 * Its `begin()` function returns a random access iterator. The operators `<, <=, >, >=` will return true
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
    return { std::move(begin), std::move(end) };
}

/**
 * @brief This function can be used to iterate over multiple containers. It stops at its smallest container.
 * Its `begin()` function returns a random access iterator. The operators `<, <=, >, >=` will return true
 * if one of the containers returns true with its corresponding `operator<`/`operator<=`/`operator>`/`operator>=`.
 * @details The tuple that is returned by `operator*` returns a `std::tuple` by value and its elements by
 * reference e.g. `std::tuple<Args&...>`. So it is possible to alter the values in the container/iterable),
 * unless the iterator is const, making it a const reference.
 * @param iterables The iterables to iterate simultaneously over.
 * @return A Take object that can be converted to an arbitrary container or can be iterated over using
 * `for (auto tuple :  lz::zip(...))`.
 */
template<LZ_CONCEPT_ITERABLE... Iterables>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 Zip<internal::IterTypeFromIterable<Iterables>...> zip(Iterables&&... iterables) {
    return zipRange(std::make_tuple(internal::begin(std::forward<Iterables>(iterables))...),
                    std::make_tuple(internal::end(std::forward<Iterables>(iterables))...));
}

// End of group
/**
 * @}
 */
} // namespace lz

#endif