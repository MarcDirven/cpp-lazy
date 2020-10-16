#pragma once

#ifndef LZ_MAP_ITERATOR_HPP
#define LZ_MAP_ITERATOR_HPP


#include "LzTools.hpp"

namespace lz { namespace internal {
    template<LZ_CONCEPT_ITERATOR Iterator, class Function>
    class MapIterator {
        Iterator _iterator{};
        Function _function;

    public:
        using value_type = FunctionReturnType<Function, decltype(*_iterator)>;
        using iterator_category = typename std::iterator_traits<Iterator>::iterator_category;
        using difference_type = std::ptrdiff_t;
        using reference = value_type;
        using pointer = FakePointerProxy<reference>;

        MapIterator(const Iterator iterator, Function function) :  // NOLINT(modernize-pass-by-value)
            _iterator(iterator),
            _function(std::move(function)) {
        }

        MapIterator() = default;

        value_type operator*() const {
            return _function(*_iterator);
        }

        FakePointerProxy<reference> operator->() const {
            return FakePointerProxy<decltype(**this)>(**this);
        }

        MapIterator& operator++() {
            ++_iterator;
            return *this;
        }

        MapIterator operator++(int) {
            MapIterator tmp(*this);
            ++*this;
            return tmp;
        }

        MapIterator& operator--() {
            --_iterator;
            return *this;
        }

        MapIterator operator--(int) {
            MapIterator tmp(*this);
            --*this;
            return tmp;
        }

        MapIterator& operator+=(const difference_type offset) {
            _iterator += offset;
            return *this;
        }

        MapIterator& operator-=(const difference_type offset) {
            _iterator -= offset;
            return *this;
        }

        MapIterator operator+(const difference_type offset) const {
            MapIterator tmp(*this);
            tmp += offset;
            return tmp;
        }

        MapIterator operator-(const difference_type offset) const {
            MapIterator tmp(*this);
            tmp -= offset;
            return tmp;
        }

        difference_type operator-(const MapIterator& other) const {
            return _iterator - other._iterator;
        }

        reference operator[](const difference_type offset) const {
            return *(*this + offset);
        }

        bool operator==(const MapIterator& other) const {
            return !(*this != other);
        }

        bool operator!=(const MapIterator& other) const {
            return _iterator != other._iterator;
        }

        bool operator<(const MapIterator& other) const {
            return _iterator < other._iterator;
        }

        bool operator>(const MapIterator& other) const {
            return other < *this;
        }

        bool operator<=(const MapIterator& other) const {
            return !(other < *this);
        }

        bool operator>=(const MapIterator& other) const {
            return !(*this < other);
        }
    };
}} // end lz::internal

#endif