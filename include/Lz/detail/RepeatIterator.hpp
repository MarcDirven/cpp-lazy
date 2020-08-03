#pragma once

#include <iterator>
#include <limits>


namespace lz { namespace detail {
    template<class T>
    struct RepeatIteratorHelper {
        mutable T toRepeat{};
        bool isWhileTrueLoop{};
    };

    template<class T>
    class RepeatIterator {
        const RepeatIteratorHelper<T>* _iterHelper = RepeatIterator<T>();
        size_t _iterator{};

    public:
        using iterator_category = std::random_access_iterator_tag;
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using pointer = T*;
        using reference = T&;

        explicit RepeatIterator(const RepeatIteratorHelper<T>* iteratorHelper, const size_t start) :
            _iterHelper{iteratorHelper},
            _iterator(start) {
        }

        reference operator*() const {
            return _iterHelper->toRepeat;
        }

        pointer operator->() const {
            return &_iterHelper->toRepeat;
        }

        RepeatIterator& operator++() {
            if (!_iterHelper->isWhileTrueLoop) {
                ++_iterator;
            }
            return *this;
        }

        RepeatIterator operator++(int) {
            RepeatIterator tmp(*this);
            ++*this;
            return tmp;
        }

        RepeatIterator& operator--() {
            if (!_iterHelper->isWhileTrueLoop) {
                --_iterator;
            }
            return *this;
        }

        RepeatIterator operator--(int) {
            RepeatIterator tmp(*this);
            --*this;
            return tmp;
        }

        RepeatIterator& operator+=(const difference_type offset) {
            if (!_iterHelper->isWhileTrueLoop) {
                _iterator += offset;
            }
            return *this;
        }

        RepeatIterator& operator-=(const difference_type offset) {
            if (!_iterHelper->isWhileTrueLoop) {
                _iterator -= offset;
            }
            return *this;
        }

        RepeatIterator operator+(const difference_type offset) const {
            RepeatIterator tmp(*this);
            tmp += offset;
            return tmp;
        }

        RepeatIterator operator-(const difference_type offset) const {
            RepeatIterator tmp(*this);
            tmp -= offset;
            return tmp;
        }

        difference_type operator-(const RepeatIterator& other) const {
            return _iterator - other._iterator;
        }

        reference operator[](const difference_type offset) const {
            return *(*this + offset);
        }

        bool operator!=(const RepeatIterator& other) const {
            return _iterator != other._iterator;
        }

        bool operator==(const RepeatIterator& other) const {
            return !(*this != other);
        }

        bool operator<(const RepeatIterator& other) const {
            return _iterator < other._iterator;
        }

        bool operator>(const RepeatIterator& other) const {
            return other < *this;
        }

        bool operator<=(const RepeatIterator& other) const {
            return !(other < *this);
        }

        bool operator>=(const RepeatIterator& other) const {
            return !(*this < other);
        }
    };
}}