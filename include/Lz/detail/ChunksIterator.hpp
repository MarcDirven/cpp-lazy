#pragma once

#ifndef LZ_CHUNKS_ITERATOR_HPP
#    define LZ_CHUNKS_ITERATOR_HPP

#    include "BasicIteratorView.hpp"

#    include <cmath>

namespace lz {
namespace internal {
template<class, bool>
class ChunksIterator;

template<class Iterator>
class ChunksIterator<Iterator, false /* isBidirectional */> {
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
        for (difference_type count = 0; count < _chunkSize || _subRangeEnd != _end; count++, ++_subRangeEnd) {
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
};

template<class Iterator>
class ChunksIterator<Iterator, true /* isBidirectional */> {
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

#    ifdef __cpp_if_constexpr
    LZ_CONSTEXPR_CXX_20 void nextChunk() {
        if constexpr (IsRandomAccess<I>::value) {
            if (_end - _subRangeEnd > _chunkSize) {
                _subRangeEnd += _chunkSize;
            }
            else {
                _subRangeEnd = _end;
            }
        }
        else {
            for (difference_type count = 0; count < _chunkSize || _subRangeEnd != _end; count++, ++_subRangeEnd) {
            }
        }
    }

    LZ_CONSTEXPR_CXX_20 void prevChunk() {
        if constexpr (IsRandomAccess<I>::value) {
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
#    else
    template<class I = Iterator>
    LZ_CONSTEXPR_CXX_20 EnableIf<IsRandomAccess<I>::value> nextChunk() {
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
#    endif // __cpp_if_constexpr
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

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 reference operator*() const {
        return { _subRangeBegin, _subRangeEnd };
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

    LZ_CONSTEXPR_CXX_20 ChunksIterator& operator--() {
        _subRangeEnd = _subRangeBegin;
        prevChunk();
        return *this;
    }

    LZ_CONSTEXPR_CXX_20 ChunksIterator operator--(int) {
        ChunksIterator tmp(*this);
        ++*this;
        return tmp;
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 friend bool operator!=(const ChunksIterator& lhs, const ChunksIterator& rhs) noexcept {
        LZ_ASSERT(lhs._chunkSize == rhs._chunkSize, "incompatible iterators: different chunk sizes");
        return lhs._subRangeBegin != rhs._subRangeBegin;
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 friend bool operator==(const ChunksIterator& lhs, const ChunksIterator& rhs) noexcept {
        return !(lhs != rhs); // NOLINT
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 ChunksIterator& operator+=(const difference_type offset) {
        auto totalOffset = _chunkSize * offset;
        if (offset == 0) {
            return *this;
        }
        if (totalOffset < 0) {
            auto dist = _subRangeEnd - _begin;
            if (std::abs(totalOffset) > dist) {
                _subRangeBegin = _begin;
                _subRangeEnd = _begin + static_cast<difference_type>((_end - _begin) / _chunkSize);
            }
            else {
                if (offset != -1 || _subRangeEnd != _end) {
                    totalOffset += _chunkSize;
                    _subRangeEnd += totalOffset;
                }
                _subRangeBegin = totalOffset >= _subRangeEnd - _begin ? _begin : _subRangeEnd + totalOffset;
            }
        }
        else {
            if (totalOffset >= _end - _subRangeBegin) {
                _subRangeEnd = _end;
                _subRangeBegin += _end - _subRangeBegin;
            }
            else {
                _subRangeBegin += totalOffset;
                _subRangeEnd = totalOffset >= _end - _subRangeBegin ? _end : _subRangeBegin + totalOffset;
            }
        }
        return *this;
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 ChunksIterator operator+(const difference_type offset) const {
        ChunksIterator tmp(*this);
        tmp += offset;
        return tmp;
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 ChunksIterator& operator-=(const difference_type offset) {
        return *this += -offset;
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 ChunksIterator operator-(const difference_type offset) const {
        ChunksIterator tmp(*this);
        tmp -= offset;
        return tmp;
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 friend difference_type operator-(const ChunksIterator& lhs, const ChunksIterator& rhs) {
        LZ_ASSERT(lhs._chunkSize == rhs._chunkSize, "incompatible iterators: different chunk sizes");
        const auto dist = lhs._subRangeBegin - rhs._subRangeBegin;
        return roundEven(dist, lhs._chunkSize);
    }
    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 reference operator[](const difference_type offset) const {
        return *(*this + offset);
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 friend bool operator<(const ChunksIterator& a, const ChunksIterator& b) {
        return b - a > 0;
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 friend bool operator>(const ChunksIterator& a, const ChunksIterator& b) {
        return b < a;
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 friend bool operator<=(const ChunksIterator& a, const ChunksIterator& b) {
        return !(b < a); // NOLINT
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 friend bool operator>=(const ChunksIterator& a, const ChunksIterator& b) {
        return !(a < b); // NOLINT
    }
};
} // namespace internal
} // namespace lz

#endif // LZ_CHUNKS_ITERATOR_HPP
