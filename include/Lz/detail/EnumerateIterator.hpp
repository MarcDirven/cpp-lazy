#pragma once

#ifndef LZ_ENUMERATE_ITERATOR_HPP
#define LZ_ENUMERATE_ITERATOR_HPP


#include "LzTools.hpp"


namespace lz { namespace internal {
    template<LZ_CONCEPT_ITERATOR Iterator, LZ_CONCEPT_INTEGRAL Arithmetic>
    class EnumerateIterator {
        Arithmetic _index;
        Iterator _iterator;

        using IterTraits = std::iterator_traits<Iterator>;
    public:
        using iterator_category = typename IterTraits::iterator_category;
        using value_type = std::pair<Arithmetic, typename IterTraits::value_type>;
        using difference_type = typename IterTraits::difference_type;
        using reference = std::pair<Arithmetic, typename IterTraits::reference>;
        using pointer = FakePointerProxy<reference>;

        EnumerateIterator(const Arithmetic start, Iterator iterator) :
            _index(start),
            _iterator(std::move(iterator)) {
        }

        EnumerateIterator() = default;

        reference operator*() const {
            return reference(_index, *_iterator);
        }

        FakePointerProxy<reference> operator->() {
            return FakePointerProxy<decltype(**this)>(**this);
        }

        EnumerateIterator& operator++() {
            ++_index;
            ++_iterator;
            return *this;
        }

        EnumerateIterator operator++(int) {
            EnumerateIterator tmp = *this;
            ++*this;
            return tmp;
        }

        EnumerateIterator& operator--() {
            --_index;
            --_iterator;
            return *this;
        }

        EnumerateIterator operator--(int) {
            EnumerateIterator tmp(*this);
            --*this;
            return tmp;
        }

        EnumerateIterator& operator+=(const difference_type offset) {
            _index += static_cast<Arithmetic>(offset);
            _iterator += offset;
            return *this;
        }

        EnumerateIterator operator+(const difference_type offset) const {
            EnumerateIterator tmp(*this);
            tmp += offset;
            return tmp;
        }

        EnumerateIterator& operator-=(const difference_type offset) {
            _index -= static_cast<Arithmetic>(offset);
            _iterator -= offset;
            return *this;
        }

        EnumerateIterator operator-(const difference_type offset) const {
            EnumerateIterator tmp(*this);
            tmp -= offset;
            return tmp;
        }

        difference_type operator-(const EnumerateIterator& other) const {
            return _iterator - other._iterator;
        }

        reference operator[](const difference_type offset) const {
            return *(*this + offset);
        }

        friend bool operator==(const EnumerateIterator& a, const EnumerateIterator& b) {
            return !(a != b);
        }

        friend bool operator!=(const EnumerateIterator& a, const EnumerateIterator& b) {
            return a._iterator != b._iterator;
        }

        friend bool operator<(const EnumerateIterator& a, const EnumerateIterator& b) {
            return a._iterator < b._iterator;
        }

        friend bool operator>(const EnumerateIterator& a, const EnumerateIterator& b) {
            return b < a;
        }

        friend bool operator<=(const EnumerateIterator& a, const EnumerateIterator& b) {
            return !(b < a);
        }

        friend bool operator>=(const EnumerateIterator& a, const EnumerateIterator& b) {
            return !(a < b);
        }
    };
}}

#endif
