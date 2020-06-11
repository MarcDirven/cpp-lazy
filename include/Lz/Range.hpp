#pragma once

#include <vector>

#include <Lz/detail/LzTools.hpp>
#include <Lz/detail/RangeIterator.hpp>


namespace lz {
    template<class IntType>
    class Range {
        IntType _begin{};
        IntType _end{};
        IntType _step{};

    public:
        using iterator = detail::RangeIterator<IntType>;
        using const_iterator = detail::ConstRangeIterator<IntType>;

        using reverse_iterator = std::reverse_iterator<iterator>;
        using const_reverse_iterator = std::reverse_iterator<const_iterator>;

        using value_type = typename iterator::value_type;

        Range(IntType start, IntType end, IntType step) :
            _begin(start),
            _end(end),
            _step(step) {
        }

        iterator begin() {
            return iterator(_begin, _end, _step);
        }

        iterator end() {
            return iterator(_begin, _end, _step);
        }

        const_iterator begin() const {
            return const_iterator(_begin, _end, _step);
        }

        const_iterator end() const {
            return const_iterator(_begin, _end, _step);
        }

        reverse_iterator rbegin() {
            return reverse_iterator(begin());
        }

        reverse_iterator rend() {
            return reverse_iterator(end());
        }

        const_reverse_iterator crbegin() const {
            return const_reverse_iterator(begin());
        }

        const_reverse_iterator crend() const {
            return const_reverse_iterator(end());
        }

        template<template<typename, typename...> class Container, typename... Args>
        Container<value_type, Args...> to() const {
            return Container<value_type, Args...>(begin(), end());
        }

        std::vector<value_type> toVector() const {
            return to<std::vector>();
        }

        template<size_t N>
        std::array<value_type, N> toArray() const {
            std::array<value_type, N> container;
            detail::fillContainer(begin(), container);
            return container;
        }
    };

    template<class IntType = int>
    Range<IntType> range(const IntType start, const IntType end, IntType step = 1) {
        static_assert(std::is_arithmetic<IntType>::value, "type must be of type arithmetic");
        return Range<IntType>(start, end, step);
    }

    template<class IntType = int>
    Range<IntType> range(const IntType end) {
        return range(0, end, 1);
    }
}