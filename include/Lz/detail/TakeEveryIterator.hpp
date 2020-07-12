#pragma once

#include <iterator>


namespace lz { namespace detail {
    template<class Iterator>
    class TakeEveryIterator {
        Iterator _iterator;
        size_t _offset;

    public:
        using value_type = typename std::iterator_traits<Iterator>::value_type;
        using iterator_category = std::random_access_iterator_tag;
        using difference_type = typename std::iterator_traits<Iterator>::difference_type;
        using reference = typename std::iterator_traits<Iterator>::reference;
        using pointer = typename std::iterator_traits<Iterator>::pointer;

        TakeEveryIterator(Iterator iterator, size_t offset) :
            _iterator(iterator),
            _offset(offset) {
        }

        reference operator*() const {
            return *_iterator;
        }

        pointer operator->() const {
            return _iterator.operator->();
        }

        TakeEveryIterator& operator++() {
            _iterator += _offset;
            return *this;
        }

        TakeEveryIterator operator++(int) {
            auto tmp = *this;
            ++*this;
            return tmp;
        }

        TakeEveryIterator& operator--() {
            _iterator -= _offset;
            return *this;
        }

        TakeEveryIterator operator--(int) {
            auto tmp(*this);
            --*this;
            return tmp;
        }

        TakeEveryIterator& operator+=(const difference_type offset) {
            _iterator += (_offset * offset);
            return *this;
        }

        TakeEveryIterator& operator-=(const difference_type offset) {
            _iterator -= (_offset * offset);
            return *this;
        }

        TakeEveryIterator operator+(const difference_type offset) {
            auto tmp(*this);
            tmp += offset;
            return tmp;
        }

        TakeEveryIterator operator-(const difference_type offset) {
            auto tmp(*this);
            tmp -= offset;
            return tmp;
        }

        difference_type operator-(const TakeEveryIterator& other) const {
            auto distance = std::distance(other._iterator, _iterator);
            auto diffOffset = static_cast<difference_type>(distance / _offset);
            return _offset % 2 == 0 ? diffOffset : diffOffset + 1;
        }

        reference operator[](const difference_type offset) {
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