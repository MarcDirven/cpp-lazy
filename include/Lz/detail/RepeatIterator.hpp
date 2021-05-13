#pragma once

#ifndef LZ_REPEAT_ITERATOR_HPP
#define LZ_REPEAT_ITERATOR_HPP

#include <limits>

#include "LzTools.hpp"


namespace lz { namespace internal {
template<class T>
struct RepeatIteratorHelper {
	mutable T toRepeat{};
	bool isWhileTrueLoop{};

	constexpr RepeatIteratorHelper(T toRepeat, const std::size_t amount) :
		toRepeat(std::move(toRepeat)),
		isWhileTrueLoop(amount == std::numeric_limits<std::size_t>::max()) {}

	constexpr RepeatIteratorHelper() = default;
};


template<class T>
class RepeatIterator {
	const RepeatIteratorHelper<T>* _iterHelper{nullptr};
	std::size_t _iterator{};

public:
	using iterator_category = std::random_access_iterator_tag;
	using value_type = T;
	using difference_type = std::ptrdiff_t;
	using pointer = T*;
	using reference = T&;

	constexpr RepeatIterator(const RepeatIteratorHelper<T>* iteratorHelper, const std::size_t start) :
		_iterHelper(iteratorHelper),
		_iterator(start) {
	}

	constexpr RepeatIterator() = default;

	constexpr reference operator*() const {
		return _iterHelper->toRepeat;
	}

	constexpr pointer operator->() const {
		return &_iterHelper->toRepeat;
	}

	LZ_CONSTEXPR_CXX_17 RepeatIterator& operator++() {
		if (!_iterHelper->isWhileTrueLoop) {
			++_iterator;
		}
		return *this;
	}

	LZ_CONSTEXPR_CXX_17 RepeatIterator operator++(int) {
		RepeatIterator tmp(*this);
		++*this;
		return tmp;
	}

	LZ_CONSTEXPR_CXX_17 RepeatIterator& operator--() {
		if (!_iterHelper->isWhileTrueLoop) {
			--_iterator;
		}
		return *this;
	}

	LZ_CONSTEXPR_CXX_17 RepeatIterator operator--(int) {
		RepeatIterator tmp(*this);
		--*this;
		return tmp;
	}

	LZ_CONSTEXPR_CXX_17 RepeatIterator& operator+=(const difference_type offset) {
		if (!_iterHelper->isWhileTrueLoop) {
			_iterator += offset;
		}
		return *this;
	}

	LZ_CONSTEXPR_CXX_17 RepeatIterator& operator-=(const difference_type offset) {
		if (!_iterHelper->isWhileTrueLoop) {
			_iterator -= offset;
		}
		return *this;
	}

	LZ_CONSTEXPR_CXX_17 RepeatIterator operator+(const difference_type offset) const {
		RepeatIterator tmp(*this);
		tmp += offset;
		return tmp;
	}

	LZ_CONSTEXPR_CXX_17 RepeatIterator operator-(const difference_type offset) const {
		RepeatIterator tmp(*this);
		tmp -= offset;
		return tmp;
	}

	LZ_CONSTEXPR_CXX_17 friend difference_type operator-(const RepeatIterator& a, const RepeatIterator& b) {
		LZ_ASSERT(a._iterHelper->isWhileTrueLoop == b._iterHelper->isWhileTrueLoop,
				  "incompatible iterator types: both must be while true loop or not");
		return a._iterator - b._iterator;
	}

	LZ_CONSTEXPR_CXX_17 reference operator[](const difference_type offset) const {
		return *(*this + offset);
	}

	LZ_CONSTEXPR_CXX_17 friend bool operator!=(const RepeatIterator& a, const RepeatIterator& b) {
		LZ_ASSERT(a._iterHelper->isWhileTrueLoop == b._iterHelper->isWhileTrueLoop,
				  "incompatible iterator types: both must be while true loop or not");
		return a._iterator != b._iterator;
	}

	LZ_CONSTEXPR_CXX_17 friend bool operator==(const RepeatIterator& a, const RepeatIterator& b) {
		return !(a != b); // NOLINT
	}

	LZ_CONSTEXPR_CXX_17 friend bool operator<(const RepeatIterator& a, const RepeatIterator& b) {
		LZ_ASSERT(a._iterHelper->isWhileTrueLoop == b._iterHelper->isWhileTrueLoop,
				  "incompatible iterator types: both must be while true loop or not");
		return a._iterator < b._iterator;
	}

	LZ_CONSTEXPR_CXX_17 friend bool operator>(const RepeatIterator& a, const RepeatIterator& b) {
		return b < a;
	}

	LZ_CONSTEXPR_CXX_17 friend bool operator<=(const RepeatIterator& a, const RepeatIterator& b) {
		return !(b < a); // NOLINT
	}

	LZ_CONSTEXPR_CXX_17 friend bool operator>=(const RepeatIterator& a, const RepeatIterator& b) {
		return !(a < b); // NOLINT
	}
};
}}

#endif