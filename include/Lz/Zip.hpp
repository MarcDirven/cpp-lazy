#pragma once

#ifndef LZ_ZIP_HPP
#    define LZ_ZIP_HPP

#    include "detail/BasicIteratorView.hpp"
#    include "detail/ZipIterator.hpp"

namespace lz {
namespace internal {
template<class DiffTy, class Tuple, size_t... Is>
LZ_CONSTEXPR_CXX_20 void doTrimTuple(const Tuple& begin, Tuple& end, const DiffTy* lengths, IndexSequence<Is...>) {
    using lz::next;
    using std::next;
    const auto smallest = static_cast<DiffTy>(*std::min_element(lengths, lengths + sizeof...(Is)));
    const int expander[] = { (std::get<Is>(end) = next(std::get<Is>(begin), smallest), 0)... };
    static_cast<void>(expander);
}

template<class... Iterators, class DiffTy = typename std::common_type<DiffType<Iterators>...>::type>
LZ_CONSTEXPR_CXX_20 void trimTuple(const std::tuple<Iterators...>& begin, std::tuple<Iterators...>& end, const DiffTy* lengths) {
    constexpr size_t argLength = sizeof...(Iterators);
    static_assert(argLength > 1, "zip requires more than 1 container/iterable");
    doTrimTuple(begin, end, lengths, MakeIndexSequence<argLength>());
}

template<class Tuple, class DiffTy, std::size_t... Is>
LZ_CONSTEXPR_CXX_20 void doGetLengthFromIterators(const Tuple& begin, const Tuple& end, DiffTy* lengths, IndexSequence<Is...>) {
    auto expander = { (lengths[Is] = getIterLength(std::get<Is>(begin), std::get<Is>(end)), 0)... };
    static_cast<void>(expander);
}

template<class DiffTy, class... Iterators>
LZ_CONSTEXPR_CXX_20 void
getLengthFromIterators(const std::tuple<Iterators...>& begin, const std::tuple<Iterators...>& end, DiffTy* lengths) {
    doGetLengthFromIterators(begin, end, lengths, MakeIndexSequence<sizeof...(Iterators)>());
}

template<class DiffTy, class... Iterables, std::size_t... Is>
LZ_CONSTEXPR_CXX_20 void doGetLengthFromIterables(DiffTy* out, IndexSequence<Is...>, const Iterables&... iterables) {
    auto expander = { (out[Is] = static_cast<DiffTy>(iterables.size()), 0)... };
    static_cast<void>(expander);
}

template<class DiffTy, class... Iterables>
LZ_CONSTEXPR_CXX_20 void getLengthFromIterables(DiffTy* out, const Iterables&... iterables) {
    doGetLengthFromIterables(out, internal::MakeIndexSequence<sizeof...(Iterables)>(), iterables...);
}
} // namespace internal

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
    using DiffTy = typename std::common_type<internal::DiffType<Iterators>...>::type;
    DiffTy lengths[sizeof...(Iterators)]{};
    internal::getLengthFromIterators(begin, end, lengths);
    internal::trimTuple(begin, end, lengths);
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
    using DiffTy = typename std::common_type<internal::DiffTypeIterable<Iterables>...>::type;
    DiffTy lengths[sizeof...(Iterables)]{};
    internal::getLengthFromIterables(lengths, iterables...);
    auto begin = std::make_tuple(internal::begin(std::forward<Iterables>(iterables))...);
    auto end = std::make_tuple(internal::end(std::forward<Iterables>(iterables))...);
    internal::trimTuple(begin, end, lengths);
    return { std::move(begin), std::move(end) };
}

// End of group
/**
 * @}
 */
} // namespace lz

#endif