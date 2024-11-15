#pragma once

#ifndef LZ_TAKE_EVERY_ITERATOR_HPP
#define LZ_TAKE_EVERY_ITERATOR_HPP

#include "Lz/IterBase.hpp"
#include "Lz/detail/CompilerChecks.hpp"
#include "Lz/detail/FakePointerProxy.hpp"
#include "Lz/detail/Traits.hpp"

namespace lz {
namespace detail {

template<class, bool>
class TakeEveryIterator;

template<class Iterator>
class TakeEveryIterator<Iterator, false /* isBidirectional */>
    : public IterBase<TakeEveryIterator<Iterator, false>, RefType<Iterator>, FakePointerProxy<RefType<Iterator>>,
                      DiffType<Iterator>, std::forward_iterator_tag> {
    using IterTraits = std::iterator_traits<Iterator>;

public:
    using value_type = typename IterTraits::value_type;
    using iterator_category = std::forward_iterator_tag;
    using difference_type = typename IterTraits::difference_type;
    using reference = typename IterTraits::reference;
    using pointer = FakePointerProxy<reference>;

    Iterator _iterator{};
    Iterator _end{};
    difference_type _offset{};

public:
    LZ_CONSTEXPR_CXX_20
    TakeEveryIterator(Iterator iterator, Iterator end, const difference_type offset) :
        _iterator(std::move(iterator)),
        _end(std::move(end)),
        _offset(offset) {
    }

    constexpr TakeEveryIterator() = default;

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 reference dereference() const {
        return *_iterator;
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 pointer arrow() const {
        return FakePointerProxy<decltype(**this)>(**this);
    }

    LZ_CONSTEXPR_CXX_20 void increment() noexcept {
        for (difference_type count = 0; _iterator != _end && count < _offset; ++_iterator, ++count) {
        }
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 friend bool operator==(const TakeEveryIterator& a, const TakeEveryIterator& b) noexcept {
        return !(a != b); // NOLINT
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 friend bool operator!=(const TakeEveryIterator& a, const TakeEveryIterator& b) noexcept {
        LZ_ASSERT(a._offset == b._offset, "incompatible iterator types: different offsets");
        return a._iterator != b._iterator;
    }
};

template<class Iterator>
class TakeEveryIterator<Iterator, true /* isBidirectional */>
    : public IterBase<TakeEveryIterator<Iterator, true>, RefType<Iterator>, FakePointerProxy<RefType<Iterator>>,
                      DiffType<Iterator>, IterCat<Iterator>> {
    using IterTraits = std::iterator_traits<Iterator>;

public:
    using value_type = typename IterTraits::value_type;
    using iterator_category = typename IterTraits::iterator_category;
    using difference_type = typename IterTraits::difference_type;
    using reference = typename IterTraits::reference;
    using pointer = FakePointerProxy<reference>;

    Iterator _begin{};
    Iterator _iterator{};
    Iterator _end{};
    difference_type _offset{};

#ifdef __cpp_if_constexpr
    LZ_CONSTEXPR_CXX_20 void advance() noexcept {
        if constexpr (!IsRandomAccess<Iterator>::value) {
            for (difference_type count = 0; _iterator != _end && count < _offset; ++_iterator, ++count) {
            }
            return;
        }
        const auto distance = _end - _iterator;
        if (_offset >= distance) {
            _iterator = _end;
        }
        else {
            _iterator += _offset;
        }
    }

    LZ_CONSTEXPR_CXX_20 void previous() noexcept {
        if constexpr (!IsRandomAccess<Iterator>::value) {
            for (difference_type count = _offset; _iterator != _begin && count >= 0; --_iterator, --count) {
            }
            return;
        }
        const auto distance = _iterator - _begin;
        if (_offset >= distance) {
            _iterator = _begin;
        }
        else {
            _iterator -= _offset;
        }
    }
#else
    template<class I = Iterator>
    LZ_CONSTEXPR_CXX_20 EnableIf<IsRandomAccess<I>::value, void> advance() {
        const auto distance = _end - _iterator;
        if (_offset >= distance) {
            _iterator = _end;
        }
        else {
            _iterator += _offset;
        }
    }

    template<class I = Iterator>
    LZ_CONSTEXPR_CXX_20 EnableIf<!IsRandomAccess<I>::value> advance() {
        for (difference_type count = 0; _iterator != _end && count < _offset; ++_iterator, ++count) {
        }
    }

    template<class I = Iterator>
    LZ_CONSTEXPR_CXX_20 EnableIf<IsRandomAccess<I>::value> previous() {
        const auto distance = _iterator - _begin;
        if (_offset >= distance) {
            _iterator = _begin;
        }
        else {
            _iterator -= _offset;
        }
    }

    template<class I = Iterator>
    LZ_CONSTEXPR_CXX_20 EnableIf<!IsRandomAccess<I>::value> previous() {
        for (difference_type count = _offset; _iterator != _begin && count >= 0; --_iterator, --count) {
        }
    }
#endif

public:
    LZ_CONSTEXPR_CXX_20
    TakeEveryIterator(Iterator iterator, Iterator begin, Iterator end, const difference_type offset) :
        _begin(std::move(begin)),
        _iterator(std::move(iterator)),
        _end(std::move(end)),
        _offset(offset) {
    }

    constexpr TakeEveryIterator() = default;

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 reference dereference() const {
        return *_iterator;
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 pointer arrow() const {
        return FakePointerProxy<decltype(**this)>(**this);
    }

    LZ_CONSTEXPR_CXX_20 void increment() {
        advance();
    }

    LZ_CONSTEXPR_CXX_20 void decrement() {
        if (_iterator == _end) {
            --_iterator;
        }
        else {
            previous();
        }
    }

    LZ_CONSTEXPR_CXX_20 void plusIs(const difference_type offset) {
        difference_type extra = 0;
        if (_iterator == _end && offset < 0) {
            --_iterator;
            if (offset == -1) {
                return;
            }
            extra = _offset;
        }
        const auto toAdd = _offset * offset + extra;
        if (toAdd < 0) {
            const auto remaining = _begin - _iterator;
            const auto remainingPos = -remaining;
            _iterator += -toAdd >= remainingPos ? remaining : toAdd;
        }
        else {
            const auto remaining = _end - _iterator;
            _iterator += toAdd >= remaining ? remaining : toAdd;
        }
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 difference_type difference(const TakeEveryIterator& other) const {
        LZ_ASSERT(_offset == other._offset, "incompatible iterator types: different offsets");
        if (_offset == 0) {
            return _iterator - other._iterator;
        }
        const auto rawDifference = _iterator - other._iterator;
        if (rawDifference == 0) {
            return 0;
        }
        return (rawDifference + (_offset - 1)) / _offset;
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 bool eq(const TakeEveryIterator& b) const noexcept {
        LZ_ASSERT(_offset == b._offset, "incompatible iterator types: different offsets");
        return _iterator == b._iterator;
    }
};
} // namespace detail
} // namespace lz

#endif