#pragma once

#ifndef LZ_RANGE_ITERATOR_HPP
#define LZ_RANGE_ITERATOR_HPP

#include <iterator>


namespace lz { namespace internal {
	template<LZ_CONCEPT_ARITHMETIC Arithmetic>
	class RangeIterator {
		Arithmetic _iterator{};
		Arithmetic _step{};

	public:
		using iterator_category = std::forward_iterator_tag;
		using value_type = Arithmetic;
		using difference_type = std::ptrdiff_t;
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
	};
}}

#endif