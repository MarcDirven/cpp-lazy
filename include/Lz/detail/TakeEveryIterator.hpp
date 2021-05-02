#pragma once

#ifndef LZ_TAKE_EVERY_ITERATOR_HPP
#define LZ_TAKE_EVERY_ITERATOR_HPP

#include "LzTools.hpp"


namespace lz { namespace internal {
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
		using iterator_category = LowestIterType<std::forward_iterator_tag, typename IterTraits::iterator_category>;
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
			if (_current + _offset >= _distance) {
				_iterator = _end;
			}
			else {
				_iterator = std::next(std::move(_iterator), _offset);
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
			return a._iterator != b._iterator;
		}
	};
}}

#endif