#pragma once

#ifndef LZ_MAP_HPP
#define LZ_MAP_HPP

#include "detail/BasicIteratorView.hpp"
#include "detail/iterators/MapIterator.hpp"

namespace lz {

LZ_MODULE_EXPORT_SCOPE_BEGIN

template<LZ_CONCEPT_ITERATOR Iterator, class Function>
class Map final : public detail::BasicIteratorView<detail::MapIterator<Iterator, Function>> {
public:
    using iterator = detail::MapIterator<Iterator, Function>;
    using const_iterator = iterator;
    using value_type = typename iterator::value_type;

    LZ_CONSTEXPR_CXX_20 Map(Iterator begin, Iterator end, Function function) :
        detail::BasicIteratorView<iterator>(iterator(std::move(begin), function), iterator(std::move(end), function)) {
    }

    constexpr Map() = default;
};

// Start of group
/**
 * @addtogroup ItFns
 * @{
 */

/**
 * @brief Returns a map object.
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
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 Map<IterT<Iterable>, detail::Decay<Function>> map(Iterable&& iterable, Function&& function) {
    return mapRange(detail::begin(std::forward<Iterable>(iterable)), detail::end(std::forward<Iterable>(iterable)),
                    std::forward<Function>(function));
}

// End of group
/**
 * @}
 */

LZ_MODULE_EXPORT_SCOPE_END

} // namespace lz

#endif