#pragma once


#include <vector>
#include <array>

#include "Tools.hpp"


namespace detail {
    template<class Iterator, class IntType>
    class EnumerateIterator {
    public:
        using iterator_category = std::bidirectional_iterator_tag;
        using value_type = std::pair<IntType, typename std::iterator_traits<Iterator>::value_type>;
        using difference_type = std::ptrdiff_t;
        using pointer = std::pair<IntType, typename std::iterator_traits<Iterator>::pointer>;
        using reference = std::pair<IntType, typename std::iterator_traits<Iterator>::reference>;

    private:
        IntType _index;
        Iterator _iterator;

    public:
        EnumerateIterator(IntType start, Iterator iterator) :
            _index(start),
            _iterator(iterator) {
        }

        reference operator*() const {
            return reference(_index, *_iterator);
        }

        pointer operator->() const {
            return pointer(_index, _iterator.operator->());
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
            _index -= offset;
            _iterator -= offset;
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
    template<class Iterator, class IntType>
    class EnumerateObject {
    public:
        using value_type = std::pair<IntType, typename std::iterator_traits<Iterator>::value_type>;
        using size_type = size_t;
        using difference_type = std::ptrdiff_t;
        using pointer = std::pair<IntType, typename std::iterator_traits<Iterator>::pointer>;
        using const_pointer = std::pair<IntType, const typename std::iterator_traits<Iterator>::pointer>;
        using reference = std::pair<IntType, typename std::iterator_traits<Iterator>::reference>;
        using const_reference = std::pair<IntType, const typename std::iterator_traits<Iterator>::reference>;

        using iterator = detail::EnumerateIterator<Iterator, IntType>;
        using const_iterator = iterator;

    private:
        iterator _begin{};
        iterator _end{};

    public:
        EnumerateObject(Iterator begin, Iterator end, IntType start) :
            _begin(start, begin),
            _end(start, end) {
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

    template<class IntType = int, class Iterator>
    auto enumeraterange(Iterator begin, Iterator end, IntType start = 0) {
        return EnumerateObject<Iterator, IntType>(begin, end, start);
    }

    template<class IntType = int, class Container>
    auto enumerate(Container&& container, IntType start = 0) {
        return enumeraterange(container.begin(), container.end(), start);
    }
}