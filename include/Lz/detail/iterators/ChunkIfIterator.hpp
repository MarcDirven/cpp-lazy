#pragma once

#ifndef LZ_CHUNK_IF_ITERATOR_HPP
#define LZ_CHUNK_IF_ITERATOR_HPP

#include "Lz/IterBase.hpp"
#include "Lz/detail/Algorithm.hpp"
#include "Lz/detail/BasicIteratorView.hpp"
#include "Lz/detail/FakePointerProxy.hpp"
#include "Lz/detail/FunctionContainer.hpp"

namespace lz {
namespace detail {

template<class Iterator, class S, class UnaryPredicate>
class ChunkIfIterator : public IterBase<ChunkIfIterator<Iterator, S, UnaryPredicate>, BasicIteratorView<Iterator>,
                                        FakePointerProxy<BasicIteratorView<Iterator>>, DiffType<Iterator>,
                                        std::forward_iterator_tag, DefaultSentinel> {
    using IterTraits = std::iterator_traits<Iterator>;

public:
    using value_type = BasicIteratorView<Iterator>;
    using difference_type = typename IterTraits::difference_type;
    using reference = value_type;
    using pointer = FakePointerProxy<reference>;

private:
    bool _trailingEmpty{ true };
    Iterator _subRangeBegin{};
    Iterator _subRangeEnd{};
    S _end{};
    mutable FunctionContainer<UnaryPredicate> _predicate{};

    // Overload for when the iterator is a sentinel
    template<class T = Iterator>
    EnableIf<!std::is_same<T, S>::value> findNext() {
        _subRangeEnd = detail::findIf(_subRangeEnd, _end, _predicate);
    }

    // Overload for when the iterator is not a sentinel
    template<class T = Iterator>
    EnableIf<std::is_same<T, S>::value> findNext() {
        _subRangeEnd = std::find_if(_subRangeEnd, _end, _predicate);
    }

public:
    constexpr ChunkIfIterator() = default;

    ChunkIfIterator(Iterator iterator, S end, UnaryPredicate predicate) :
        _subRangeBegin(iterator),
        _subRangeEnd(std::move(iterator)),
        _end(std::move(end)),
        _predicate(std::move(predicate)) {
        if (_subRangeBegin != _end) {
            findNext();
        }
        else {
            _trailingEmpty = false;
        }
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 reference dereference() const {
        return { _subRangeBegin, _subRangeEnd };
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 pointer arrow() const {
        return FakePointerProxy<decltype(**this)>(**this);
    }

    LZ_CONSTEXPR_CXX_20 void increment() {
        if (_trailingEmpty && _subRangeEnd == _end) {
            _subRangeBegin = _subRangeEnd;
            _trailingEmpty = false;
            return;
        }

        auto prev = _subRangeEnd++;
        if (_subRangeEnd != _end) {
            _subRangeBegin = _subRangeEnd;
            findNext();
            return;
        }

        if (_predicate(*prev)) {
            _subRangeBegin = _subRangeEnd = _trailingEmpty ? std::move(prev) : _end;
            _trailingEmpty = false;
            return;
        }
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 bool eq(const ChunkIfIterator& rhs) const {
        return _subRangeBegin == rhs._subRangeBegin && _subRangeEnd == rhs._subRangeEnd && _trailingEmpty == rhs._trailingEmpty;
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 bool eq(DefaultSentinel) const {
        return _subRangeBegin == _end && !_trailingEmpty;
    }
};
} // namespace detail
} // namespace lz

#endif // LZ_CHUNK_IF_ITERATOR_HPP
