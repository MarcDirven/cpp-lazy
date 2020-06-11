#pragma once

#include <iterator>


namespace lz { namespace detail {
    template<class IntType>
    class ConstRangeIterator {
        IntType _begin{};
        IntType _end{};
        IntType _step{};

    public:
        using iterator_category = std::bidirectional_iterator_tag;
        using value_type = IntType;
        using difference_type = IntType;
        using pointer = IntType;
        using reference = IntType;

        ConstRangeIterator(IntType begin, IntType end, IntType step) :
            _begin(begin),
            _end(end),
            _step(step) {
        }

        reference operator*() const {
            return _begin;
        }

        pointer operator->() const {
            return _begin;
        }

        bool operator!=(const ConstRangeIterator& other) const {
            if (_step < 0) {
                return _begin > other._end;
            }
            return _begin < other._end;
        }

        ConstRangeIterator& operator++() {
            _begin += _step;
            return *this;
        }

        ConstRangeIterator& operator--() {
            _begin -= _step;
            return *this;
        }

        ConstRangeIterator& operator+=(const difference_type offset) {
            _begin += (offset * _step);
            return *this;
        }

        ConstRangeIterator operator+(const difference_type offset) const {
            auto* tmp = *this;
            return tmp += offset;
        }

        ConstRangeIterator& operator-=(const difference_type offset) {
            _begin -= (offset * _step);
            return *this;
        }

        ConstRangeIterator operator-(const difference_type other) const {
            auto tmp = *this;
            return tmp -= other;
        }
    };

    template<class IntType>
    class RangeIterator final : public ConstRangeIterator<IntType> {
    public:
        RangeIterator(IntType begin, IntType end, IntType step) :
            ConstRangeIterator<IntType>(begin, end, step) {
        }
    };
}}
