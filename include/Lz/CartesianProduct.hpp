#pragma once

#ifndef LZ_CARTESIAN_PRODUCT_HPP
#define LZ_CARTESIAN_PRODUCT_HPP

#include "detail/BasicIteratorView.hpp"
#include "detail/iterators/CartesianProductIterator.hpp"

namespace lz {

LZ_MODULE_EXPORT_SCOPE_BEGIN

template<class... Iterators>
class CartesianProduct final : public detail::BasicIteratorView<detail::CartesianProductIterator<Iterators...>> {
public:
    using iterator = detail::CartesianProductIterator<Iterators...>;
    using const_iterator = iterator;
    using value_type = typename iterator::value_type;

    constexpr CartesianProduct() = default;

    LZ_CONSTEXPR_CXX_20 CartesianProduct(std::tuple<Iterators...> begin, std::tuple<Iterators...> end) :
        detail::BasicIteratorView<iterator>(iterator(begin, begin, end), iterator(end, begin, end)) {
    }
};

/**
 * @addtogroup ItFns
 * @{
 */

/**
 * Creates an iterator view object that, when iterated over, gets all possible combinations of all its values of the iterators.
 * @param begin The tuple containing all the beginnings of the sequences.
 * @param end The ending containing all the endings of the sequences.
 * @return A cartesian product view object.
 */
template<LZ_CONCEPT_ITERATOR... Iterators>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 CartesianProduct<Iterators...>
cartesianRange(std::tuple<Iterators...> begin, std::tuple<Iterators...> end) {
    return { std::move(begin), std::move(end) };
}

/**
 * Creates an iterator view object that, when iterated over, gets all possible combinations of all its values of the iterables.
 * @param iterables The iterables to make all of the possible combinations with.
 * @return A cartesian product view object.
 */
template<LZ_CONCEPT_ITERABLE... Iterables>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 CartesianProduct<detail::IterTypeFromIterable<Iterables>...>
cartesian(Iterables&&... iterables) {
    return cartesianRange(std::make_tuple(detail::begin(std::forward<Iterables>(iterables))...),
                          std::make_tuple(detail::end(std::forward<Iterables>(iterables))...));
}

// End of group
/**
 * @}
 */

LZ_MODULE_EXPORT_SCOPE_END

} // namespace lz

#endif // LZ_CARTESIAN_PRODUCT_HPP
