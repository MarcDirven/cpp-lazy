#pragma once

#ifndef LZ_FILTER_ITERATOR_HPP
#define LZ_FILTER_ITERATOR_HPP

#include "Lz/detail/algorithm.hpp"
#include "Lz/detail/fake_ptr_proxy.hpp"
#include "Lz/detail/func_container.hpp"
#include "Lz/iter_base.hpp"

#include <algorithm>

namespace lz {
namespace detail {
template<class Iterator, class S, class UnaryPredicate>
class filter_iterator;

template<class Iterator, class S, class UnaryPredicate>
class filter_iterator
    : public iter_base<filter_iterator<Iterator, S, UnaryPredicate>, ref_t<Iterator>, fake_ptr_proxy<ref_t<Iterator>>,
                       diff_type<Iterator>, common_type<std::bidirectional_iterator_tag, iter_cat_t<Iterator>>,
                       sentinel_selector<iter_cat_t<Iterator>, filter_iterator<Iterator, S, UnaryPredicate>>> {

    using traits = std::iterator_traits<Iterator>;

public:
    using value_type = typename traits::value_type;
    using difference_type = typename traits::difference_type;
    using reference = typename traits::reference;
    using pointer = fake_ptr_proxy<reference>;

    template<class I = Iterator>
    LZ_CONSTEXPR_CXX_20 enable_if<std::is_same<I, S>::value, I> find(I first, I last) {
        return std::find_if(std::move(first), std::move(last), _predicate);
    }

    template<class I = Iterator>
    LZ_CONSTEXPR_CXX_20 enable_if<!std::is_same<I, S>::value, I> find(I first, S last) {
        return detail::find_if(std::move(first), std::move(last), _predicate);
    }

private:
    Iterator _begin{};
    Iterator _iterator{};
    S _end{};
    mutable func_container<UnaryPredicate> _predicate{};

public:
    LZ_CONSTEXPR_CXX_20 filter_iterator(Iterator iterator, Iterator begin, S end, UnaryPredicate up) :
        _begin(std::move(begin)),
        _iterator(std::move(iterator)),
        _end(std::move(end)),
        _predicate(std::move(up)) {
        if (_iterator == _begin) {
            _iterator = find(std::move(_iterator), _end);
        }
    }

    constexpr filter_iterator() = default;

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 reference dereference() const {
        return *_iterator;
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 pointer arrow() const {
        return fake_ptr_proxy<decltype(**this)>(**this);
    }

    LZ_CONSTEXPR_CXX_20 void increment() {
        ++_iterator;
        _iterator = find(std::move(_iterator), _end);
    }

    LZ_CONSTEXPR_CXX_20 void decrement() {
        do {
            --_iterator;
        } while (!_predicate(*_iterator) && _iterator != _begin);
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 bool eq(const filter_iterator& b) const noexcept {
        return _iterator == b._iterator;
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 bool eq(default_sentinel) const noexcept {
        return _iterator == _end;
    }
};
} // namespace detail
} // namespace lz

#endif