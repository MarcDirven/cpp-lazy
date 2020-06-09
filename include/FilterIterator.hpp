#pragma once

#include "Tools.hpp"

#include <type_traits>
#include <algorithm>
#include <vector>

namespace detail {
    template<class Iterator, class Function>
    class FilterIterator {
    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = typename  std::remove_reference<decltype(*std::declval<Iterator>())>::type;
        using difference_type = std::ptrdiff_t ;
        using pointer = value_type*;
        using reference = value_type&;

    private:
        Iterator _begin{};
        Iterator _end{};
        Function _function{};

    public:
        FilterIterator(Iterator begin, Iterator end, Function function) :
            _begin(begin),
            _end(end),
            _function(function) {
            _begin = std::find_if(_begin, _end, _function);
        }

        reference operator*() const {
            return *_begin;
        }

        pointer operator->() const {
            return _begin.operator->();
        }

        bool operator!=(const FilterIterator& other) const {
            return _begin != other._end;
        }

        bool operator==(const FilterIterator& other) const {
            return !(*this != other);
        }

        FilterIterator& operator++() {
            if (_begin != _end) {
                _begin = std::find_if(_begin + 1, _end, _function);
            }
            return *this;
        }

        FilterIterator& operator+=(const difference_type offset) {
            for (difference_type i = 0; i < offset; ++i, ++*this) {}
            return *this;
        }

        FilterIterator operator+(const difference_type other) const {
            auto tmp = *this;
            tmp += other;
            return tmp;
        }
    };
}

namespace lz {
    template<class Iterator, class Function>
    class FilterObject {
    public:
        using iterator = detail::FilterIterator<Iterator, Function>;
        using const_iterator = iterator;

        using value_type = typename iterator::value_type;
        using size_type = size_t;
        using difference_type = ptrdiff_t;
        using reference = value_type;
        using const_reference = value_type;
        using pointer = value_type;
        using const_pointer = value_type;

    private:
        iterator _begin{};
        iterator _end{};

    public:
        FilterObject(Iterator begin, Iterator end, Function function) :
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
            std::array<value_type, N> container{};
            detail::fillContainer(begin(), container);
            return container;
        }
    };

    template<class Iterator, class Function>
    auto filterrange(Iterator begin, Iterator end, Function predicate) {
        return FilterObject<Iterator, Function>(begin, end, predicate);
    }

    template<class Container, class Function>
    auto filter(Container&& container, Function predicate) {
        return filterrange(container.begin(), container.end(), predicate);
    }
}