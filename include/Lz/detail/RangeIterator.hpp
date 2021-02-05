#pragma once

#ifndef LZ_RANGE_ITERATOR_HPP
#define LZ_RANGE_ITERATOR_HPP

#include <iterator>


namespace lz { namespace internal {
    template<LZ_CONCEPT_ARITHMETIC Arithmetic>
    class RangeIterator {
        Arithmetic _iterator{};
        Arithmetic _step{};

    public:
        using iterator_category = std::random_access_iterator_tag;
        using value_type = Arithmetic;
        using difference_type = std::ptrdiff_t;
        using pointer = const Arithmetic*;
        using reference = Arithmetic;

        RangeIterator(const Arithmetic iterator, const Arithmetic step) :
            _iterator(iterator),
            _step(step) {
        }

        RangeIterator() = default;

        value_type operator*() const {
            return _iterator;
        }

        pointer operator->() const {
            return &_iterator;
        }

        RangeIterator& operator++() {
            _iterator += _step;
            return *this;
        }

        RangeIterator operator++(int) {
            RangeIterator tmp(*this);
            ++*this;
            return tmp;
        }

        RangeIterator& operator--() {
            _iterator -= _step;
            return *this;
        }

        RangeIterator operator--(int) {
            RangeIterator tmp(*this);
            --*this;
            return tmp;
        }

        RangeIterator& operator+=(const difference_type offset) {
            _iterator += (static_cast<Arithmetic>(offset) * _step);
            return *this;
        }

        RangeIterator operator+(const difference_type offset) const {
            RangeIterator tmp(*this);
            return tmp += offset;
        }

        RangeIterator& operator-=(const difference_type offset) {
            _iterator -= (static_cast<Arithmetic>(offset) * _step);
            return *this;
        }

        RangeIterator operator-(const difference_type other) const {
            RangeIterator tmp = *this;
            return tmp -= other;
        }

        difference_type operator-(const RangeIterator& other) const {
            difference_type distance = _iterator - other._iterator;
            return static_cast<difference_type>(distance / _step);
        }

        value_type operator[](const difference_type offset) const {
            return *(*this + offset);
        }

        friend bool operator!=(const RangeIterator& a, const RangeIterator& b) {
            if (a._step < 0) {
                return a._iterator > b._iterator;
            }
            return a._iterator < b._iterator;
        }

        friend bool operator==(const RangeIterator& a, const RangeIterator& b) {
            return !(a != b); // NOLINT
        }

        friend bool operator<(const RangeIterator& a, const RangeIterator& b) {
            return a._iterator < b._iterator;
        }

        friend bool operator>(const RangeIterator& a, const RangeIterator& b) {
            return b < a;
        }

        friend bool operator<=(const RangeIterator& a, const RangeIterator& b) {
            return !(b < a); // NOLINT
        }

        friend bool operator>=(const RangeIterator& a, const RangeIterator& b) {
            return !(a < b); // NOLINT
        }
    };
}}

#endif