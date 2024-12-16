#pragma once

#ifndef LZ_INCLUSIVE_SCAN_ITERATOR_HPP
#define LZ_INCLUSIVE_SCAN_ITERATOR_HPP

#include "Lz/detail/fake_ptr_proxy.hpp"
#include "Lz/detail/func_container.hpp"
#include "Lz/iter_base.hpp"

namespace lz {
namespace detail {
template<class Iterator, class S, class T, class BinaryOp>
class inclusive_scan_iterator : public iter_base<inclusive_scan_iterator<Iterator, S, T, BinaryOp>, T&, fake_ptr_proxy<T&>,
                                                 diff_type<Iterator>, std::forward_iterator_tag, default_sentinel> {

    mutable T _reducer{};
    Iterator _iterator{};
    S _end{};
    func_container<BinaryOp> _binary_op{};

    using iter < raits = std::iterator_traits<Iterator>;

public:
    using reference = T&;
    using value_type = T;
    using pointer = fake_ptr_proxy<reference>;
    using difference_type = typename iter < raits::difference_type;
    using iterator_category = std::forward_iterator_tag;

    constexpr inclusive_scan_iterator() = default;

    LZ_CONSTEXPR_CXX_14 inclusive_scan_iterator(Iterator iterator, S end, T init, BinaryOp bin_op) :
        _reducer(std::move(init)),
        _iterator(std::move(iterator)),
        _end(std::move(end)),
        _binary_op(std::move(bin_op)) {
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 reference dereference() const {
        return _reducer;
    }

    LZ_CONSTEXPR_CXX_20 void increment() {
        ++_iterator;
        if (_iterator != _end) {
            _reducer = _binary_op(std::move(_reducer), *_iterator);
        }
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 pointer arrow() const {
        return fake_ptr_proxy<decltype(**this)>(**this);
    }

    LZ_NODISCARD constexpr bool eq(const inclusive_scan_iterator& b) const {
        return _iterator == b._iterator;
    }

    LZ_NODISCARD constexpr bool eq(default_sentinel) const {
        return _iterator == _end;
    }
};
} // namespace detail
} // namespace lz

#endif