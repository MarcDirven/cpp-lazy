#pragma once

#ifndef LZ_CARTESIAN_PRODUCT_HPP
#define LZ_CARTESIAN_PRODUCT_HPP

#include "Lz/detail/basic_iterable.hpp"
#include "Lz/detail/iterators/cartesian_product.hpp"

namespace lz {

LZ_MODULE_EXPORT_SCOPE_BEGIN

template<class IterTuple, class SentinelTuple>
class cartesian_product_iterable final
    : public detail::basic_iterable<detail::cartesian_product_iterator<IterTuple, SentinelTuple>,
                                    typename detail::cartesian_product_iterator<IterTuple, SentinelTuple>::sentinel> {

    using s = typename detail::cartesian_product_iterator<IterTuple, SentinelTuple>::sentinel;

    LZ_CONSTEXPR_CXX_20
    cartesian_product_iterable(IterTuple begin, SentinelTuple end, std::forward_iterator_tag /* unused */) :
        detail::basic_iterable<iterator, s>(iterator(std::move(begin), std::move(begin), std::move(end))) {
    }

    LZ_CONSTEXPR_CXX_20
    cartesian_product_iterable(IterTuple begin, SentinelTuple end, std::bidirectional_iterator_tag /* unused */) :
        detail::basic_iterable<iterator>(iterator(begin, begin, end), iterator(end, begin, end)) {
    }

public:
    using iterator = detail::cartesian_product_iterator<IterTuple, SentinelTuple>;
    using const_iterator = iterator;
    using value_type = typename iterator::value_type;

    constexpr cartesian_product_iterable() = default;

    LZ_CONSTEXPR_CXX_20 cartesian_product_iterable(IterTuple begin, SentinelTuple end) :
        cartesian_product_iterable(std::move(begin), std::move(end), iter_cat_t<iterator>{}) {
    }
};

/**
 * @addtogroup ItFns
 * @{
 */

/**
 * Creates an iterator view object that, when iterated over, gets all possible combinations of all its values of the iterables.
 * @param iterables The iterables to make all of the possible combinations with.
 * @return A cartesian_product product view object.
 */
// clang-format off
template<LZ_CONCEPT_ITERABLE... Iterables>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20
cartesian_product_iterable<std::tuple<iter_t<Iterables>...>, std::tuple<sentinel_t<Iterables>...>>
cartesian_product(Iterables&&... iterables) {
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
