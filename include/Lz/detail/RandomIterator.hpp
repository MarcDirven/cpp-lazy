#pragma once

#ifndef LZ_RANDOM_ITERATOR_HPP
#define LZ_RANDOM_ITERATOR_HPP

#include <random>

#include "LzTools.hpp"


namespace lz { namespace internal {
    template<LZ_CONCEPT_ARITHMETIC Arithmetic, class Distribution, class Generator>
    class RandomIterator {
    public:
        using iterator_category = std::input_iterator_tag;
        using value_type = Arithmetic;
        using difference_type = std::ptrdiff_t;
        using pointer = FakePointerProxy<Arithmetic>;
        using reference = value_type;
        using result_type = value_type;

    private:
        std::ptrdiff_t  _current{};
        Arithmetic _min{}, _max{};
        bool _isWhileTrueLoop{};

    public:
        explicit RandomIterator(const Arithmetic min, const Arithmetic max, const std::ptrdiff_t current, const bool isWhileTrueLoop) :
            _current(current),
            _min(min),
            _max(max),
            _isWhileTrueLoop(isWhileTrueLoop) {
        }

        RandomIterator() = default;

        value_type operator*() const {
            static std::random_device randomEngine;
            static Generator generator(randomEngine());
            Distribution randomNumber(_min, _max);
            return randomNumber(generator);
        }

        result_type(min)() {
        	return _min;
        }

		result_type(max)() {
			return _max;
		}

        value_type operator()() const {
        	return **this;
        }

        void setMin(value_type min) {
        	_min = min;
        }

        void setMax(value_type max) {
        	_max = max;
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

        friend bool operator!=(const RandomIterator& a, const RandomIterator& b) {
            return a._current != b._current;
        }

        friend bool operator==(const RandomIterator& a, const RandomIterator& b) {
            return !(a != b); // NOLINT
        }
    };
}}

#endif