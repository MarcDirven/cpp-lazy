#pragma once

#ifndef LZ_TAKE_EVERY_ITERATOR_HPP
#define LZ_TAKE_EVERY_ITERATOR_HPP

#include <iterator>
#include "LzTools.hpp"


namespace lz {
    template<LZ_CONCEPT_ITERATOR>
    class TakeEvery;

    namespace detail {

    template<LZ_CONCEPT_ITERATOR Iterator>
    class TakeEveryIterator {
        Iterator _iterator{};
        Iterator _end{};
        size_t _offset{};
        size_t _current{};
        size_t _distance{};


        friend class TakeEvery<Iterator>;
        using IterTraits = std::iterator_traits<Iterator>;

    public:
        using value_type = typename IterTraits::value_type;
        using iterator_category = typename IterTraits::iterator_category;
        using difference_type = typename IterTraits::difference_type;
        using reference = typename IterTraits::reference;
        using pointer = typename IterTraits::pointer;

        TakeEveryIterator(const Iterator iterator, const Iterator end, const size_t offset, const size_t distance) :
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
            auto total = static_cast<size_t>(_offset * offset);

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

            if (static_cast<std::ptrdiff_t>(_current - total) < 0) {
                _iterator = _end;
            }
            else {
                _iterator = std::prev(_iterator, total);
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

#endif