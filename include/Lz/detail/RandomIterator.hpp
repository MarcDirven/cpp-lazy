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
	mutable Distribution _distribution{};
	std::ptrdiff_t _current{};
	bool _isWhileTrueLoop{};
	Generator* _generator{nullptr};

public:
	RandomIterator(const Distribution& distribution, Generator& generator, const std::ptrdiff_t current, const bool isWhileTrueLoop) :
		_distribution(distribution),
		_current(current),
		_isWhileTrueLoop(isWhileTrueLoop),
		_generator(&generator) {
	}

	RandomIterator() = default;

	LZ_NODISCARD value_type operator*() const {
		return _distribution(*_generator);
	}

	LZ_NODISCARD value_type operator()() const {
		return _distribution(*_generator);
	}

	LZ_NODISCARD pointer operator->() const {
		return FakePointerProxy<decltype(**this)>(**this);
	}

	LZ_NODISCARD result_type (min)() const noexcept {
		return _distribution->min();
	}

	LZ_NODISCARD result_type (max)() const noexcept {
		return _distribution->max();
	}

	RandomIterator& operator--() noexcept {
		if (!_isWhileTrueLoop) {
			--_current;
		}
		return *this;
	}

	RandomIterator operator--(int) noexcept {
		RandomIterator tmp(*this);
		--*this;
		return tmp;
	}

	RandomIterator& operator+=(const difference_type offset) noexcept {
		if (!_isWhileTrueLoop) {
			_current += offset;
		}
		return *this;
	}

	LZ_NODISCARD RandomIterator operator+(const difference_type offset) const noexcept {
		RandomIterator tmp(*this);
		tmp += offset;
		return tmp;
	}

	RandomIterator& operator-=(const difference_type offset) noexcept {
		if (!_isWhileTrueLoop) {
			_current -= offset;
		}
		return *this;
	}

	LZ_NODISCARD RandomIterator operator-(const difference_type offset) const noexcept {
		RandomIterator tmp(*this);
		tmp -= offset;
		return tmp;
	}

	LZ_NODISCARD friend difference_type operator-(const RandomIterator& a, const RandomIterator& b) noexcept {
		LZ_ASSERT(a._isWhileTrueLoop == b._isWhileTrueLoop, "incompatible iterator types: both must be while true or not");
		return a._current - b._current;
	}

	LZ_NODISCARD value_type operator[](const difference_type offset) const noexcept {
		return *(*this + offset);
	}

	RandomIterator& operator++() noexcept {
		if (!_isWhileTrueLoop) {
			++_current;
		}
		return *this;
	}

	RandomIterator operator++(int) noexcept {
		RandomIterator tmp(*this);
		++*this;
		return tmp;
	}

	LZ_NODISCARD friend bool operator!=(const RandomIterator& a, const RandomIterator& b) noexcept {
		LZ_ASSERT(a._isWhileTrueLoop == b._isWhileTrueLoop, "incompatible iterator types: both must be while true or not");
		return a._current != b._current;
	}

	LZ_NODISCARD friend bool operator==(const RandomIterator& a, const RandomIterator& b) noexcept {
		return !(a != b); // NOLINT
	}

	LZ_NODISCARD friend bool operator<(const RandomIterator& a, const RandomIterator& b) noexcept {
		LZ_ASSERT(a._isWhileTrueLoop == b._isWhileTrueLoop, "incompatible iterator types: both must be while true or not");
		return a._current < b._current;
	}

	LZ_NODISCARD friend bool operator>(const RandomIterator& a, const RandomIterator& b) noexcept {
		return b < a;
	}

	LZ_NODISCARD friend bool operator<=(const RandomIterator& a, const RandomIterator& b) noexcept {
		return !(b < a); // NOLINT
	}

	LZ_NODISCARD friend bool operator>=(const RandomIterator& a, const RandomIterator& b) noexcept {
		return !(a < b); // NOLINT
	}
};
}}

#endif