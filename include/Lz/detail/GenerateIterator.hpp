#pragma once

#ifndef LZ_GENERATE_ITERATOR_HPP
#define LZ_GENERATE_ITERATOR_HPP

#include "LzTools.hpp"
#include "FunctionContainer.hpp"


namespace lz { namespace internal {
    template<class GeneratorFunc>
    class GenerateIterator {
    public:
        using iterator_category = std::random_access_iterator_tag;
		using reference = FunctionReturnType<GeneratorFunc>;
        using value_type = Decay<reference>;
        using difference_type = std::ptrdiff_t;
        using pointer = FakePointerProxy<reference>;

    private:
        std::size_t _current{};
        mutable FunctionContainer<GeneratorFunc> _generator{};
        bool _isWhileTrueLoop{};

    public:
        GenerateIterator() = default;

        GenerateIterator(const std::size_t start, GeneratorFunc generatorFunc, const bool isWhileTrueLoop) :
            _current(static_cast<difference_type>(start)),
            _generator(std::move(generatorFunc)),
            _isWhileTrueLoop(isWhileTrueLoop)
        {}

        reference operator*() const {
            return _generator();
        }

		reference operator*() {
			return _generator();
		}

        pointer operator->() const {
            return FakePointerProxy<decltype(**this)>(**this);
        }

		pointer operator->() {
			return FakePointerProxy<decltype(**this)>(**this);
		}

        GenerateIterator& operator++() {
            if (!_isWhileTrueLoop) {
                ++_current;
            }
            return *this;
        }

        GenerateIterator operator++(int) {
            GenerateIterator tmp(*this);
            ++*this;
            return tmp;
        }

        GenerateIterator& operator--() {
            if (!_isWhileTrueLoop) {
                --_current;
            }
            return *this;
        }

        GenerateIterator operator--(int) {
            GenerateIterator tmp(*this);
            --*this;
            return tmp;
        }

        GenerateIterator& operator+=(const difference_type offset) {
            if (!_isWhileTrueLoop) {
                _current += offset;
            }
            return *this;
        }

        GenerateIterator& operator-=(const difference_type offset) {
            if (!_isWhileTrueLoop) {
                _current -= offset;
            }
            return *this;
        }

        GenerateIterator operator+(const difference_type offset) const {
            GenerateIterator tmp(*this);
            tmp += offset;
            return tmp;
        }

        GenerateIterator operator-(const difference_type offset) const {
            GenerateIterator tmp(*this);
            tmp -= offset;
            return tmp;
        }

        difference_type operator-(const GenerateIterator& other) const {
            return _current - other._current;
        }

        value_type operator[](const difference_type offset) const {
            return *(*this + offset);
        }

        friend bool operator==(const GenerateIterator& a, const GenerateIterator& b) {
            return !(a != b); // NOLINT
        }

        friend bool operator!=(const GenerateIterator& a, const GenerateIterator& b) {
            return a._current != b._current;
        }

        friend bool operator<(const GenerateIterator& a, const GenerateIterator& b) {
            return a._current < b._current;
        }

        friend bool operator>(const GenerateIterator& a, const GenerateIterator& b) {
            return b < a;
        }

        friend bool operator<=(const GenerateIterator& a, const GenerateIterator& b) {
            return !(b < a); // NOLINT
        }

        friend bool operator>=(const GenerateIterator& a, const GenerateIterator& b) {
            return !(a < b); // NOLINT
        }
    };
}}

#endif