#pragma once

#ifndef LZ_LOOP_ITERATOR_HPP
#    define LZ_LOOP_ITERATOR_HPP


#    include "LzTools.hpp"

namespace lz { namespace internal {
template<class Iterator>
class LoopIterator {
    using IterTraits = std::iterator_traits<Iterator>;

    Iterator _begin{};
    Iterator _iterator{};
    Iterator _end{};

public:
    using reference = typename IterTraits::reference;
    using value_type = typename IterTraits::value_type;
    using pointer = FakePointerProxy<reference>;
    using difference_type = typename IterTraits::difference_type;
    using iterator_category = typename IterTraits::iterator_category;

    LZ_CONSTEXPR_CXX_20 LoopIterator(Iterator iterator, Iterator begin, Iterator end):
        _begin(std::move(begin)),
        _iterator(std::move(iterator)),
        _end(std::move(end))
    {}

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 reference operator*() const {
        return *_iterator;
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 pointer operator->() const {
        return FakePointerProxy<decltype(**this)>(**this);
    }

    LZ_CONSTEXPR_CXX_20 LoopIterator& operator++() {
        ++_iterator;
        if (_iterator == _end) {
            _iterator = _begin;
        }
        return *this;
    }

    LZ_CONSTEXPR_CXX_20 LoopIterator operator++(int) {
        LoopIterator tmp(*this);
        ++*this;
        return tmp;
    }

    LZ_CONSTEXPR_CXX_20 LoopIterator& operator--() {
        if (_iterator == _begin) {
            _iterator = _end;
        }
        --_iterator;
        return *this;
    }

    LZ_CONSTEXPR_CXX_20 LoopIterator operator--(int) {
        LoopIterator tmp(*this);
        --*this;
        return tmp;
    }

    LZ_CONSTEXPR_CXX_20 LoopIterator& operator+=(const difference_type offset) {
        _iterator += offset % (_end - _begin);
        return *this;
    }

    LZ_CONSTEXPR_CXX_20 LoopIterator& operator-=(difference_type offset) {
        const auto dist = _end - _begin;
        offset %= dist;
        _iterator -= offset == 0 ? dist : offset;
        return *this;
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 LoopIterator operator+(const difference_type offset) const {
        LoopIterator tmp(*this);
        tmp += offset;
        return tmp;
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 LoopIterator operator-(const difference_type offset) const {
        LoopIterator tmp(*this);
        tmp -= offset;
        return tmp;
    }

    LZ_NODISCARD constexpr friend difference_type operator-(const LoopIterator&, const LoopIterator&) noexcept {
        return (std::numeric_limits<difference_type>::max)();
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 reference operator[](const difference_type offset) const {
        return *(*this + offset);
    }

    LZ_NODISCARD constexpr friend bool operator!=(const LoopIterator&, const LoopIterator&) noexcept {
        return true;
    }

    LZ_NODISCARD constexpr friend bool operator==(const LoopIterator& a, const LoopIterator& b) noexcept {
        return !(a != b); // NOLINT
    }

    LZ_NODISCARD constexpr friend bool operator<(const LoopIterator&, const LoopIterator&) noexcept {
        return true;
    }

    LZ_NODISCARD constexpr friend bool operator>(const LoopIterator&, const LoopIterator&) noexcept {
        return true;
    }

    LZ_NODISCARD constexpr friend bool operator<=(const LoopIterator&, const LoopIterator&) noexcept {
        return true;
    }

    LZ_NODISCARD constexpr friend bool operator>=(const LoopIterator&, const LoopIterator&) noexcept {
        return true;
    }
};
}}

#endif // LZ_LOOP_ITERATOR_HPP