#pragma once

#ifndef LZ_MAP_HPP
#define LZ_MAP_HPP

#include "detail/basic_iterable.hpp"
#include "detail/iterators/map.hpp"

namespace lz {

LZ_MODULE_EXPORT_SCOPE_BEGIN

template<LZ_CONCEPT_ITERATOR Iterator, class S, class Function>
class map_iterable final : public detail::basic_iterable<detail::map_iterator<Iterator, S, Function>,
                                                         typename detail::map_iterator<Iterator, S, Function::sentinel>> {

public:
    using iterator = detail::map_iterator<Iterator, S, Function>;
    using const_iterator = iterator;
    using value_type = typename iterator::value_type;

private:
    LZ_CONSTEXPR_CXX_20 map_iterable(Iterator begin, S end, Function function, std::forward_iterator_tag) :
        detail::basic_iterable<iterator, S>(iterator(std::move(begin), std::move(function)), std::move(end)) {
    }

    LZ_CONSTEXPR_CXX_20 map_iterable(Iterator begin, S end, Function function, std::bidirectional_iterator_tag) :
        detail::basic_iterable<iterator>(iterator(std::move(begin), function), iterator(std::move(end), function)) {
    }

public:
    LZ_CONSTEXPR_CXX_20 map_iterable(Iterator begin, S end, Function function) :
        map_iterable(std::move(begin), std::move(end), std::move(function), IterCat<Iterator>{}) {
    }

    constexpr map_iterable() = default;
};

// Start of group
/**
 * @addtogroup ItFns
 * @{
 */

/**
 * @brief Returns a bidirectional map object.
 * @details E.g. `map({std::pair(1, 2), std::pair(3, 2)}, [](std::pair<int, int> pairs) { return pair.first; });`
 * will return all pairs first values in the sequence, that is, `1` and `3`.
 * @param iterable The iterable to do the mapping over.
 * @param function A function that takes a value type as parameter. It may return anything.
 * @return A map_iterable object that can be converted to an arbitrary container or can be iterated over using
 * `for (auto... lz::map(...))`.
 */
template<LZ_CONCEPT_ITERABLE Iterable, class Function>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 map_iterable<iter<Iterable>, sentinel<Iterable>, detail::decay<Function>>
map(Iterable&& iterable, Function&& function) {
    return { detail::begin(std::forward<Iterable>(iterable)), detail::end(std::forward<Iterable>(iterable)),
             std::forward<Function>(function) };
}

// End of group
/**
 * @}
 */

LZ_MODULE_EXPORT_SCOPE_END

} // namespace lz

#endif