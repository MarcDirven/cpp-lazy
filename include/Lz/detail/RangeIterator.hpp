#pragma once

#include <iterator>


namespace lz { namespace detail {
    template<class Arithmetic>
    class RangeIterator {
        Arithmetic _iterator{};
        Arithmetic _step{};

    public:
        using iterator_category = std::random_access_iterator_tag;
        using value_type = Arithmetic;
        using difference_type = Arithmetic;
        using pointer = const Arithmetic*;
        using reference = Arithmetic;

        RangeIterator(const Arithmetic iterator, const Arithmetic step) :
            _iterator(iterator),
            _step(step) {
        }

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
            _iterator += (offset * _step);
            return *this;
        }

        RangeIterator operator+(const difference_type offset) const {
            RangeIterator tmp(*this);
            return tmp += offset;
        }

        RangeIterator& operator-=(const difference_type offset) {
            _iterator -= (offset * _step);
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

        bool operator!=(const RangeIterator& other) const {
            if (_step < 0) {
                return _iterator > other._iterator;
            }
            return _iterator < other._iterator;
        }

        bool operator==(const RangeIterator& other) const {
            return !(*this != other);
        }

        bool operator<(const RangeIterator& other) const {
            return _iterator < other._iterator;
        }

        bool operator>(const RangeIterator& other) const {
            return other < *this;
        }

        bool operator<=(const RangeIterator& other) const {
            return !(other < *this);
        }

        bool operator>=(const RangeIterator& other) const {
            return !(*this < other);
        }
    };
}}
