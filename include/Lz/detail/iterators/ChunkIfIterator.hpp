#pragma once

#ifndef LZ_CHUNK_IF_ITERATOR_HPP
#define LZ_CHUNK_IF_ITERATOR_HPP

#include "Lz/IterBase.hpp"
#include "Lz/detail/BasicIteratorView.hpp"
#include "Lz/detail/FakePointerProxy.hpp"
#include "Lz/detail/FunctionContainer.hpp"

namespace lz {
namespace detail {
#ifdef LZ_HAS_EXECUTION
template<class Iterator, class UnaryPredicate, class Execution>
class ChunkIfIterator
    : public IterBase<ChunkIfIterator<Iterator, UnaryPredicate, Execution>, BasicIteratorView<Iterator>,
                      FakePointerProxy<BasicIteratorView<Iterator>>, DiffType<Iterator>, std::forward_iterator_tag> {
#else  // ^^ LZ_HAS_EXECUTION vv !LZ_HAS_EXECUTION

template<class Iterator, class UnaryPredicate>
class ChunkIfIterator
    : public IterBase<ChunkIfIterator<Iterator, UnaryPredicate>, BasicIteratorView<Iterator>,
                      FakePointerProxy<BasicIteratorView<Iterator>>, DiffType<Iterator>, std::forward_iterator_tag> {
#endif // LZ_HAS_EXECUTION

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
    using iterator_category = std::forward_iterator_tag;
    using reference = value_type;
    using pointer = FakePointerProxy<reference>;

private:
    template<class I>
    LZ_CONSTEXPR_CXX_20 I findNext(I first, I last) {
#ifdef LZ_HAS_EXECUTION
        if constexpr (detail::isCompatibleForExecution<Execution, Iterator>()) {
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

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 reference dereference() const {
        return { _subRangeBegin, _subRangeEnd };
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 pointer arrow() const {
        return FakePointerProxy<decltype(**this)>(**this);
    }

    LZ_CONSTEXPR_CXX_20 void increment() {
        if (_subRangeEnd == _end) {
            _subRangeBegin = _end;
            return;
        }
        auto next = std::next(_subRangeEnd);
        if (next == _end) {
            if (_subRangeBegin == _subRangeEnd) {
                _subRangeBegin = _end, _subRangeEnd = _end;
                return;
            }
            _subRangeBegin = _subRangeEnd;
            return;
        }
        _subRangeBegin = std::move(next);
        _subRangeEnd = findNext(_subRangeBegin, _end);
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 bool eq(const ChunkIfIterator& rhs) const {
        return _subRangeBegin == rhs._subRangeBegin;
    }
};
} // namespace detail
} // namespace lz

#endif // LZ_CHUNK_IF_ITERATOR_HPP
