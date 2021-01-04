#pragma once

#ifndef LZ_CHUNKS_ITERATOR_HPP
#define LZ_CHUNKS_ITERATOR_HPP

#include "BasicIteratorView.hpp"


namespace lz { namespace internal {
	template<class Iterator>
	class ChunksIterator {
		Iterator _begin{};
		Iterator _subRangeBegin{};
		Iterator _subRangeEnd{};
		Iterator _end{};
		std::size_t _chunkSize{};

		void prevChunk() {
			auto distance = std::distance(_begin, _subRangeBegin);
			auto chunkSizeSigned = static_cast<difference_type>(_chunkSize);

			if (distance > chunkSizeSigned) {
				std::advance(_subRangeBegin, -chunkSizeSigned);
			}
			else {
				std::advance(_subRangeBegin, -distance);
			}
		}

		void nextChunk() {
			auto distance = std::distance(_subRangeEnd, _end);
			auto chunkSizeSigned = static_cast<difference_type>(_chunkSize);

			if (distance > chunkSizeSigned) {
				std::advance(_subRangeEnd, chunkSizeSigned);
			}
			else {
				std::advance(_subRangeEnd, distance);
			}
		}

		using IterTraits = std::iterator_traits<Iterator>;
	public:
		using iterator_category = typename IterTraits::iterator_category;
		using value_type = BasicIteratorView<Iterator>;
		using reference = value_type;
		using pointer = FakePointerProxy<value_type>;
		using difference_type = typename IterTraits::difference_type;

		ChunksIterator(Iterator begin, Iterator end, const std::size_t chunkSize):
			_begin(begin),
			_subRangeBegin(begin == end ? end : begin),
			_subRangeEnd(begin == end ? end : begin),
			_end(end),
			_chunkSize(chunkSize)
		{
			if (_begin == _end) { // end iterator
				return;
			}
			nextChunk();
		}

		ChunksIterator() = default;

		reference operator*() const {
			return reference(_subRangeBegin, _subRangeEnd);
		}

		pointer operator->() const {
			return FakePointerProxy<decltype(**this)>(**this);
		}

		ChunksIterator& operator++() {
			_subRangeBegin = _subRangeEnd;
			nextChunk();
			return *this;
		}

		ChunksIterator operator++(int) {
			ChunksIterator tmp(*this);
			++*this;
			return tmp;
		}

		ChunksIterator& operator--() {
			_subRangeEnd = _subRangeBegin;
			prevChunk();
			return *this;
		}

		ChunksIterator operator--(int) {
			ChunksIterator tmp(*this);
			++*this;
			return tmp;
		}

		friend bool operator!=(const ChunksIterator& lhs, const ChunksIterator& rhs) {
			return lhs._subRangeBegin != rhs._subRangeBegin;
		}

		friend bool operator==(const ChunksIterator& lhs, const ChunksIterator& rhs) {
			return !(lhs != rhs); // NOLINT
		}
	};
}}

#endif //LZ_CHUNKS_ITERATOR_HPP
