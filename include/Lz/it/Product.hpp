#pragma once

#include <cstddef>
#include <tuple>
#include <Lz/it/detail/ProductIterator.hpp>

namespace lz { namespace it {
    template<size_t Repeat, class Iterator>
    class Product {
    public:
        using iterator = detail::ProductIterator<Repeat, Iterator>;
        using value_type = typename iterator::value_type;

    private:
        iterator _begin;
        iterator _end;

    public:
        Product(Iterator begin, Iterator end)
    };

    template<size_t Repeat, class Iterator>
    auto productwise(Iterator begin, Iterator end) {
        return Product<Repeat, Iterator>(begin, end);
    }
}}