#pragma once

#ifndef LZ_TAKE_EVERY_ITERATOR_HPP
#define LZ_TAKE_EVERY_ITERATOR_HPP

#include "LzTools.hpp"


namespace lz {
namespace internal {
template<LZ_CONCEPT_ITERATOR Iterator>
class TakeEveryIterator {
	Iterator _iterator{};
	Iterator _end{};
	std::size_t _offset{};
	std::size_t _current{};
	std::size_t _distance{};

	using IterTraits = std::iterator_traits<Iterator>;

public:
	using value_type = typename IterTraits::value_type;
	using iterator_category = typename std::common_type<std::forward_iterator_tag, typename IterTraits::iterator_category>::type;
	using difference_type = typename IterTraits::difference_type;
	using reference = typename IterTraits::reference;
	using pointer = typename IterTraits::pointer;

	constexpr TakeEveryIterator(Iterator iterator, Iterator end, const std::size_t offset, const std::size_t distance) :
		_iterator(std::move(iterator)),
		_end(std::move(end)),
		_offset(offset),
		_current(iterator == end ? distance : 0),
		_distance(distance) {
	}

	constexpr TakeEveryIterator() = default;

	constexpr reference operator*() const {
		return *_iterator;
	}

	constexpr pointer operator->() const {
		return &*_iterator;
	}

	LZ_CONSTEXPR_CXX_17 TakeEveryIterator& operator++() {
		using std::next; using lz::next;
		if (_current + _offset >= _distance) {
			_iterator = _end;
		}
		else {
			_iterator = next(std::move(_iterator), static_cast<difference_type>(_offset));
			_current += _offset;
		}
		return *this;
	}

	LZ_CONSTEXPR_CXX_17 TakeEveryIterator operator++(int) {
		TakeEveryIterator tmp(*this);
		++*this;
		return tmp;
	}

	constexpr friend bool operator==(const TakeEveryIterator& a, const TakeEveryIterator& b) {
		return !(a != b); // NOLINT
	}

	constexpr friend bool operator!=(const TakeEveryIterator& a, const TakeEveryIterator& b) {
		LZ_ASSERT(a._offset == b._offset, "incompatible iterator types: different offsets");
		return a._iterator != b._iterator;
	}

	LZ_CONSTEXPR_CXX_17 friend difference_type operator-(const TakeEveryIterator& a, const TakeEveryIterator& b) {
		using lz::distance; using std::distance;
		LZ_ASSERT(a._offset == b._offset, "incompatible iterator types: different offsets");
		const auto dist = distance(b._iterator, a._iterator) / static_cast<float>(a._offset);
		return static_cast<difference_type>(std::ceil(dist));
	}

	LZ_CONSTEXPR_CXX_17 TakeEveryIterator operator+(const difference_type offset) const {
		using lz::distance; using std::distance; using std::next; using lz::next;
		const auto dist = distance(_iterator, _end);
		const auto diffOffset = static_cast<difference_type>(_offset) * offset;
		if (diffOffset >= dist) {
			return TakeEveryIterator(_end, _end, _offset, 0);
		}
		return TakeEveryIterator(next(_iterator, diffOffset), _end, _offset, dist);
	}
};
} // internal

/**
 * Gets the distance between begin and end. If the underlying iterator type is random access, distance is O(1).
 * @param begin Beginning of the sequence.
 * @param end Ending of the sequence.
 * @return The distance between begin and end.
 */
template<class Iterator>
LZ_CONSTEXPR_CXX_17 typename internal::TakeEveryIterator<Iterator>::difference_type
distance(const internal::TakeEveryIterator<Iterator>& begin, const internal::TakeEveryIterator<Iterator>& end) {
	return end - begin;
}

/**
 * Gets the nth value from iter. If the underlying iterator type is random access, next is O(1).
 * @param iter A take every iterator instance.
 * @param value The amount to add.
 * @return A take every iterator with offset iter + value.
 */
template<class Iterator>
LZ_CONSTEXPR_CXX_17 internal::TakeEveryIterator<Iterator>
next(const internal::TakeEveryIterator<Iterator>& t, const internal::DiffType<internal::TakeEveryIterator<Iterator>> value) {
	LZ_ASSERT(value >= 0, "Take every iterator is not random access, offset must be >= 0");
	return t + value;
}
} // lz

#endif