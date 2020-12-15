#pragma once

#ifndef LZ_CARTESIAN_PRODUCT_HPP
#define LZ_CARTESIAN_PRODUCT_HPP

#include "Lz/detail/CartesianProductIterator.hpp"
#include "Lz/detail/BasicIteratorView.hpp"


namespace lz {
    template<class... Iterators>
    class CartesianProduct : public internal::BasicIteratorView<internal::CartesianProductIterator<Iterators...>> {
    public:
        using iterator = internal::CartesianProductIterator<Iterators...>;
        using const_iterator = iterator;

    public:
        CartesianProduct() = default;

        CartesianProduct(std::tuple<Iterators...> begin, std::tuple<Iterators...> end):
            internal::BasicIteratorView<iterator>(iterator(begin, end), iterator(end, end))
        {}
    };

    template<LZ_CONCEPT_ITERATOR... Iterators>
    CartesianProduct<Iterators...> cartesianRange(std::tuple<Iterators...> begin, std::tuple<Iterators...> end) {
        return CartesianProduct<Iterators...>(std::move(begin), std::move(end));
    }

    template<LZ_CONCEPT_ITERABLE... Iterables>
    CartesianProduct<internal::IterTypeFromIterable<Iterables>...> cartesian(Iterables&&... iterables) {
        return cartesianRange(std::make_tuple(internal::begin(std::forward<Iterables>(iterables))...),
                              std::make_tuple(internal::end(std::forward<Iterables>(iterables))...));
    }
}

#endif // LZ_CARTESIAN_PRODUCT_HPP
