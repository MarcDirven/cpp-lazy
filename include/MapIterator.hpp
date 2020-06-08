#pragma once

#include <functional>
#include <type_traits>


namespace detail {
    template<class Iterator, class Function>
    class MapIterator {
    private:
        Iterator _iterator;
        Function _function;

    public:
        using value_type = decltype(_function(*_iterator));
        using iterator_category = std::bidirectional_iterator_tag;
        using difference_type = std::ptrdiff_t;
        using reference = value_type;
        using pointer = value_type;

        MapIterator(Iterator iterator, Function function):
            _iterator(iterator),
            _function(function)
        {
        }

        value_type operator*() const {
            return _function(*_iterator);
        }

        bool operator!=(const MapIterator& other) const {
            return _iterator != other._iterator;
        }

        bool operator==(const MapIterator& other) const {
            return !(*this != other);
        }

        MapIterator& operator++() {
            ++_iterator;
            return *this;
        }

        MapIterator& operator--() {
            --_iterator;
            return *this;
        }

        MapIterator& operator+=(const difference_type offset) {
            _iterator += offset;
            return *this;
        }

        MapIterator& operator-=(const difference_type offset) {
            _iterator -= offset;
            return *this;
        }

        MapIterator operator+(const difference_type offset) {
            auto tmp(*this);
            tmp += offset;
            return tmp;
        }

        MapIterator operator-(const difference_type offset) {
            auto tmp(*this);
            tmp -= offset;
            return tmp;
        }
    };
}

namespace lz {
    template<class Iterator, class Function>
    class MapObject {
    public:
        using iterator = detail::MapIterator<Iterator, Function>;
        using const_iterator = iterator;
        using value_type = typename iterator::value_type;
        using size_type = size_t;
        using difference_type = ptrdiff_t;
        using reference = value_type;
        using const_reference = value_type;
        using pointer = std::remove_reference<value_type>*;
        using const_pointer = value_type;

    private:
        iterator _begin{};
        iterator _end{};

    public:
        MapObject(Iterator begin, Iterator end, Function function) :
	        _begin(begin, function),
	        _end(end, function)
        {
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
            return to<std::vector>();
        }
    };

    template<class Iterator, class Function>
    auto maprange(Iterator begin, Iterator end, Function f) {
        return MapObject<Iterator, Function>(begin, end, f);
    }

    template<class Container, class Function>
    auto map(Container&& container, Function function) {
        return maprange(container.begin(), container.end(), function);
    }
}
