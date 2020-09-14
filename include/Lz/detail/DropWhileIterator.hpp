#pragma once

#include "LzTools.hpp"

namespace lz { namespace detail {
    template<class Iterator, class Function>
    class DropWhileIterator {
        using IterTraits = std::iterator_traits<Iterator>;

    public:
        using iterator_category = typename IterTraits::iterator_category;
        using value_type = typename IterTraits::value_type;
        using difference_type = typename IterTraits::difference_type;
        using reference = typename IterTraits::reference;
        using pointer = typename IterTraits::pointer;

    private:
        Iterator _iterator{};

    public:
        DropWhileIterator(const Iterator begin, const Iterator end, const Function& function) :
            _iterator(begin != end ? std::find_if(begin, end, [&function](const value_type& value) {
                return !function(value);
            }) : end) {}

        DropWhileIterator() = default;

        reference operator*() const {
            return *_iterator;
        }

        pointer operator->() const {
            return &*_iterator;
        }

        DropWhileIterator& operator++() {
            ++_iterator;
            return *this;
        }

        DropWhileIterator operator++(int) {
            DropWhileIterator tmp(*this);
            ++*this;
            return tmp;
        }

        DropWhileIterator& operator--() {
            --_iterator;
            return *this;
        }

        DropWhileIterator operator--(int) {
            DropWhileIterator tmp(*this);
            --*this;
            return tmp;
        }

        bool operator!=(const DropWhileIterator& other) const {
            return _iterator != other._iterator;
        }

        bool operator==(const DropWhileIterator& other) const {
            return !(*this != other);
        }

        DropWhileIterator& operator+=(const difference_type offset) {
            _iterator += offset;
            return *this;
        }

        DropWhileIterator& operator-=(const difference_type offset) {
            _iterator -= offset;
            return *this;
        }

        DropWhileIterator operator+(const difference_type offset) const {
            DropWhileIterator tmp(*this);
            tmp += offset;
            return tmp;
        }

        DropWhileIterator operator-(const difference_type offset) const {
            auto tmp(*this);
            tmp -= offset;
            return tmp;
        }

        difference_type operator-(const DropWhileIterator& other) const {
            return _iterator - other._iterator;
        }

        reference operator[](const difference_type offset) const {
            return *(*this + offset);
        }

        bool operator<(const DropWhileIterator& other) const {
            return _iterator < other._iterator;
        }

        bool operator>(const DropWhileIterator& other) const {
            return other < *this;
        }

        bool operator<=(const DropWhileIterator& other) const {
            return !(other < *this);
        }

        bool operator>=(const DropWhileIterator& other) const {
            return !(*this < other);
        }
    };
}}