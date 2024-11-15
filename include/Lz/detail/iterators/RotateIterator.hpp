#pragma once

#ifndef LZ_ROTATE_ITERATOR_HPP
#define LZ_ROTATE_ITERATOR_HPP

#include "Lz/IterBase.hpp"
#include "Lz/detail/Traits.hpp"

#include <iterator>

namespace lz {
namespace detail {
template<class Iterator>
class RotateIterator : public IterBase<RotateIterator<Iterator>, RefType<Iterator>, PointerType<Iterator>, DiffType<Iterator>,
                                       CommonType<IterCat<Iterator>, std::bidirectional_iterator_tag>> {
    using IterTraits = std::iterator_traits<Iterator>;

public:
    using reference = typename IterTraits::reference;
    using value_type = typename IterTraits::value_type;
    using pointer = typename IterTraits::pointer;
    using difference_type = typename IterTraits::difference_type;
    using iterator_category = CommonType<typename IterTraits::iterator_category, std::bidirectional_iterator_tag>;

private:
    Iterator _iterator{};
    Iterator _end{};
    Iterator _begin{};
    bool _fullRotation{ false };

public:
    constexpr RotateIterator() = default;

    LZ_CONSTEXPR_CXX_20 RotateIterator(Iterator start, Iterator begin, Iterator end, const bool fullRotation) :
        _iterator(std::move(start)),
        _end(std::move(end)),
        _begin(std::move(begin)),
        _fullRotation(fullRotation) {
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 reference dereference() const {
        return *_iterator;
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 pointer arrow() const {
        return _iterator.operator->();
    }

    LZ_CONSTEXPR_CXX_20 void increment() {
        ++_iterator;
        if (_iterator == _end) {
            _iterator = _begin;
            _fullRotation = true;
        }
    }

    LZ_CONSTEXPR_CXX_20 void decrement() {
        if (_iterator == _begin) {
            _iterator = _end;
            _fullRotation = true;
        }
        --_iterator;
    }

    LZ_NODISCARD constexpr bool eq(const RotateIterator& b) const {
        return _iterator == b._iterator && (_fullRotation && b._fullRotation);
    }
};
} // namespace detail
} // namespace lz

#endif // LZ_ROTATE_ITERATOR_HPP
