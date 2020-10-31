#pragma once

#ifndef LZ_TAKE_EVERY_ITERATOR_HPP
#define LZ_TAKE_EVERY_ITERATOR_HPP

#include "LzTools.hpp"


namespace lz {
    template<LZ_CONCEPT_ITERATOR>
    class TakeEvery;

    namespace internal {

    template<LZ_CONCEPT_ITERATOR Iterator>
    class TakeEveryIterator {
        Iterator _iterator{};
        Iterator _end{};
        std::size_t _offset{};
        std::size_t _current{};
        std::size_t _distance{};

        using IterTraits = std::iterator_traits<Iterator>;

    public:
        using value_type = typename IterTraits::value_type;
        using iterator_category = typename IterTraits::iterator_category;
        using difference_type = typename IterTraits::difference_type;
        using reference = typename IterTraits::reference;
        using pointer = typename IterTraits::pointer;

        TakeEveryIterator(Iterator iterator, Iterator end, const std::size_t offset, const std::size_t distance) :
            _iterator(iterator),
            _end(end),
            _offset(offset),
            _current(iterator == end ? distance : 0),
            _distance(distance) {
        }

        TakeEveryIterator() = default;

        reference operator*() const {
            return *_iterator;
        }

        pointer operator->() const {
            return &*_iterator;
        }

        TakeEveryIterator& operator++() {
            *this += 1;
            return *this;
        }

        TakeEveryIterator operator++(int) {
            TakeEveryIterator tmp(*this);
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
            auto total = static_cast<std::size_t>(_offset * offset);

            if (_current + total >= _distance) {
                _iterator = _end;
            }
            else {
                std::advance(_iterator, total);
                _current += total;
            }
            return *this;
        }

        TakeEveryIterator& operator-=(const difference_type offset) {
            const auto total = _offset * offset;

            if (static_cast<std::ptrdiff_t>(_current - total) < 0) {
                _iterator = _end;
            }
            else {
                std::advance(_iterator, -static_cast<difference_type>(total));
                _current -= total;
            }
            return *this;
        }

        TakeEveryIterator operator+(const difference_type offset) const {
            TakeEveryIterator tmp(*this);
            tmp += offset;
            return tmp;
        }

        TakeEveryIterator operator-(const difference_type offset) const {
            TakeEveryIterator tmp(*this);
            tmp -= offset;
            return tmp;
        }

        difference_type operator-(const TakeEveryIterator& other) const {
            const difference_type distance = std::distance(other._iterator, _iterator);
            const auto diffOffset = static_cast<difference_type>(distance / _offset);
            return _offset % 2 == 0 ? diffOffset : diffOffset + 1;
        }

        reference operator[](const difference_type offset) const {
            return *(*this + offset);
        }

        friend bool operator==(const TakeEveryIterator& a, const TakeEveryIterator& b) {
            return !(a != b);
        }

        friend bool operator!=(const TakeEveryIterator& a, const TakeEveryIterator& b) {
            return std::distance(a._iterator, b._iterator) >= static_cast<difference_type>(a._offset);
        }

        friend bool operator<(const TakeEveryIterator& a, const TakeEveryIterator& b) {
            return a._iterator < b._iterator;
        }

        friend bool operator>(const TakeEveryIterator& a, const TakeEveryIterator& b) {
            return b < a;
        }

        friend bool operator<=(const TakeEveryIterator& a, const TakeEveryIterator& b) {
            return !(b < a);
        }

        friend bool operator>=(const TakeEveryIterator& a, const TakeEveryIterator& b) {
            return !(a < b);
        }
    };
}}

#endif