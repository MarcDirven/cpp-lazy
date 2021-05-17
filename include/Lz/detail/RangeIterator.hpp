#pragma once

#ifndef LZ_RANGE_ITERATOR_HPP
#define LZ_RANGE_ITERATOR_HPP

#include <iterator>
#include <cmath>

namespace lz {
namespace internal {
template<class Arithmetic>
class RangeIterator {
	Arithmetic _iterator{};
	Arithmetic _step{};

public:
	using iterator_category = std::forward_iterator_tag;
	using value_type = Arithmetic;
	using difference_type = typename std::make_signed<Arithmetic>::type;
	using pointer = Arithmetic*;
	using reference = Arithmetic&;

	constexpr RangeIterator(const Arithmetic iterator, const Arithmetic step) :
		_iterator(iterator),
		_step(step) {
	}

	constexpr RangeIterator() = default;

	constexpr value_type operator*() const {
		return _iterator;
	}

	constexpr pointer operator->() const {
		return &_iterator;
	}

	LZ_CONSTEXPR_CXX_14 RangeIterator& operator++() {
		_iterator += _step;
		return *this;
	}

	LZ_CONSTEXPR_CXX_14 RangeIterator operator++(int) {
		RangeIterator tmp(*this);
		++*this;
		return tmp;
	}

	LZ_CONSTEXPR_CXX_14 friend bool operator!=(const RangeIterator& a, const RangeIterator& b) {
		LZ_ASSERT(a._step == b._step, "incompatible iterator types: difference step size");
		if (a._step < 0) {
			return a._iterator > b._iterator;
		}
		return a._iterator < b._iterator;
	}

	LZ_CONSTEXPR_CXX_14 friend bool operator==(const RangeIterator& a, const RangeIterator& b) {
		return !(a != b); // NOLINT
	}

	friend difference_type operator-(const RangeIterator& a, const RangeIterator& b) {
		LZ_ASSERT(a._step == b._step, "incompatible iterator types: difference step size");
		const auto difference = static_cast<difference_type>(a._iterator) - static_cast<difference_type>(b._iterator);
		if (a._step == 1) {
			return difference;
		}
		return static_cast<difference_type>(std::ceil(difference / static_cast<float>(a._step)));
	}

	constexpr RangeIterator operator+(const Arithmetic value) const {
		return RangeIterator(_iterator + value * _step, _step);
	}
};
} // internal

/**
 * Gets the distance between begin and end. Distance is always O(1).
 * @param begin Beginning of the sequence.
 * @param end Ending of the sequence.
 * @return The distance between begin and end.
 */
template<class Arithmetic>
typename internal::RangeIterator<Arithmetic>::difference_type
distance(const internal::RangeIterator<Arithmetic>& a, const internal::RangeIterator<Arithmetic>& b) {
	return b - a;
}

/**
 * Gets the nth value from iter. Next is always O(1).
 * @param iter A chunks iterator instance.
 * @param value The amount to add.
 * @return A chunks iterator with offset iter + value.
 */
template<class Arithmetic>
constexpr internal::RangeIterator<Arithmetic>
next(const internal::RangeIterator<Arithmetic>& iter, const internal::DiffType<internal::RangeIterator<Arithmetic>> value) {
	LZ_ASSERT(value >= 0, "Range iterator is not bidirectional, offset must be >= 0");
	return iter + value;
}
} // lz

#endif