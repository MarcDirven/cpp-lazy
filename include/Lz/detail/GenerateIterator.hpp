#pragma once

#ifndef LZ_GENERATE_ITERATOR_HPP
#define LZ_GENERATE_ITERATOR_HPP

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
	constexpr GenerateIterator() = default;

	constexpr GenerateIterator(const std::size_t start, GeneratorFunc generatorFunc, const bool isWhileTrueLoop) :
		_current(static_cast<difference_type>(start)),
		_generator(std::move(generatorFunc)),
		_isWhileTrueLoop(isWhileTrueLoop) {}

	constexpr reference operator*() const {
		return _generator();
	}

	LZ_CONSTEXPR_CXX_14 reference operator*() {
		return _generator();
	}

	constexpr pointer operator->() const {
		return FakePointerProxy<decltype(**this)>(**this);
	}

	LZ_CONSTEXPR_CXX_14 pointer operator->() {
		return FakePointerProxy<decltype(**this)>(**this);
	}

	LZ_CONSTEXPR_CXX_14 GenerateIterator& operator++() {
		if (!_isWhileTrueLoop) {
			++_current;
		}
		return *this;
	}

	LZ_CONSTEXPR_CXX_14 GenerateIterator operator++(int) {
		GenerateIterator tmp(*this);
		++*this;
		return tmp;
	}

	LZ_CONSTEXPR_CXX_14 GenerateIterator& operator--() {
		if (!_isWhileTrueLoop) {
			--_current;
		}
		return *this;
	}

	LZ_CONSTEXPR_CXX_14 GenerateIterator operator--(int) {
		GenerateIterator tmp(*this);
		--*this;
		return tmp;
	}

	LZ_CONSTEXPR_CXX_14 GenerateIterator& operator+=(const difference_type offset) {
		if (!_isWhileTrueLoop) {
			_current += offset;
		}
		return *this;
	}

	LZ_CONSTEXPR_CXX_14 GenerateIterator& operator-=(const difference_type offset) {
		if (!_isWhileTrueLoop) {
			_current -= offset;
		}
		return *this;
	}

	LZ_CONSTEXPR_CXX_14 GenerateIterator operator+(const difference_type offset) const {
		GenerateIterator tmp(*this);
		tmp += offset;
		return tmp;
	}

	LZ_CONSTEXPR_CXX_14 GenerateIterator operator-(const difference_type offset) const {
		GenerateIterator tmp(*this);
		tmp -= offset;
		return tmp;
	}

	constexpr difference_type operator-(const GenerateIterator& other) const {
		return _current - other._current;
	}

	LZ_CONSTEXPR_CXX_14 value_type operator[](const difference_type offset) const {
		return *(*this + offset);
	}

	constexpr friend bool operator==(const GenerateIterator& a, const GenerateIterator& b) {
		return !(a != b); // NOLINT
	}

	constexpr friend bool operator!=(const GenerateIterator& a, const GenerateIterator& b) {
		return a._current != b._current;
	}

	constexpr friend bool operator<(const GenerateIterator& a, const GenerateIterator& b) {
		return a._current < b._current;
	}

	constexpr friend bool operator>(const GenerateIterator& a, const GenerateIterator& b) {
		return b < a;
	}

	constexpr friend bool operator<=(const GenerateIterator& a, const GenerateIterator& b) {
		return !(b < a); // NOLINT
	}

	constexpr friend bool operator>=(const GenerateIterator& a, const GenerateIterator& b) {
		return !(a < b); // NOLINT
	}
};
}}

#endif