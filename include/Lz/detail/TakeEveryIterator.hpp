#pragma once

#ifndef LZ_TAKE_EVERY_ITERATOR_HPP
#    define LZ_TAKE_EVERY_ITERATOR_HPP

#    include "LzTools.hpp"

namespace lz {
namespace internal {
template<class, bool>
class TakeEveryIterator;

template<class Iterator>
class TakeEveryIterator<Iterator, false /* isBidirectional */> {
    using IterTraits = std::iterator_traits<Iterator>;

public:
    using value_type = typename IterTraits::value_type;
    using iterator_category = typename std::common_type<std::forward_iterator_tag, typename IterTraits::iterator_category>::type;
    using difference_type = typename IterTraits::difference_type;
    using reference = typename IterTraits::reference;
    using pointer = FakePointerProxy<reference>;

    Iterator _iterator{};
    Iterator _end{};
    difference_type _offset{};

    void advance() noexcept {
        for (difference_type count = 0; _iterator != _end && count < _offset; ++_iterator, ++count) {
        }
    }

public:
    LZ_CONSTEXPR_CXX_20
    TakeEveryIterator(Iterator iterator, Iterator end, const difference_type offset) noexcept :
        _iterator(std::move(iterator)),
        _end(std::move(end)),
        _offset(offset) {
    }

    constexpr TakeEveryIterator() = default;

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 reference operator*() const {
        return *_iterator;
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 pointer operator->() const {
        return FakePointerProxy<decltype(**this)>(**this);
    }

    LZ_CONSTEXPR_CXX_20 TakeEveryIterator& operator++() noexcept {
        advance();
        return *this;
    }

    LZ_CONSTEXPR_CXX_20 TakeEveryIterator operator++(int) {
        TakeEveryIterator tmp(*this);
        ++*this;
        return tmp;
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
class TakeEveryIterator<Iterator, true /* isBidirectional */> {
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

#    ifdef __cpp_if_constexpr
    LZ_CONSTEXPR_CXX_20 void advance() noexcept {
        if constexpr (IsRandomAccess<Iterator>::value) {
            const auto distance = _end - _iterator;
            if (_offset >= distance) {
                _iterator = _end;
            }
            else {
                _iterator += _offset;
            }
        }
        else {
            for (difference_type count = 0; _iterator != _end && count < _offset; ++_iterator, ++count) {
            }
        }
    }

    LZ_CONSTEXPR_CXX_20 void previous() noexcept {
        if constexpr (IsRandomAccess<Iterator>::value) {
            const auto distance = _iterator - _begin;
            if (_offset >= distance) {
                _iterator = _begin;
            }
            else {
                _iterator -= _offset;
            }
        }
        else {
            for (difference_type count = _offset; _iterator != _begin && count >= 0; --_iterator, --count) {
            }
        }
    }
#    else
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
#    endif

public:
    LZ_CONSTEXPR_CXX_20
    TakeEveryIterator(Iterator iterator, Iterator begin, Iterator end, const difference_type offset) :
        _begin(std::move(begin)),
        _iterator(std::move(iterator)),
        _end(std::move(end)),
        _offset(offset) {
    }

    constexpr TakeEveryIterator() = default;

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 reference operator*() const {
        return *_iterator;
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 pointer operator->() const {
        return FakePointerProxy<decltype(**this)>(**this);
    }

    LZ_CONSTEXPR_CXX_20 TakeEveryIterator& operator++() {
        advance();
        return *this;
    }

    LZ_CONSTEXPR_CXX_20 TakeEveryIterator operator++(int) {
        TakeEveryIterator tmp(*this);
        ++*this;
        return tmp;
    }

    LZ_CONSTEXPR_CXX_20 TakeEveryIterator& operator--() {
        if (_iterator == _end) {
            --_iterator;
        }
        else {
            previous();
        }
        return *this;
    }

    LZ_CONSTEXPR_CXX_20 TakeEveryIterator operator--(int) {
        TakeEveryIterator tmp(*this);
        --*this;
        return tmp;
    }
    LZ_CONSTEXPR_CXX_20 TakeEveryIterator& operator+=(difference_type offset) {
        difference_type extra = 0;
        if (_iterator == _end && offset < 0) {
            --_iterator;
            if (offset == -1) {
                return *this;
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
        return *this;
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 TakeEveryIterator operator+(const difference_type offset) const {
        TakeEveryIterator tmp(*this);
        tmp += offset;
        return tmp;
    }

    LZ_CONSTEXPR_CXX_20 TakeEveryIterator& operator-=(const difference_type offset) {
        return *this += -offset;
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 TakeEveryIterator operator-(const difference_type offset) const {
        TakeEveryIterator tmp(*this);
        tmp -= offset;
        return tmp;
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 friend difference_type
    operator-(const TakeEveryIterator& a, const TakeEveryIterator& b) {
        LZ_ASSERT(a._offset == b._offset, "incompatible iterator types: different offsets");
        return roundEven(a._iterator - b._iterator, a._offset);
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 reference operator[](const difference_type offset) const {
        return *(*this + offset);
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 friend bool operator==(const TakeEveryIterator& a, const TakeEveryIterator& b) noexcept {
        return !(a != b); // NOLINT
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 friend bool operator!=(const TakeEveryIterator& a, const TakeEveryIterator& b) noexcept {
        LZ_ASSERT(a._offset == b._offset, "incompatible iterator types: different offsets");
        return a._iterator != b._iterator;
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 friend bool operator<(const TakeEveryIterator& a, const TakeEveryIterator& b) {
        LZ_ASSERT(a._offset == b._offset, "incompatible iterator types: different offsets");
        return a._iterator < b._iterator;
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 friend bool operator>(const TakeEveryIterator& a, const TakeEveryIterator& b) {
        return b < a; // NOLINT
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 friend bool operator<=(const TakeEveryIterator& a, const TakeEveryIterator& b) {
        return !(b < a); // NOLINT
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 friend bool operator>=(const TakeEveryIterator& a, const TakeEveryIterator& b) {
        return !(a < b); // NOLINT
    }
};
} // namespace internal
} // namespace lz

#endif