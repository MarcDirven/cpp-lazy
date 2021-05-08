#pragma once

#ifndef LZ_RANDOM_ITERATOR_HPP
#define LZ_RANDOM_ITERATOR_HPP

#include "LzTools.hpp"


namespace lz { namespace internal {
template<LZ_CONCEPT_ARITHMETIC Arithmetic, class Distribution, class Generator>
class RandomIterator {
public:
	using iterator_category = std::random_access_iterator_tag;
	using value_type = Arithmetic;
	using difference_type = std::ptrdiff_t;
	using pointer = FakePointerProxy<Arithmetic>;
	using reference = value_type;
	using result_type = value_type;

private:
	std::ptrdiff_t _current{};
	bool _isWhileTrueLoop{};
	Distribution _distribution{};
	Generator* _generator{nullptr};

public:
	RandomIterator(const Distribution& distribution, Generator& generator, const std::ptrdiff_t current,
				   const bool isWhileTrueLoop) :
		_current(current),
		_isWhileTrueLoop(isWhileTrueLoop),
		_distribution(distribution),
		_generator(&generator) {
	}

	RandomIterator() = default;

	value_type operator*() {
		return _distribution(*_generator);
	}

	value_type operator()() {
		return _distribution(*_generator);
	}

	pointer operator->() {
		return FakePointerProxy<decltype(**this)>(**this);
	}

	result_type (min)() const {
		return _distribution->min();
	}

	result_type (max)() const {
		return _distribution->max();
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

	friend bool operator<(const RandomIterator& a, const RandomIterator& b) {
		return a._current < b._current;
	}

	friend bool operator>(const RandomIterator& a, const RandomIterator& b) {
		return b < a;
	}

	friend bool operator<=(const RandomIterator& a, const RandomIterator& b) {
		return !(b < a); // NOLINT
	}

	friend bool operator>=(const RandomIterator& a, const RandomIterator& b) {
		return !(a < b); // NOLINT
	}
};
}}

#endif