#pragma once

#ifndef LZ_CHUNKS_ITERATOR_HPP
#define LZ_CHUNKS_ITERATOR_HPP

#include "Lz/IterBase.hpp"
#include "Lz/detail/BasicIteratorView.hpp"
#include "Lz/detail/FakePointerProxy.hpp"

#include <cmath>

namespace lz {
namespace detail {
template<class, bool>
class ChunksIterator;

template<class Iterator>
class ChunksIterator<Iterator, false /* isBidirectional */>
    : public IterBase<ChunksIterator<Iterator, false>, BasicIteratorView<Iterator>, FakePointerProxy<BasicIteratorView<Iterator>>,
                      DiffType<Iterator>, IterCat<Iterator>> {

    using IterTraits = std::iterator_traits<Iterator>;

public:
    using iterator_category = typename IterTraits::iterator_category;
    using value_type = BasicIteratorView<Iterator>;
    using reference = value_type;
    using pointer = FakePointerProxy<value_type>;
    using difference_type = typename IterTraits::difference_type;

private:
    Iterator _subRangeBegin{};
    Iterator _subRangeEnd{};
    Iterator _end{};
    difference_type _chunkSize{};

    LZ_CONSTEXPR_CXX_20 void nextChunk() {
        for (difference_type count = 0; count < _chunkSize && _subRangeEnd != _end; count++, ++_subRangeEnd) {
        }
    }

public:
    LZ_CONSTEXPR_CXX_20 ChunksIterator(Iterator iterator, Iterator end, const std::size_t chunkSize) :
        _subRangeBegin(iterator),
        _subRangeEnd(std::move(iterator)),
        _end(std::move(end)),
        _chunkSize(static_cast<difference_type>(chunkSize)) {
        if (_subRangeBegin == _end) {
            return;
        }
        nextChunk();
    }

    constexpr ChunksIterator() = default;

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 reference dereference() const {
        return { _subRangeBegin, _subRangeEnd };
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 pointer arrow() const {
        return FakePointerProxy<decltype(**this)>(**this);
    }

    LZ_CONSTEXPR_CXX_20 void increment() {
        _subRangeBegin = _subRangeEnd;
        nextChunk();
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 bool eq(const ChunksIterator& rhs) const noexcept {
        LZ_ASSERT(_chunkSize == rhs._chunkSize, "incompatible iterators: different chunk sizes");
        return _subRangeBegin == rhs._subRangeBegin;
    }
};

template<class Iterator>
class ChunksIterator<Iterator, true /* isBidirectional */>
    : public IterBase<ChunksIterator<Iterator, true>, BasicIteratorView<Iterator>, FakePointerProxy<BasicIteratorView<Iterator>>,
                      DiffType<Iterator>, IterCat<Iterator>> {

    using IterTraits = std::iterator_traits<Iterator>;

public:
    using iterator_category = typename IterTraits::iterator_category;
    using value_type = BasicIteratorView<Iterator>;
    using reference = value_type;
    using pointer = FakePointerProxy<value_type>;
    using difference_type = typename IterTraits::difference_type;

private:
    Iterator _begin{};
    Iterator _subRangeBegin{};
    Iterator _subRangeEnd{};
    Iterator _end{};
    difference_type _chunkSize{};

#ifdef __cpp_if_constexpr
    LZ_CONSTEXPR_CXX_20 void nextChunk() {
        if constexpr (IsRandomAccess<Iterator>::value) {
            if (_end - _subRangeEnd > _chunkSize) {
                _subRangeEnd += _chunkSize;
            }
            else {
                _subRangeEnd = _end;
            }
        }
        else {
            for (difference_type count = 0; count < _chunkSize && _subRangeEnd != _end; count++, ++_subRangeEnd) {
            }
        }
    }

    LZ_CONSTEXPR_CXX_20 void prevChunk() {
        if constexpr (IsRandomAccess<Iterator>::value) {
            if (_subRangeBegin - _begin > _chunkSize) {
                _subRangeBegin -= _chunkSize;
            }
            else {
                _subRangeBegin = _begin;
            }
        }
        else {
            for (difference_type count = _chunkSize; 0 < count && _subRangeBegin != _begin; count--, --_subRangeBegin) {
            }
        }
    }
#else
    template<class I = Iterator>
    LZ_CONSTEXPR_CXX_20 EnableIf<IsRandomAccess<I>::value, void> nextChunk() {
        if (_end - _subRangeEnd > _chunkSize) {
            _subRangeEnd += _chunkSize;
        }
        else {
            _subRangeEnd = _end;
        }
    }

    template<class I = Iterator>
    LZ_CONSTEXPR_CXX_20 EnableIf<!IsRandomAccess<I>::value> nextChunk() {
        for (difference_type count = 0; count < _chunkSize && _subRangeEnd != _end; count++, ++_subRangeEnd) {
        }
    }

    template<class I = Iterator>
    LZ_CONSTEXPR_CXX_20 EnableIf<IsRandomAccess<I>::value> prevChunk() {
        if (_subRangeBegin - _begin > _chunkSize) {
            _subRangeBegin -= _chunkSize;
        }
        else {
            _subRangeBegin = _begin;
        }
    }

    template<class I = Iterator>
    LZ_CONSTEXPR_CXX_20 EnableIf<!IsRandomAccess<I>::value> prevChunk() {
        for (difference_type count = _chunkSize; 0 < count && _subRangeBegin != _begin; count--, --_subRangeBegin) {
        }
    }
#endif // __cpp_if_constexpr

    void handleNegativeOffset(const difference_type offset, difference_type totalOffset) {
        auto dist = _subRangeEnd - _begin;
        if (std::abs(totalOffset) > dist) {
            _subRangeBegin = _begin;
            _subRangeEnd = _begin + static_cast<difference_type>((_end - _begin) / _chunkSize);
            return;
        }
        if (offset != -1 || _subRangeEnd != _end) {
            totalOffset += _chunkSize;
            _subRangeEnd += totalOffset;
        }
        _subRangeBegin = totalOffset >= _subRangeEnd - _begin ? _begin : _subRangeEnd + totalOffset;
    }

    void handlePositiveOffset(const difference_type totalOffset) {
        if (totalOffset >= _end - _subRangeBegin) {
            _subRangeEnd = _end;
            _subRangeBegin += _end - _subRangeBegin;
        }
        else {
            _subRangeBegin += totalOffset;
            _subRangeEnd = totalOffset >= _end - _subRangeBegin ? _end : _subRangeBegin + totalOffset;
        }
    }

public:
    LZ_CONSTEXPR_CXX_20 ChunksIterator(Iterator iterator, Iterator begin, Iterator end, const std::size_t chunkSize) :
        _begin(std::move(begin)),
        _subRangeBegin(iterator),
        _subRangeEnd(std::move(iterator)),
        _end(std::move(end)),
        _chunkSize(static_cast<difference_type>(chunkSize)) {
        if (_subRangeBegin == _end) {
            return;
        }
        nextChunk();
    }

    constexpr ChunksIterator() = default;

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 reference dereference() const {
        return { _subRangeBegin, _subRangeEnd };
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 pointer arrow() const {
        return FakePointerProxy<decltype(**this)>(**this);
    }

    LZ_CONSTEXPR_CXX_20 void increment() {
        _subRangeBegin = _subRangeEnd;
        nextChunk();
    }

    LZ_CONSTEXPR_CXX_20 void decrement() {
        _subRangeEnd = _subRangeBegin;
        prevChunk();
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 bool eq(const ChunksIterator& rhs) const noexcept {
        LZ_ASSERT(_chunkSize == rhs._chunkSize, "incompatible iterators: different chunk sizes");
        return _subRangeBegin == rhs._subRangeBegin;
    }

    LZ_CONSTEXPR_CXX_20 void plusIs(const difference_type offset) {
        const auto totalOffset = _chunkSize * offset;
        if (totalOffset < 0) {
            handleNegativeOffset(offset, totalOffset);
        }
        // Use > so that we distinguish 0
        else if (totalOffset > 0) {
            handlePositiveOffset(totalOffset);
        }
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 difference_type difference(const ChunksIterator& rhs) const {
        LZ_ASSERT(_chunkSize == rhs._chunkSize, "incompatible iterators: different chunk sizes");
        const auto dist = _subRangeBegin - rhs._subRangeBegin;
        return dist == 0 ? 0 : roundEven<difference_type>(dist, _chunkSize);
    }
};
} // namespace detail
} // namespace lz

#endif // LZ_CHUNKS_ITERATOR_HPP
