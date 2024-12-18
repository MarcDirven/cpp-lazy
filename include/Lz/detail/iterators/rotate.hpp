#pragma once

#ifndef LZ_ROTATE_ITERATOR_HPP
#define LZ_ROTATE_ITERATOR_HPP

#include "Lz/detail/traits.hpp"
#include "Lz/iterator_base.hpp"

#include <iterator>

namespace lz {
namespace detail {
// TODO make rotate_iterator random access if possible
template<class Iterator, class S>
class rotate_iterator : public iter_base<rotate_iterator<Iterator, S>, ref_t<Iterator>, ptr_t<Iterator>, diff_type<Iterator>,
                                         common_type<std::bidirectional_iterator_tag, iter_cat_t<Iterator>>,
                                         sentinel_selector<common_type<std::bidirectional_iterator_tag, iter_cat_t<Iterator>>,
                                                           rotate_iterator<Iterator, S>, Iterator>> {

    using traits = std::iterator_traits<Iterator>;

public:
    using reference = typename traits::reference;
    using value_type = typename traits::value_type;
    using pointer = typename traits::pointer;
    using difference_type = typename traits::difference_type;

private:
    Iterator _iterator{};
    Iterator _begin{};
    S _end{};
    bool _full_rotation{ false };

public:
    constexpr rotate_iterator() = default;

    LZ_CONSTEXPR_CXX_20 rotate_iterator(Iterator begin, S end, Iterator start, const bool full_rotation) :
        _iterator(std::move(start)),
        _begin(std::move(begin)),
        _end(std::move(end)),
        _full_rotation(full_rotation) {
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

    LZ_NODISCARD constexpr bool eq(const Iterator& start) const {
        return _iterator == start && _full_rotation;
    }
};
} // namespace detail
} // namespace lz

#endif // LZ_ROTATE_ITERATOR_HPP
