#pragma once

#ifndef LZ_CHUNKS_ITERATOR_HPP
#define LZ_CHUNKS_ITERATOR_HPP

#include "BasicIteratorView.hpp"


namespace lz {
namespace internal {
template<class Iterator>
class ChunksIterator {
	Iterator _subRangeBegin{};
	Iterator _subRangeEnd{};
	Iterator _end{};
	std::size_t _chunkSize{};

	using IterTraits = std::iterator_traits<Iterator>;

public:
	using iterator_category = typename std::common_type<std::forward_iterator_tag, typename IterTraits::iterator_category>::type;
	using value_type = BasicIteratorView<Iterator>;
	using reference = value_type;
	using pointer = FakePointerProxy<value_type>;
	using difference_type = typename IterTraits::difference_type;

private:
	LZ_CONSTEXPR_CXX_17 void nextChunk() {
		using lz::distance; using std::distance; using std::next; using lz::next;
		const auto dist = distance(_subRangeEnd, _end);
		const auto chunkSizeSigned = static_cast<difference_type>(_chunkSize);

		if (dist > chunkSizeSigned) {
			_subRangeEnd = next(std::move(_subRangeEnd), chunkSizeSigned);
		}
		else {
			_subRangeEnd = _end;
		}
	}

public:
	LZ_CONSTEXPR_CXX_17 ChunksIterator(Iterator begin, Iterator end, const std::size_t chunkSize) :
		_subRangeBegin(begin == end ? end : begin),
		_subRangeEnd(begin == end ? end : begin),
		_end(end),
		_chunkSize(chunkSize) {
		if (begin == _end) { // end iterator
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

	LZ_CONSTEXPR_CXX_17 friend difference_type operator-(const ChunksIterator& lhs, const ChunksIterator& rhs) {
		using std::distance; using lz::distance;
		const auto dist = distance(rhs._subRangeBegin, lhs._end) / static_cast<float>(lhs._chunkSize);
		return static_cast<difference_type>(std::ceil(dist));
	}

	LZ_CONSTEXPR_CXX_20 ChunksIterator operator+(const difference_type offset) const {
		using lz::distance; using std::distance; using std::next; using lz::next;
		ChunksIterator tmp(*this);
		auto dist = distance(tmp._subRangeEnd, tmp._end);
		const auto totalOffset = static_cast<difference_type>(tmp._chunkSize) * offset;
		if (totalOffset >= dist) {
			tmp._subRangeEnd = tmp._end;
			dist = distance(tmp._subRangeBegin, tmp._end);
			LZ_ASSERT(dist + dist > totalOffset, "cannot access elements after end");
			tmp._subRangeBegin = totalOffset >= dist ? tmp._end : next(std::move(tmp._subRangeBegin), totalOffset);
		}
		else {
			tmp._subRangeBegin = next(std::move(tmp._subRangeBegin), totalOffset);
			tmp._subRangeEnd = next(std::move(tmp._subRangeEnd), totalOffset);
		}
		return tmp;
	}
};
} // internal

template<class Iterator>
LZ_CONSTEXPR_CXX_20 typename internal::ChunksIterator<Iterator>::difference_type
distance(const internal::ChunksIterator<Iterator>& a, const internal::ChunksIterator<Iterator>& b) {
	return b - a;
}

template<class Iterator>
LZ_CONSTEXPR_CXX_20 internal::ChunksIterator<Iterator>
next(const internal::ChunksIterator<Iterator>& c, const internal::DiffType<internal::ChunksIterator<Iterator>> value) {
	return c + value;
}
} // lz

#endif //LZ_CHUNKS_ITERATOR_HPP
