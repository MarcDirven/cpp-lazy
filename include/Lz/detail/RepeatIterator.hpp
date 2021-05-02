#pragma once

#ifndef LZ_REPEAT_ITERATOR_HPP
#define LZ_REPEAT_ITERATOR_HPP

#include <iterator>
#include <limits>


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

		constexpr RepeatIterator& operator++() {
			if (!_iterHelper->isWhileTrueLoop) {
				++_iterator;
			}
			return *this;
		}

		constexpr RepeatIterator operator++(int) {
			RepeatIterator tmp(*this);
			++*this;
			return tmp;
		}

		constexpr RepeatIterator& operator--() {
			if (!_iterHelper->isWhileTrueLoop) {
				--_iterator;
			}
			return *this;
		}

		constexpr RepeatIterator operator--(int) {
			RepeatIterator tmp(*this);
			--*this;
			return tmp;
		}

		constexpr RepeatIterator& operator+=(const difference_type offset) {
			if (!_iterHelper->isWhileTrueLoop) {
				_iterator += offset;
			}
			return *this;
		}

		constexpr RepeatIterator& operator-=(const difference_type offset) {
			if (!_iterHelper->isWhileTrueLoop) {
				_iterator -= offset;
			}
			return *this;
		}

		constexpr RepeatIterator operator+(const difference_type offset) const {
			RepeatIterator tmp(*this);
			tmp += offset;
			return tmp;
		}

		constexpr RepeatIterator operator-(const difference_type offset) const {
			RepeatIterator tmp(*this);
			tmp -= offset;
			return tmp;
		}

		constexpr difference_type operator-(const RepeatIterator& other) const {
			return _iterator - other._iterator;
		}

		constexpr reference operator[](const difference_type offset) const {
			return *(*this + offset);
		}

		constexpr friend bool operator!=(const RepeatIterator& a, const RepeatIterator& b) {
			return a._iterator != b._iterator;
		}

		constexpr friend bool operator==(const RepeatIterator& a, const RepeatIterator& b) {
			return !(a != b); // NOLINT
		}

		constexpr friend bool operator<(const RepeatIterator& a, const RepeatIterator& b) {
			return a._iterator < b._iterator;
		}

		constexpr friend bool operator>(const RepeatIterator& a, const RepeatIterator& b) {
			return b < a;
		}

		constexpr friend bool operator<=(const RepeatIterator& a, const RepeatIterator& b) {
			return !(b < a); // NOLINT
		}

		constexpr friend bool operator>=(const RepeatIterator& a, const RepeatIterator& b) {
			return !(a < b); // NOLINT
		}
	};
}}

#endif