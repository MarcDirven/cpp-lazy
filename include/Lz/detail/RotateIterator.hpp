#pragma once

#ifndef LZ_ROTATE_ITERATOR_HPP
#    define LZ_ROTATE_ITERATOR_HPP

#    include <iterator>

namespace lz {
namespace internal {
template<class Iterator>
class RotateIterator {
    using IterTraits = std::iterator_traits<Iterator>;

public:
    using reference = typename IterTraits::reference;
    using value_type = typename IterTraits::value_type;
    using pointer = typename IterTraits::pointer;
    using difference_type = typename IterTraits::difference_type;
    using iterator_category = typename std::common_type<typename IterTraits::iterator_category, std::bidirectional_iterator_tag>::type;

private:
    Iterator _iterator{};
    Iterator _end{};
    Iterator _begin{};
    bool _fullRotation{false};

public:
    LZ_CONSTEXPR_CXX_20 RotateIterator(Iterator start, Iterator begin, Iterator end, const bool fullRotation) :
        _iterator(std::move(start)),
        _end(std::move(end)),
        _begin(std::move(begin)),
        _fullRotation(fullRotation) {
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 reference operator*() const {
        return *_iterator;
    }

    LZ_CONSTEXPR_CXX_20 RotateIterator& operator++() {
        ++_iterator;
        if (_iterator == _end) {
            _iterator = _begin;
            _fullRotation = true;
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
            _fullRotation = true;
        }
        --_iterator;
        return *this;
    }

    LZ_CONSTEXPR_CXX_20 RotateIterator operator--(int) {
        RotateIterator tmp(*this);
        --*this;
        return tmp;
    }

    LZ_NODISCARD constexpr friend bool operator!=(const RotateIterator& a, const RotateIterator& b) {
        return a._iterator != b._iterator || !(a._fullRotation && b._fullRotation);
    }

    LZ_NODISCARD constexpr friend bool operator==(const RotateIterator& a, const RotateIterator& b) {
        return !(a != b); // NOLINT
    }
};
} // namespace internal
} // namespace lz

#endif // LZ_ROTATE_ITERATOR_HPP
