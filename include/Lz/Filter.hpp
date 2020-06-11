#pragma once

#include <type_traits>
#include <algorithm>
#include <vector>

#include <Lz/detail/LzTools.hpp>
#include <Lz/detail/FilterIterator.hpp>


namespace lz {
    template<class Iterator, class Function>
    class Filter {
    public:
        using iterator = detail::FilterIterator<Iterator, Function>;
        using const_iterator = iterator;

        using value_type = typename iterator::value_type;

    private:
        iterator _begin{};
        iterator _end{};

    public:
        Filter(Iterator begin, Iterator end, Function function) :
            _begin(begin, end, function),
            _end(begin, end, function) {
        }

        iterator begin() const {
            return _begin;
        }

        iterator end() const {
            return _end;
        }

        template<template<class, class...> class ContainerType, class... Args>
        ContainerType<value_type, Args...> to() const {
            return ContainerType<value_type, Args...>(begin(), end());
        }

        std::vector<value_type> toVector() const {
            return to<std::vector>();
        }

        template<size_t N>
        std::array<value_type, N> toArray() const {
            return detail::fillArray<N>(begin());
        }
    };

    template<class Iterator, class Function>
    auto filterrange(Iterator begin, Iterator end, Function predicate) {
        return Filter<Iterator, Function>(begin, end, predicate);
    }

    template<class Container, class Function>
    auto filter(Container&& container, Function predicate) {
        return filterrange(container.begin(), container.end(), predicate);
    }
}