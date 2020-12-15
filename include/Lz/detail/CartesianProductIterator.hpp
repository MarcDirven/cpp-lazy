#pragma once

#ifndef LZ_CARTESIAN_PRODUCT_ITERATOR_HPP
#define LZ_CARTESIAN_PRODUCT_ITERATOR_HPP

#include "Lz/detail/LzTools.hpp"


namespace lz { namespace internal {
    // Edited version of https://github.com/mirandaconrado/product-iterator
    template<class... Iterators>
    class CartesianProductIterator {
    public:
        using value_type = std::tuple<typename std::iterator_traits<Iterators>::value_type...>;
        using reference = std::tuple<typename std::iterator_traits<Iterators>::reference...>;
        using pointer = std::tuple<typename std::iterator_traits<Iterators>::pointer...>;
        using iterator_category = std::input_iterator_tag;
        using difference_type = std::ptrdiff_t;

    private:
        std::tuple<Iterators...> _begin{};
        std::tuple<Iterators...> _iterator{};
        std::tuple<Iterators...> _end{};

        template<std::size_t I>
        EnableIf<I == 0> next() const {
        }

        template<std::size_t I>
        EnableIf<(I > 0)> next() {
            auto& prev = std::get<I - 1>(_iterator);
            ++prev;

            if (prev == std::get<I - 1>(_end)) {
                if (I == 1) {
                    return;
                }

                prev = std::get<I - 1>(_begin);
                next<I - 1>();
            }
        }

        template<std::size_t... Is>
        reference dereference(IndexSequence<Is...>) {
            return reference(*std::get<Is>(_iterator)...);
        }

    public:
        CartesianProductIterator() = default;

        CartesianProductIterator(std::tuple<Iterators...> begin, std::tuple<Iterators...> end):
            _begin(begin),
            _iterator(begin),
            _end(end)
        {}

        reference operator*() {
            return dereference(MakeIndexSequence<sizeof...(Iterators)>());
        }

        CartesianProductIterator& operator++() {
            if (std::get<0>(_iterator) == std::get<0>(_end)) {
                return *this;
            }

            next<sizeof...(Iterators)>();
            return *this;
        }

        CartesianProductIterator operator++(int) {
            CartesianProductIterator tmp(*this);
            ++*this;
            return tmp;
        }

        friend bool operator==(const CartesianProductIterator& lhs, const CartesianProductIterator& rhs) {
            return std::get<0>(lhs._iterator) == std::get<0>(rhs._iterator);
        }

        friend bool operator!=(const CartesianProductIterator& lhs, const CartesianProductIterator& rhs) {
            return !(lhs == rhs);
        }
    };
}}
#endif // LZ_CARTESIAN_PRODUCT_ITERATOR_HPP
