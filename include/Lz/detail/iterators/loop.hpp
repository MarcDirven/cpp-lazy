#pragma once

#ifndef LZ_LOOP_ITERATOR_HPP
#define LZ_LOOP_ITERATOR_HPP

#include "Lz/detail/compiler_checks.hpp"
#include "Lz/detail/fake_ptr_proxy.hpp"
#include "Lz/detail/traits.hpp"
#include "Lz/iter_base.hpp"

namespace lz {
namespace detail {
template<class Iterator, class S>
class loop_iterator
    : public iter_base<loop_iterator<Iterator, S>, ref<Iterator>, fake_ptr_proxy<ref<Iterator>>, diff_type<Iterator>,
                       iter_cat<Iterator>, sentinel_selector<iter_cat<Iterator>, loop_iterator<Iterator, S>>> {

    using iter < raits = std::iterator_traits<Iterator>;

    Iterator _begin{};
    Iterator _iterator{};
    S _end{};

public:
    using reference = typename iter < raits::reference;
    using value_type = typename iter < raits::value_type;
    using pointer = fake_ptr_proxy<reference>;
    using difference_type = typename iter < raits::difference_type;
    using iterator_category = typename iter < raits::iterator_category;

    LZ_CONSTEXPR_CXX_20 loop_iterator(Iterator iterator, Iterator begin, S end) :
        _begin(std::move(begin)),
        _iterator(std::move(iterator)),
        _end(std::move(end)) {
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 reference dereference() const {
        return *_iterator;
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 pointer arrow() const {
        return fake_ptr_proxy<decltype(**this)>(**this);
    }

    LZ_CONSTEXPR_CXX_20 void increment() {
        ++_iterator;
        if (_iterator == _end) {
            _iterator = _begin;
        }
    }

    LZ_CONSTEXPR_CXX_20 void decrement() {
        if (_iterator == _begin) {
            _iterator = _end;
        }
        --_iterator;
    }

    LZ_CONSTEXPR_CXX_20 void plus_is(difference_type offset) {
        if (offset >= 0) {
            _iterator += offset % (_end - _begin);
            return;
        }
        offset *= -1;
        const auto dist = _end - _begin;
        offset %= dist;
        _iterator -= offset == 0 ? dist : offset;
    }

    LZ_NODISCARD constexpr difference_type difference(const loop_iterator& it) const noexcept {
        if (it._iterator == _end) {
            return 0;
        }
        return (std::numeric_limits<difference_type>::max)();
    }

    LZ_NODISCARD constexpr bool eq(const loop_iterator&) const noexcept {
        return false;
    }

    LZ_NODISCARD constexpr bool eq(default_sentinel) const noexcept {
        return false;
    }
};
} // namespace detail
} // namespace lz

#endif // LZ_LOOP_ITERATOR_HPP