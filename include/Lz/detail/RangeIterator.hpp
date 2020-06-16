#pragma once

#include <iterator>


namespace lz { namespace detail {
    template<class IntType>
    class ConstRangeIterator {
        IntType _iterator{};
        IntType _end{};
        IntType _step{};

    public:
        using iterator_category = std::random_access_iterator_tag;
        using value_type = IntType;
        using difference_type = IntType;
        using pointer = IntType*;
        using reference = IntType;

        ConstRangeIterator(IntType begin, IntType end, IntType step) :
            _iterator(begin),
            _end(end),
            _step(step) {
        }

        value_type operator*() const {
            return _iterator;
        }

        pointer operator->() const {
            return &_iterator;
        }

        ConstRangeIterator& operator++() {
            _iterator += _step;
            return *this;
        }

        ConstRangeIterator& operator--() {
            _iterator -= _step;
            return *this;
        }

        ConstRangeIterator& operator+=(const difference_type offset) {
            _iterator += (offset * _step);
            return *this;
        }

        ConstRangeIterator operator+(const difference_type offset) const {
            auto tmp = *this;
            return tmp += offset;
        }

        ConstRangeIterator& operator-=(const difference_type offset) {
            _iterator -= (offset * _step);
            return *this;
        }

        ConstRangeIterator operator-(const difference_type other) const {
            auto tmp = *this;
            return tmp -= other;
        }

        difference_type operator-(const ConstRangeIterator& other) const {
            return other._end - _iterator;
        }

        value_type operator[](const difference_type offset) const {
            return *(*this + offset);
        }

        bool operator!=(const ConstRangeIterator& other) const {
            if (_step < 0) {
                return _iterator > other._end;
            }
            return _iterator < other._end;
        }

        bool operator==(const ConstRangeIterator& other) const {
            return !(*this != other);
        }

        bool operator<(const ConstRangeIterator& other) const {
            return _iterator < other._end;
        }

        bool operator>(const ConstRangeIterator& other) const {
            return other < *this;
        }

        bool operator<=(const ConstRangeIterator& other) const {
            return !(other < *this);
        }

        bool operator>=(const ConstRangeIterator& other) const {
            return !(*this < other);
        }
    };
}}
