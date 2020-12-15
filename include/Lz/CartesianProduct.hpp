#pragma once

#ifndef LZ_CARTESIAN_PRODUCT_HPP
#define LZ_CARTESIAN_PRODUCT_HPP

#include "Lz/detail/CartesianProductIterator.hpp"
#include "Lz/detail/BasicIteratorView.hpp"


namespace lz {
    template<class... Iterators>
    class CartesianProduct final : public internal::BasicIteratorView<internal::CartesianProductIterator<Iterators...>> {
    public:
        using iterator = internal::CartesianProductIterator<Iterators...>;
        using const_iterator = iterator;

        CartesianProduct() = default;

        CartesianProduct(std::tuple<Iterators...> begin, std::tuple<Iterators...> end):
            internal::BasicIteratorView<iterator>(iterator(begin, end), iterator(end, end))
        {}
    };

    /**
     * Creates an iterator view object that, when iterated over, gets all possible combinations of all its values of the iterators.
     * @param begin The tuple containing all the beginnings of the sequences.
     * @param end The ending containing all the endings of the sequences.
     * @return A cartesian product view object.
     */
    template<LZ_CONCEPT_ITERATOR... Iterators>
    CartesianProduct<Iterators...> cartesianRange(std::tuple<Iterators...> begin, std::tuple<Iterators...> end) {
        return CartesianProduct<Iterators...>(std::move(begin), std::move(end));
    }

    /**
     * Creates an iterator view object that, when iterated over, gets all possible combinations of all its values of the iterables.
     * @param iterables The iterables to make all of the possible combinations with.
     * @return A cartesian product view object.
     */
    template<LZ_CONCEPT_ITERABLE... Iterables>
    CartesianProduct<internal::IterTypeFromIterable<Iterables>...> cartesian(Iterables&&... iterables) {
        return cartesianRange(std::make_tuple(internal::begin(std::forward<Iterables>(iterables))...),
                              std::make_tuple(internal::end(std::forward<Iterables>(iterables))...));
    }
}

#endif // LZ_CARTESIAN_PRODUCT_HPP