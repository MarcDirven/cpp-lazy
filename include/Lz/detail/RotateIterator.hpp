#pragma once

#ifndef LZ_ROTATE_ITERATOR_HPP
#    define LZ_ROTATE_ITERATOR_HPP

#    include <iterator>

namespace lz {
namespace internal {
template<class Iterator, bool>
class RotateIterator;

template<class Iterator>
class RotateIterator<Iterator, true /* isRandomAccess */> {
    using IterTraits = std::iterator_traits<Iterator>;

public:
    using reference = typename IterTraits::reference;
    using value_type = typename IterTraits::value_type;
    using pointer = typename IterTraits::pointer;
    using difference_type = typename IterTraits::difference_type;
    using iterator_category = typename IterTraits::iterator_category;

private:
    Iterator _iterator{};
    Iterator _end{};
    Iterator _begin{};
    difference_type _start{};
    difference_type _current{};
    difference_type _distance{};

public:
    LZ_CONSTEXPR_CXX_20 RotateIterator(Iterator begin, Iterator end, Iterator iterator, const difference_type start,
                                       const difference_type current, const difference_type distance) :
        _iterator(std::move(iterator)),
        _end(std::move(end)),
        _begin(std::move(begin)),
        _start(start),
        _current(current),
        _distance(distance) {
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 reference operator*() const {
        return *_iterator;
    }

    LZ_CONSTEXPR_CXX_20 RotateIterator& operator++() {
        ++_iterator;
        ++_current;
        if (_iterator == _end) {
            _iterator = _begin;
        }
        return *this;
    }

    LZ_CONSTEXPR_CXX_20 RotateIterator operator++(int) {
        RotateIterator tmp(*this);
        ++*this;
        return tmp;
    }

    LZ_CONSTEXPR_CXX_20 RotateIterator& operator--() {
        if (_iterator == _begin) {
            _iterator = _end;
        }
        --_iterator;
        --_current;
        return *this;
    }

    LZ_CONSTEXPR_CXX_20 RotateIterator operator--(int) {
        RotateIterator tmp(*this);
        --*this;
        return tmp;
    }

    LZ_CONSTEXPR_CXX_20 RotateIterator& operator+=(difference_type offset) {
        if (offset < 0) {
            offset = _distance + offset;
        }
        _current += offset;
        _iterator = _begin + (_start + _current) % _distance;
        return *this;
    }

    LZ_CONSTEXPR_CXX_20 RotateIterator& operator-=(const difference_type offset) {
        return *this += -offset;
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 RotateIterator operator+(const difference_type offset) const {
        RotateIterator tmp(*this);
        tmp += offset;
        return tmp;
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 RotateIterator operator-(const difference_type offset) const {
        RotateIterator tmp(*this);
        tmp -= offset;
        return tmp;
    }

    LZ_NODISCARD constexpr friend difference_type operator-(const RotateIterator& a, const RotateIterator& b) {
        return a._current - b._current;
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 reference operator[](const difference_type offset) const {
        return *(*this + offset);
    }

    LZ_NODISCARD constexpr friend bool operator!=(const RotateIterator& a, const RotateIterator& b) noexcept {
        return a._current != b._current;
    }

    LZ_NODISCARD constexpr friend bool operator==(const RotateIterator& a, const RotateIterator& b) noexcept {
        return !(a != b); // NOLINT
    }

    LZ_NODISCARD constexpr friend bool operator<(const RotateIterator& a, const RotateIterator& b) {
        return a._current < b._current;
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 friend bool operator>(const RotateIterator& a, const RotateIterator& b) {
        return b < a;
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 friend bool operator<=(const RotateIterator& a, const RotateIterator& b) {
        return !(b < a); // NOLINT
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 friend bool operator>=(const RotateIterator& a, const RotateIterator& b) {
        return !(a < b); // NOLINT
    }
};

template<class Iterator>
class RotateIterator<Iterator, false /* isRandomAccess */> {
    using IterTraits = std::iterator_traits<Iterator>;

public:
    using reference = typename IterTraits::reference;
    using value_type = typename IterTraits::value_type;
    using pointer = FakePointerProxy<reference>;
    using difference_type = typename IterTraits::difference_type;
    using iterator_category = typename IterTraits::iterator_category;

private:
    Iterator _iterator{};
    Iterator _end{};
    Iterator _begin{};
    difference_type _current{};

public:
    LZ_CONSTEXPR_CXX_20 RotateIterator(Iterator begin, Iterator end, Iterator iterator, const difference_type current) :
        _iterator(std::move(iterator)),
        _end(std::move(end)),
        _begin(std::move(begin)),
        _current(current) {
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 reference operator*() const {
        return *_iterator;
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 pointer operator->() const {
        return FakePointerProxy<decltype(**this)>(**this);
    }

    LZ_CONSTEXPR_CXX_20 RotateIterator& operator++() {
        ++_iterator;
        ++_current;
        if (_iterator == _end) {
            _iterator = _begin;
        }
        return *this;
    }

    LZ_CONSTEXPR_CXX_20 RotateIterator operator++(int) {
        RotateIterator tmp(*this);
        ++*this;
        return tmp;
    }

    LZ_CONSTEXPR_CXX_20 RotateIterator& operator--() {
        if (_iterator == _begin) {
            _iterator = _end;
        }
        --_iterator;
        --_current;
        return *this;
    }

    LZ_CONSTEXPR_CXX_20 RotateIterator operator--(int) {
        RotateIterator tmp(*this);
        --*this;
        return tmp;
    }

    LZ_NODISCARD constexpr friend difference_type operator-(const RotateIterator& a, const RotateIterator& b) {
        return a._current - b._current;
    }

    LZ_NODISCARD constexpr friend bool operator!=(const RotateIterator& a, const RotateIterator& b) noexcept {
        return a._current != b._current;
    }

    LZ_NODISCARD constexpr friend bool operator==(const RotateIterator& a, const RotateIterator& b) noexcept {
        return !(a != b); // NOLINT
    }
};
} // namespace internal

template<LZ_CONCEPT_ITERATOR Iterator>
LZ_NODISCARD LZ_CONSTEXPR_CXX_14 typename internal::RotateIterator<Iterator, false>::difference_type
distance(const internal::RotateIterator<Iterator, false>& a, const internal::RotateIterator<Iterator, false>& b) {
    return b - a;
}

} // namespace lz

#endif // LZ_ROTATE_ITERATOR_HPP
