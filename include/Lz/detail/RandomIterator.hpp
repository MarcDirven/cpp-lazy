#pragma once

#ifndef LZ_RANDOM_ITERATOR_HPP
#define LZ_RANDOM_ITERATOR_HPP

#include <random>

#include "LzTools.hpp"


namespace lz { namespace internal {
    template<LZ_CONCEPT_ARITHMETIC Arithmetic, class Distribution>
    class RandomIterator {
    public:
        using iterator_category = std::random_access_iterator_tag;
        using value_type = Arithmetic;
        using difference_type = std::ptrdiff_t;
        using pointer = FakePointerProxy<Arithmetic>;
        using reference = value_type;

    private:
        std::ptrdiff_t  _current{};
        Arithmetic _min{}, _max{};
        bool _isWhileTrueLoop{};

    public:
        explicit RandomIterator(const Arithmetic min, const Arithmetic max, const std::ptrdiff_t  current, const bool isWhileTrueLoop) :
            _current(current),
            _min(min),
            _max(max),
            _isWhileTrueLoop(isWhileTrueLoop) {
        }

        RandomIterator() = default;

        value_type operator*() const {
            static std::random_device randomEngine;
            static std::mt19937 generator(randomEngine());
            Distribution randomNumber(_min, _max);
            return randomNumber(generator);
        }

        pointer operator->() const {
            return FakePointerProxy<decltype(**this)>(**this);
        }

        RandomIterator& operator++() {
            if (!_isWhileTrueLoop) {
                ++_current;
            }
            return *this;
        }

        RandomIterator operator++(int) {
            RandomIterator tmp(*this);
            ++*this;
            return tmp;
        }

        RandomIterator& operator--() {
            if (!_isWhileTrueLoop) {
                --_current;
            }
            return *this;
        }

        RandomIterator operator--(int) {
            RandomIterator tmp(*this);
            --*this;
            return tmp;
        }

        RandomIterator& operator+=(const difference_type offset) {
            if (!_isWhileTrueLoop) {
                _current += offset;
            }
            return *this;
        }

        RandomIterator operator+(const difference_type offset) const {
            RandomIterator tmp(*this);
            tmp += offset;
            return tmp;
        }

        RandomIterator& operator-=(const difference_type offset) {
            if (!_isWhileTrueLoop) {
                _current -= offset;
            }
            return *this;
        }

        RandomIterator operator-(const difference_type offset) const {
            RandomIterator tmp(*this);
            tmp -= offset;
            return tmp;
        }

        difference_type operator-(const RandomIterator& other) const {
            return _current - other._current;
        }

        value_type operator[](const difference_type offset) const {
            return *(*this + offset);
        }

        friend bool operator!=(const RandomIterator& a, const RandomIterator& b) {
            return a._current != b._current;
        }

        friend bool operator==(const RandomIterator& a, const RandomIterator& b) {
            return !(a != b);
        }

        friend bool operator<(const RandomIterator& a, const RandomIterator& b) {
            return a._current < b._current;
        }

        friend bool operator>(const RandomIterator& a, const RandomIterator& b) {
            return b < a;
        }

        friend bool operator<=(const RandomIterator& a, const RandomIterator& b) {
            return !(b < a);
        }

        friend bool operator>=(const RandomIterator& a, const RandomIterator& b) {
            return !(a < b);
        }
    };
}}

#endif