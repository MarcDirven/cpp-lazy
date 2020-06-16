#pragma once

#include <Lz/detail/LzTools.hpp>
#include <Lz/detail/MapIterator.hpp>

#include <vector>
#include <array>


namespace lz {
    template<class Iterator, class Function>
    class Map {
    public:
        using iterator = detail::MapIterator<Iterator, Function>;
        using const_iterator = iterator;

        using value_type = typename iterator::value_type;

    private:
        iterator _begin{};
        iterator _end{};

    public:
        Map(Iterator begin, Iterator end, Function function) :
            _begin(begin, function),
            _end(end, function) {
        }

        iterator begin() const {
            return _begin;
        }

        iterator end() const {
            return _end;
        }

        template<template<typename, typename...> class Container, typename... Args>
        Container<value_type, Args...> to() const {
            return Container<value_type, Args...>(begin(), end());
        }

        std::vector<value_type> toVector() const {
            return toVector<std::allocator<value_type>>();
        }

        template<typename Allocator>
        std::vector<value_type, Allocator> toVector(const Allocator& alloc = Allocator()) const {
            return detail::makeVector<value_type>(begin(), end(), alloc);
        }

        template<size_t N>
        std::array<value_type, N> toArray() const {
            return detail::fillArray<value_type, N>(begin());
        }
    };

    template<class Iterator, class Function>
    auto maprange(Iterator begin, Iterator end, Function f) {
        return Map<Iterator, Function>(begin, end, f);
    }

    template<class Container, class Function>
    auto map(Container&& container, Function function) {
        return maprange(container.begin(), container.end(), function);
    }
}
