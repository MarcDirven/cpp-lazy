#pragma once

#ifndef LZ_CHUNKS_ITERATOR_HPP
#define LZ_CHUNKS_ITERATOR_HPP

#include "BasicIteratorView.hpp"

#include <cmath>

namespace lz {
namespace internal {
template<class Iterator>
class ChunksIterator {
    using IterTraits = std::iterator_traits<Iterator>;

public:
    using iterator_category = typename std::common_type<std::forward_iterator_tag, typename IterTraits::iterator_category>::type;
    using value_type = BasicIteratorView<Iterator>;
    using reference = value_type;
    using pointer = FakePointerProxy<value_type>;
    using difference_type = typename IterTraits::difference_type;

private:
    Iterator _subRangeBegin{};
    Iterator _subRangeEnd{};
    Iterator _end{};
    difference_type _chunkSize{};
    difference_type _length{};

    LZ_CONSTEXPR_CXX_20 void nextChunk() {
        using lz::next;
        using std::next;

        if (_length > _chunkSize) {
            _subRangeEnd = next(std::move(_subRangeEnd), _chunkSize);
            _length -= _chunkSize;
        }
        else {
            _subRangeEnd = _end;
            _length = 0;
        }
    }

public:
    LZ_CONSTEXPR_CXX_20 ChunksIterator(Iterator begin, Iterator end, const std::size_t chunkSize, const difference_type length) :
        _subRangeBegin(begin == end ? end : begin),
        _subRangeEnd(begin == end ? end : std::move(begin)),
        _end(std::move(end)),
        _chunkSize(static_cast<difference_type>(chunkSize)),
        _length(length) {
        if (_subRangeBegin == _end) {
            return;
        }
        nextChunk();
    }

    constexpr ChunksIterator() = default;

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 reference operator*() const {
        return reference(_subRangeBegin, _subRangeEnd);
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 pointer operator->() const {
        return FakePointerProxy<decltype(**this)>(**this);
    }

    LZ_CONSTEXPR_CXX_20 ChunksIterator& operator++() {
        _subRangeBegin = _subRangeEnd;
        nextChunk();
        return *this;
    }

    LZ_CONSTEXPR_CXX_20 ChunksIterator operator++(int) {
        ChunksIterator tmp(*this);
        ++*this;
        return tmp;
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 friend bool operator!=(const ChunksIterator& lhs, const ChunksIterator& rhs) {
        LZ_ASSERT(lhs._chunkSize == rhs._chunkSize, "incompatible iterators: different chunk sizes");
        return lhs._subRangeBegin != rhs._subRangeBegin;
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 friend bool operator==(const ChunksIterator& lhs, const ChunksIterator& rhs) {
        return !(lhs != rhs); // NOLINT
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 friend difference_type operator-(const ChunksIterator& lhs, const ChunksIterator& rhs) {
        LZ_ASSERT(lhs._chunkSize == rhs._chunkSize, "incompatible iterators: different chunk sizes");
        const auto dist = getIterLength(rhs._subRangeBegin, lhs._end) / static_cast<float>(lhs._chunkSize);
        return static_cast<difference_type>(std::ceil(dist));
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 ChunksIterator operator+(const difference_type offset) const {
        using lz::next;
        using std::next;
        ChunksIterator tmp(*this);
        auto dist = getIterLength(tmp._subRangeEnd, tmp._end);
        const auto totalOffset = static_cast<difference_type>(tmp._chunkSize) * offset;
        if (totalOffset >= dist) {
            tmp._subRangeEnd = tmp._end;
            dist = getIterLength(tmp._subRangeBegin, tmp._end);
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
} // namespace internal

/**
 * Gets the distance between begin and end. If the underlying iterator type is random access, distance is O(1).
 * @param begin Beginning of the sequence.
 * @param end Ending of the sequence.
 * @return The distance between begin and end.
 */
template<LZ_CONCEPT_ITERATOR Iterator>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 typename internal::ChunksIterator<Iterator>::difference_type
distance(const internal::ChunksIterator<Iterator>& begin, const internal::ChunksIterator<Iterator>& end) {
    return end - begin;
}

/**
 * Gets the nth value from iter. If the underlying iterator type is random access, next is O(1).
 * @param iter A chunks iterator instance.
 * @param value The amount to add.
 * @return A chunks iterator with offset iter + value.
 */
template<LZ_CONCEPT_ITERATOR Iterator>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 internal::ChunksIterator<Iterator>
next(const internal::ChunksIterator<Iterator>& iter, const internal::DiffType<internal::ChunksIterator<Iterator>> value) {
    LZ_ASSERT(value >= 0, "offset must be greater than 0 since this is not a bidirectional/random access iterator");
    return iter + value;
}
} // namespace lz

#endif // LZ_CHUNKS_ITERATOR_HPP
