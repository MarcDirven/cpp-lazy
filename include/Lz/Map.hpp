#pragma once

#ifndef LZ_MAP_HPP
#define LZ_MAP_HPP

#include "detail/BasicIteratorView.hpp"
#include "detail/MapIterator.hpp"

namespace lz {
template<LZ_CONCEPT_ITERATOR Iterator, class Function>
class Map final : public internal::BasicIteratorView<internal::MapIterator<Iterator, Function>> {
public:
    using iterator = internal::MapIterator<Iterator, Function>;
    using const_iterator = iterator;
    using value_type = typename iterator::value_type;

    /**
     * @brief The Map constructor.
     * @param begin Beginning of the iterator.
     * @param end End of the iterator.
     * @param function A function with parameter the value type. It may return anything.
     */
    LZ_CONSTEXPR_CXX_20 Map(Iterator begin, Iterator end, Function function) :
        internal::BasicIteratorView<iterator>(iterator(std::move(begin), function), iterator(std::move(end), function)) {
    }

    constexpr Map() = default;
};

// Start of group
/**
 * @addtogroup ItFns
 * @{
 */

/**
 * @brief Returns a random access map object. If MSVC and the type is an STL iterator, pass a pointer iterator, not
 * an actual iterator object.
 * @details E.g. `map({std::pair(1, 2), std::pair(3, 2)}, [](std::pair<int, int> pairs) { return pair.first; });`
 * will return all pairs first values in the sequence, that is, `1` and `3`.
 * @param begin The beginning of the sequence.
 * @param end The ending of the sequence.
 * @param function A function that takes a value type as parameter. It may return anything.
 * @return A Map object from [begin, end) that can be converted to an arbitrary container or can be iterated over
 * using `for (auto... lz::map(...))`.
 */
template<class Function, LZ_CONCEPT_ITERATOR Iterator>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 Map<Iterator, Function> mapRange(Iterator begin, Iterator end, Function function) {
    return { std::move(begin), std::move(end), std::move(function) };
}

/**
 * @brief Returns a bidirectional map object.
 * @details E.g. `map({std::pair(1, 2), std::pair(3, 2)}, [](std::pair<int, int> pairs) { return pair.first; });`
 * will return all pairs first values in the sequence, that is, `1` and `3`.
 * @param iterable The iterable to do the mapping over.
 * @param function A function that takes a value type as parameter. It may return anything.
 * @return A Map object that can be converted to an arbitrary container or can be iterated over using
 * `for (auto... lz::map(...))`.
 */
template<class Function, LZ_CONCEPT_ITERABLE Iterable>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 Map<internal::IterTypeFromIterable<Iterable>, Function>
map(Iterable&& iterable, Function function) {
    return mapRange(internal::begin(std::forward<Iterable>(iterable)), internal::end(std::forward<Iterable>(iterable)),
                    std::move(function));
}

// End of group
/**
 * @}
 */
} // namespace lz

#endif