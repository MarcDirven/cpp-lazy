#pragma once

#ifndef LZ_EXCEPT_ITERATOR_HPP
#define LZ_EXCEPT_ITERATOR_HPP

#include "Lz/IterBase.hpp"
#include "Lz/detail/FakePointerProxy.hpp"
#include "Lz/detail/FunctionContainer.hpp"

#include <algorithm>

namespace lz {
namespace detail {
template<class Iterator, class S, class IteratorToExcept, class SentinelToExcept, class Compare>
class ExceptIterator
    : public IterBase<ExceptIterator<Iterator, S, IteratorToExcept, SentinelToExcept, Compare>, RefType<Iterator>,
                      FakePointerProxy<RefType<Iterator>>, DiffType<Iterator>, std::forward_iterator_tag, DefaultSentinel> {

    using IterTraits = std::iterator_traits<Iterator>;

public:
    using value_type = typename IterTraits::value_type;
    using difference_type = typename IterTraits::difference_type;
    using reference = typename IterTraits::reference;
    using pointer = FakePointerProxy<reference>;

private:
    Iterator _iterator{};
    IteratorToExcept _toExceptBegin{};
    mutable FunctionContainer<Compare> _compare{};
    SentinelToExcept _toExceptEnd{};
    S _end{};

    template<class T = Iterator>
    LZ_CONSTEXPR_CXX_20 EnableIf<std::is_same<T, S>::value && std::is_same<IteratorToExcept, SentinelToExcept>::value>
    findNext() {
        _iterator = std::find_if(std::move(_iterator), _end, [this](const value_type& value) {
            return !std::binary_search(_toExceptBegin, _toExceptEnd, value, _compare);
        });
    }

    template<class T = Iterator>
    LZ_CONSTEXPR_CXX_20 EnableIf<!std::is_same<T, S>::value && std::is_same<IteratorToExcept, SentinelToExcept>::value>
    findNext() {
        _iterator = detail::findIf(std::move(_iterator), _end, [this](const value_type& value) {
            return !std::binary_search(_toExceptBegin, _toExceptEnd, value, _compare);
        });
    }

    template<class T = Iterator>
    LZ_CONSTEXPR_CXX_20 EnableIf<std::is_same<T, S>::value && !std::is_same<IteratorToExcept, SentinelToExcept>::value>
    findNext() {
        _iterator = std::find_if(std::move(_iterator), _end, [this](const value_type& value) {
            return !detail::binarySearch(_toExceptBegin, _toExceptEnd, value, _compare);
        });
    }

    template<class T = Iterator>
    LZ_CONSTEXPR_CXX_20 EnableIf<!std::is_same<T, S>::value && !std::is_same<IteratorToExcept, SentinelToExcept>::value>
    findNext() {
        _iterator = detail::findIf(std::move(_iterator), _end, [this](const value_type& value) {
            return !detail::binarySearch(_toExceptBegin, _toExceptEnd, value, _compare);
        });
    }

public:
    constexpr ExceptIterator() = default;

    ExceptIterator(Iterator begin, S end, IteratorToExcept toExceptBegin, SentinelToExcept toExceptEnd, Compare compare) :
        _iterator(std::move(begin)),
        _toExceptBegin(std::move(toExceptBegin)),
        _compare(std::move(compare)),
        _toExceptEnd(std::move(toExceptEnd)),
        _end(std::move(end)) {
        if (_toExceptBegin == _toExceptEnd) {
            return;
        }
        findNext();
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 reference dereference() const {
        return *_iterator;
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 pointer arrow() const {
        return FakePointerProxy<decltype(**this)>(**this);
    }

    LZ_CONSTEXPR_CXX_20 void increment() {
        ++_iterator;
        findNext();
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 bool eq(const ExceptIterator& b) const {
        return _iterator == b._iterator;
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 bool eq(DefaultSentinel) const {
        return _iterator == _end;
    }
};
} // namespace detail
} // namespace lz

#endif