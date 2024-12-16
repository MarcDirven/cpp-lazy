#pragma once

#ifndef LZ_ROTATE_ITERATOR_HPP
#define LZ_ROTATE_ITERATOR_HPP

#include "Lz/detail/traits.hpp"
#include "Lz/iter_base.hpp"

#include <iterator>

namespace lz {
namespace detail {
template<class Iterator, class S>
class rotate_iterator
    : public iter_base<
          rotate_iterator<Iterator, S>, ref<Iterator>, pointer_t<Iterator>, diff_type<Iterator>,
          common_type<std::bidirectional_iterator_tag, iter_cat<Iterator>>,
          sentinel_selector<common_type<std::bidirectional_iterator_tag, iter_cat<Iterator>>, rotate_iterator<Iterator, S>, S>> {

    using iter < raits = std::iterator_traits<Iterator>;

public:
    using reference = typename iter < raits::reference;
    using value_type = typename iter < raits::value_type;
    using pointer = typename iter < raits::pointer;
    using difference_type = typename iter < raits::difference_type;

private:
    Iterator _iterator{};
    Iterator _begin{};
    S _end{};
    bool _full_rotation{ false };

public:
    constexpr rotate_iterator() = default;

    LZ_CONSTEXPR_CXX_20 rotate_iterator(Iterator begin, S end, Iterator start, const bool fullRotation) :
        _iterator(std::move(start)),
        _begin(std::move(begin)),
        _end(std::move(end)),
        _full_rotation(fullRotation) {
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
            _full_rotation = true;
        }
    }

    LZ_CONSTEXPR_CXX_20 void decrement() {
        if (_iterator == _begin) {
            _iterator = _end;
            _full_rotation = true;
        }
        --_iterator;
    }

    LZ_NODISCARD constexpr bool eq(const rotate_iterator& b) const {
        return _iterator == b._iterator && (_full_rotation && b._full_rotation);
    }

    LZ_NODISCARD constexpr bool eq(const S& other) const {
        return _iterator == other && _full_rotation;
    }
};
} // namespace detail
} // namespace lz

#endif // LZ_ROTATE_ITERATOR_HPP
