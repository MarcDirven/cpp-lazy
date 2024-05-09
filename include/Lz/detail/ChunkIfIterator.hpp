#pragma once

#ifndef LZ_CHUNK_IF_ITERATOR_HPP
#define LZ_CHUNK_IF_ITERATOR_HPP

#include "BasicIteratorView.hpp"
#include "FunctionContainer.hpp"

namespace lz {
namespace internal {
#ifdef LZ_HAS_EXECUTION
template<class Iterator, class UnaryPredicate, class Execution>
#else  // ^^ LZ_HAS_EXECUTION vv !LZ_HAS_EXECUTION

template<class Iterator, class UnaryPredicate>
#endif // LZ_HAS_EXECUTION
class ChunkIfIterator {
    Iterator _begin{};
    Iterator _subRangeBegin{};
    Iterator _subRangeEnd{};
    Iterator _end{};
    mutable FunctionContainer<UnaryPredicate> _predicate{};
#ifdef LZ_HAS_EXECUTION
    LZ_NO_UNIQUE_ADDRESS
    Execution _execution{};
#endif // LZ_HAS_EXECUTION
    using IterTraits = std::iterator_traits<Iterator>;

public:
    using value_type = BasicIteratorView<Iterator>;
    using difference_type = typename IterTraits::difference_type;
    using iterator_category =
        typename std::common_type<std::bidirectional_iterator_tag, typename IterTraits::iterator_category>::type;
    using reference = value_type;
    using pointer = FakePointerProxy<reference>;

private:
    template<class I>
    LZ_CONSTEXPR_CXX_20 I findNext(I first, I last) {
#ifdef LZ_HAS_EXECUTION
        if constexpr (internal::isCompatibleForExecution<Execution, Iterator>()) {
            return std::find_if(std::move(first), std::move(last), _predicate);
        }
        else {
            return std::find_if(_execution, std::move(first), std::move(last), _predicate);
        }
#else  // ^^ LZ_HAS_EXECUTION vv !LZ_HAS_EXECUTION
        return std::find_if(std::move(first), std::move(last), _predicate);
#endif // LZ_HAS_EXECUTION
    }

public:
    constexpr ChunkIfIterator() = default;

#ifdef LZ_HAS_EXECUTION
    LZ_CONSTEXPR_CXX_20
    ChunkIfIterator(Iterator iterator, Iterator begin, Iterator end, UnaryPredicate predicate, Execution execution) :
#else  // ^^ LZ_HAS_EXECUTION vv !LZ_HAS_EXECUTION

    ChunkIfIterator(Iterator iterator, Iterator begin, Iterator end, UnaryPredicate predicate) :
#endif // LZ_HAS_EXECUTION
        _begin(std::move(begin)),
        _subRangeBegin(iterator),
        _subRangeEnd(std::move(iterator)),
        _end(std::move(end)),
        _predicate(std::move(predicate))
#ifdef LZ_HAS_EXECUTION
        ,
        _execution(execution)
#endif // LZ_HAS_EXECUTION
    {
        if (_subRangeBegin == _end) {
            return;
        }
        _subRangeEnd = findNext(_subRangeBegin, _end);
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 reference operator*() const {
        return { _subRangeBegin, _subRangeEnd };
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 pointer operator->() const {
        return FakePointerProxy<decltype(**this)>(**this);
    }

    LZ_CONSTEXPR_CXX_20 ChunkIfIterator& operator++() {
        if (_subRangeEnd == _end) {
            _subRangeBegin = _end;
            return *this;
        }
        auto next = std::next(_subRangeEnd);
        if (next == _end) {
            if (_subRangeBegin == _subRangeEnd) {
                _subRangeBegin = _end, _subRangeEnd = _end;
                return *this;
            }
            _subRangeBegin = _subRangeEnd;
            return *this;
        }
        _subRangeBegin = std::move(next);
        _subRangeEnd = findNext(_subRangeBegin, _end);
        return *this;
    }

    LZ_CONSTEXPR_CXX_20 ChunkIfIterator operator++(int) {
        ChunkIfIterator tmp(*this);
        ++*this;
        return tmp;
    }

    LZ_CONSTEXPR_CXX_20 ChunkIfIterator& operator--() {
        if (_subRangeBegin != _end && _subRangeBegin != _subRangeEnd) {
            _subRangeEnd = --_subRangeBegin;
        }
        std::reverse_iterator<Iterator> subRangeBegin(std::move(_subRangeBegin));
        std::reverse_iterator<Iterator> end(_begin);
        _subRangeBegin = findNext(std::move(subRangeBegin), std::move(end)).base();
        if (_subRangeBegin == _subRangeEnd && _subRangeBegin != _begin) {
            --_subRangeBegin, --_subRangeEnd;
        }
        return *this;
    }

    LZ_CONSTEXPR_CXX_20 ChunkIfIterator operator--(int) {
        ChunkIfIterator tmp(*this);
        --*this;
        return tmp;
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 friend bool operator==(const ChunkIfIterator& lhs, const ChunkIfIterator& rhs) noexcept {
        return lhs._subRangeBegin == rhs._subRangeBegin;
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 friend bool operator!=(const ChunkIfIterator& lhs, const ChunkIfIterator& rhs) noexcept {
        return !(lhs == rhs); // NOLINT
    }
};
} // namespace internal
} // namespace lz

#endif // LZ_CHUNK_IF_ITERATOR_HPP
