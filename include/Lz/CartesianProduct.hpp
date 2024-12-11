#pragma once

#ifndef LZ_CARTESIAN_PRODUCT_HPP
#define LZ_CARTESIAN_PRODUCT_HPP

#include "detail/BasicIterable.hpp"
#include "detail/iterators/CartesianProductIterator.hpp"

namespace lz {

LZ_MODULE_EXPORT_SCOPE_BEGIN

template<class IterTuple, class SentinelTuple>
class CartesianProduct final
    : public detail::BasicIterable<detail::CartesianProductIterator<IterTuple, SentinelTuple>,
                                       typename detail::CartesianProductIterator<IterTuple, SentinelTuple>::Sentinel> {

    using Sentinel = typename detail::CartesianProductIterator<IterTuple, SentinelTuple>::Sentinel;

    LZ_CONSTEXPR_CXX_20
    CartesianProduct(IterTuple begin, SentinelTuple end, std::forward_iterator_tag /* unused */) :
        detail::BasicIterable<iterator, Sentinel>(iterator(begin, begin, end)) {
    }

    LZ_CONSTEXPR_CXX_20 CartesianProduct(IterTuple begin, SentinelTuple end, std::bidirectional_iterator_tag /* unused */) :
        detail::BasicIterable<iterator>(iterator(begin, begin, end), iterator(end, begin, end)) {
    }

public:
    using iterator = detail::CartesianProductIterator<IterTuple, SentinelTuple>;
    using const_iterator = iterator;
    using value_type = typename iterator::value_type;

    constexpr CartesianProduct() = default;

    LZ_CONSTEXPR_CXX_20 CartesianProduct(IterTuple begin, SentinelTuple end) :
        CartesianProduct(std::move(begin), std::move(end), IterCat<iterator>{}) {
    }
};

/**
 * @addtogroup ItFns
 * @{
 */

// clang-format off

/**
 * Creates an iterator view object that, when iterated over, gets all possible combinations of all its values of the iterables.
 * @param iterables The iterables to make all of the possible combinations with.
 * @return A cartesian product view object.
 */
template<LZ_CONCEPT_ITERABLE... Iterables>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20
CartesianProduct<std::tuple<IterT<Iterables>...>, std::tuple<SentinelT<Iterables>...>>
cartesian(Iterables&&... iterables) {
    return { std::make_tuple(detail::begin(std::forward<Iterables>(iterables))...),
             std::make_tuple(detail::end(std::forward<Iterables>(iterables))...) };
}

// clang-format on

// End of group
/**
 * @}
 */

LZ_MODULE_EXPORT_SCOPE_END

} // namespace lz

#endif // LZ_CARTESIAN_PRODUCT_HPP
