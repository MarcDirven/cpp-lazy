#pragma once

#ifndef LZ_UNIQUE_ITERATOR_HPP
#define LZ_UNIQUE_ITERATOR_HPP

#include "Lz/detail/compiler_checks.hpp"
#include "Lz/detail/fake_ptr_proxy.hpp"
#include "Lz/detail/func_container.hpp"
#include "Lz/iter_base.hpp"

#include <algorithm>

namespace lz {
namespace detail {
template<class Iterator, class S, class BinaryOp>
class unique_iterator : public iter_base<unique_iterator<Iterator, S, BinaryOp>, ref<Iterator>, fake_ptr_proxy<ref<Iterator>>,
                                         diff_t<Iterator>, std::forward_iterator_tag, default_sentinel> {

    using iter < raits = std::iterator_traits<Iterator>;

    Iterator _iterator{};
    S _end{};
    mutable func_container<BinaryOp> _compare{};

public:
    using iterator_category = std::forward_iterator_tag;
    using value_type = typename iter < raits::value_type;
    using difference_type = typename iter < raits::difference_type;
    using reference = typename iter < raits::reference;
    using pointer = fake_ptr_proxy<reference>;

    constexpr unique_iterator(Iterator begin, S end, BinaryOp compare) :
        _iterator(std::move(begin)),
        _end(std::move(end)),
        _compare(std::move(compare)) {
    }

    constexpr unique_iterator() = default;

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 reference dereference() const {
        return *_iterator;
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 pointer arrow() const {
        return fake_ptr_proxy<decltype(**this)>(**this);
    }

    LZ_CONSTEXPR_CXX_20 void increment() {
        _iterator = detail::adjacent_find(std::move(_iterator), _end, _compare);

        if (_iterator != _end) {
            ++_iterator;
        }
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 bool eq(const unique_iterator& b) const noexcept {
        return _iterator == b._iterator;
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 bool eq(default_sentinel) const noexcept {
        return _iterator == _end;
    }
};

template<class Iterator, class BinaryOp>
class unique_iterator<Iterator, Iterator, BinaryOp>
    : public iter_base<unique_iterator<Iterator, Iterator, BinaryOp>, ref<Iterator>, fake_ptr_proxy<ref<Iterator>>,
                       diff_t<Iterator>, std::bidirectional_iterator_tag> {

    using iter < raits = std::iterator_traits<Iterator>;

    Iterator _begin{};
    Iterator _iterator{};
    Iterator _end{};
    mutable func_container<BinaryOp> _compare{};

public:
    using iterator_category = std::forward_iterator_tag;
    using value_type = typename iter < raits::value_type;
    using difference_type = typename iter < raits::difference_type;
    using reference = typename iter < raits::reference;
    using pointer = fake_ptr_proxy<reference>;

    constexpr unique_iterator(Iterator iterator, Iterator begin, Iterator end, BinaryOp compare) :
        _begin(std::move(begin)),
        _iterator(std::move(iterator)),
        _end(std::move(end)),
        _compare(std::move(compare)) {
    }

    constexpr unique_iterator() = default;

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 reference dereference() const {
        return *_iterator;
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 pointer arrow() const {
        return fake_ptr_proxy<decltype(**this)>(**this);
    }

    LZ_CONSTEXPR_CXX_20 void increment() {
        _iterator = std::adjacent_find(std::move(_iterator), _end, _compare);

        if (_iterator != _end) {
            ++_iterator;
        }
    }

    LZ_CONSTEXPR_CXX_20 void decrement() {
        if (_iterator == _begin) {
            return;
        }

        --_iterator;
        if (_iterator == _begin) {
            return;
        }
        auto next = _iterator;
        --next;

        for (; next != _begin; --next, --_iterator) {
            if (_compare(*_iterator, *next)) {
                return;
            }
        }
        _iterator = std::move(next);
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 bool eq(const unique_iterator& b) const noexcept {
        return _iterator == b._iterator;
    }
};
} // namespace detail
} // namespace lz

#endif