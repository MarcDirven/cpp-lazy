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

		LZ_CONSTEXPR_CXX_17 void nextChunk() {
			auto distance = std::distance(_subRangeEnd, _end);
			auto chunkSizeSigned = static_cast<difference_type>(_chunkSize);

			if (distance > chunkSizeSigned) {
				_subRangeEnd = std::next(std::move(_subRangeEnd), chunkSizeSigned);
			}
			else {
				_subRangeEnd = std::next(std::move(_subRangeEnd), distance);
			}
		}

		using IterTraits = std::iterator_traits<Iterator>;
	public:
		using iterator_category = LowestIterTypeT<std::forward_iterator_tag, typename IterTraits::iterator_category>;
		using value_type = BasicIteratorView<Iterator>;
		using reference = value_type;
		using pointer = FakePointerProxy<value_type>;
		using difference_type = typename IterTraits::difference_type;

		LZ_CONSTEXPR_CXX_17 ChunksIterator(Iterator begin, Iterator end, const std::size_t chunkSize) :
			_begin(begin),
			_subRangeBegin(begin == end ? end : begin),
			_subRangeEnd(begin == end ? end : begin),
			_end(end),
			_chunkSize(chunkSize) {
			if (_begin == _end) { // end iterator
				return;
			}
			nextChunk();
		}

		constexpr ChunksIterator() = default;

		LZ_CONSTEXPR_CXX_17 reference operator*() const {
			return reference(_subRangeBegin, _subRangeEnd);
		}

		LZ_CONSTEXPR_CXX_17 pointer operator->() const {
			return FakePointerProxy<decltype(**this)>(**this);
		}

		LZ_CONSTEXPR_CXX_17 ChunksIterator& operator++() {
			_subRangeBegin = _subRangeEnd;
			nextChunk();
			return *this;
		}

		LZ_CONSTEXPR_CXX_17 ChunksIterator operator++(int) {
			ChunksIterator tmp(*this);
			++*this;
			return tmp;
		}

		LZ_CONSTEXPR_CXX_17 friend bool operator!=(const ChunksIterator& lhs, const ChunksIterator& rhs) {
			return lhs._subRangeBegin != rhs._subRangeBegin;
		}

		LZ_CONSTEXPR_CXX_17 friend bool operator==(const ChunksIterator& lhs, const ChunksIterator& rhs) {
			return !(lhs != rhs); // NOLINT
		}
	};
}}

#endif //LZ_CHUNKS_ITERATOR_HPP
