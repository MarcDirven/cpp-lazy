#pragma once

#include <vector>
#include <array>

#include <Lz/detail/TakeIterator.hpp>
#include <Lz/detail/LzTools.hpp>


namespace lz {
    template<class Iterator, class Function>
    class Take {
    public:
        using iterator = detail::TakeIterator<Iterator, Function>;
        using const_iterator = iterator;

        using value_type = typename std::iterator_traits<Iterator>::value_type;

    private:
        iterator _begin{};
        iterator _end{};

    public:
        Take(Iterator begin, Iterator end, Function function):
            _begin(begin, end, function),
            _end(begin, end, function)
        {
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
    auto takewhilerange(Iterator begin, Iterator end, Function predicate) {
        return Take<Iterator, Function>(begin, end, predicate);
    }

    template<class Container, class Function>
    auto takewhile(Container&& container, Function predicate) {
        return takewhilerange(container.begin(), container.end(), predicate);
    }

    template<class Iterator>
    auto takerange(Iterator begin, Iterator end) {
        return takewhilerange(begin, end, [](const auto&) { return true; });
    }

    template<class Container>
    auto take(Container&& container, const size_t amount) {
        auto begin = container.begin();
        return takerange(begin, begin + amount);
    }

    template<class Container>
    auto slice(Container&& container, const size_t from, const size_t to) {
        auto begin = container.begin();
        return takerange(begin + from, begin + to);
    }
}