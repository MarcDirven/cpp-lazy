#pragma once

#ifndef LZ_RANGE_ITERATOR_HPP
#define LZ_RANGE_ITERATOR_HPP

#include <iterator>


namespace lz {
namespace internal {
template<LZ_CONCEPT_ARITHMETIC Arithmetic>
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
		if (a._step < 0) {
			return a._iterator > b._iterator;
		}
		return a._iterator < b._iterator;
	}

	LZ_CONSTEXPR_CXX_14 friend bool operator==(const RangeIterator& a, const RangeIterator& b) {
		return !(a != b); // NOLINT
	}

	LZ_CONSTEXPR_CXX_14 friend difference_type operator-(const RangeIterator& a, const RangeIterator& b) {
		return static_cast<difference_type>(a._iterator) - static_cast<difference_type>(b._iterator);
	}

	constexpr RangeIterator operator+(const Arithmetic value) const {
		return RangeIterator(_iterator + value, _step);
	}
};
} // internal

template<class Arithmetic>
LZ_CONSTEXPR_CXX_14 typename internal::RangeIterator<Arithmetic>::difference_type
distance(const internal::RangeIterator<Arithmetic>& a, const internal::RangeIterator<Arithmetic>& b) {
	return b - a;
}

template<class Arithmetic>
constexpr internal::RangeIterator<Arithmetic>
next(const internal::RangeIterator<Arithmetic>& r, const internal::DiffType<internal::RangeIterator<Arithmetic>> value) {
	return r + value;
}
} // lz

#endif