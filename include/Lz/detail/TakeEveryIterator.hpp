#pragma once

#include <iterator>


namespace lz {
    template<class>
    class TakeEvery;

    namespace detail {

    template<class Iterator>
    class TakeEveryIterator {
        Iterator _iterator{};
        Iterator _end{};
        size_t _offset{};
        size_t _current{};
        size_t _distance{};


        friend class ::lz::TakeEvery<Iterator>;

    public:
        using value_type = typename std::iterator_traits<Iterator>::value_type;
        using iterator_category = typename std::iterator_traits<Iterator>::iterator_category;
        using difference_type = typename std::iterator_traits<Iterator>::difference_type;
        using reference = typename std::iterator_traits<Iterator>::reference;
        using pointer = typename std::iterator_traits<Iterator>::pointer;

        TakeEveryIterator(const Iterator iterator, const Iterator end, const size_t offset, const size_t distance) :
            _iterator(iterator),
            _end(end),
            _offset(offset),
            _current(iterator == end ? distance : 0),
            _distance(distance) {
        }

        reference operator*() const {
            return *_iterator;
        }

        pointer operator->() const {
            return _iterator.operator->();
        }

        TakeEveryIterator& operator++() {
            *this += 1;
            return *this;
        }

        TakeEveryIterator operator++(int) {
            auto tmp = *this;
            ++*this;
            return tmp;
        }

        TakeEveryIterator& operator--() {
            *this -= 1;
            return *this;
        }

        TakeEveryIterator operator--(int) {
            auto tmp(*this);
            --*this;
            return tmp;
        }

        TakeEveryIterator& operator+=(const difference_type offset) {
            auto total = _offset * offset;

            if (_current + total >= _distance) {
                _iterator = _end;
            }
            else {
                _iterator = std::next(_iterator, total);
                _current += total;
            }
            return *this;
        }

        TakeEveryIterator& operator-=(const difference_type offset) {
            auto total = _offset * offset;

            if (static_cast<ptrdiff_t>(_current - total) < 0) {
                _iterator = _end;
            }
            else {
                _iterator = std::prev(_iterator, total);
                _current -= total;
            }
            return *this;
        }

        TakeEveryIterator operator+(const difference_type offset) const {
            auto tmp(*this);
            tmp += offset;
            return tmp;
        }

        TakeEveryIterator operator-(const difference_type offset) const {
            auto tmp(*this);
            tmp -= offset;
            return tmp;
        }

        difference_type operator-(const TakeEveryIterator& other) const {
            auto distance = std::distance(other._iterator, _iterator);
            auto diffOffset = static_cast<difference_type>(distance / _offset);
            return _offset % 2 == 0 ? diffOffset : diffOffset + 1;
        }

        reference operator[](const difference_type offset) const {
            return *(*this + offset);
        }

        bool operator==(const TakeEveryIterator& other) const {
            return !(*this != other);
        }

        bool operator!=(const TakeEveryIterator& other) const {
            return _iterator < other._iterator;
        }

        bool operator<(const TakeEveryIterator& other) const {
            return _iterator < other._iterator;
        }

        bool operator>(const TakeEveryIterator& other) const {
            return other < *this;
        }

        bool operator<=(const TakeEveryIterator& other) const {
            return !(other < *this);
        }

        bool operator>=(const TakeEveryIterator& other) const {
            return !(*this < other);
        }
    };
}}