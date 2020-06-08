#pragma once


#include <vector>
#include "ZipIterator.hpp"

namespace detail {
    template<typename Container, typename IntType>
    class EnumerateIterator {
    public:
        using iterator_category = std::bidirectional_iterator_tag;
        using value_type = std::pair<IntType, decltype(*std::declval<Container>().begin())>;
        using difference_type = std::ptrdiff_t;
        using pointer = value_type;
        using reference = value_type;

    private:
        IntType _index;
        decltype(std::declval<Container>().begin()) _iterator;

    public:
        EnumerateIterator(IntType start, decltype(_iterator) iterator):
            _index(start),
            _iterator(iterator)
        {
        }

        reference operator*() const {
            return value_type(_index, *_iterator);
        }

        pointer operator->() const {
            return value_type(_index, _iterator.operator->());
        }

        bool operator==(const EnumerateIterator& other) const {
            return !(*this != other);
        }

        bool operator!=(const EnumerateIterator& other) const {
            return _iterator != other._iterator;
        }

        EnumerateIterator& operator++() {
            ++_index;
            ++_iterator;
            return *this;
        }

        EnumerateIterator& operator--() {
            --_index;
            --_iterator;
            return *this;
        }

        EnumerateIterator& operator+=(const difference_type offset) {
            _index += offset;
            _iterator += offset;
            return *this;
        }

        EnumerateIterator operator+(const difference_type offset) {
            auto tmp(*this);
            tmp += offset;
            return tmp;
        }

        EnumerateIterator& operator-=(const difference_type offset) {
            return *this;
        }

        EnumerateIterator operator-(const difference_type offset) {
            auto tmp(*this);
            tmp -= offset;
            return *this;
        }
    };
}

namespace lz {
    template<typename Container, typename IntType>
    class EnumerateObject {
    public:
        using value_type = std::pair<IntType, decltype(*std::declval<Container>().begin())>;
        using size_type = typename std::decay_t<Container>::size_type;
        using difference_type = typename std::decay_t<Container>::difference_type;
        using pointer = typename std::decay_t<Container>::pointer;
        using const_pointer = typename std::decay_t<Container>::const_pointer;
        using reference = typename std::decay_t<Container>::reference;
        using const_reference = typename std::decay_t<Container>::const_reference;

        using iterator = detail::EnumerateIterator<Container, IntType>;
        using const_iterator = iterator;

    private:
        iterator _begin{};
        iterator _end{};

    public:
        EnumerateObject(Container&& container, IntType start):
            _begin(start, container.begin()),
            _end(start, container.end())
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
            std::array<value_type, N> container{};
            detail::fillContainer(begin(), container);
            return container;
        }
    };

    template<class IntType = int, class Container>
    auto enumerate(Container&& container, IntType start = 0) {
        return EnumerateObject<Container, IntType>(container, start);
    }
}