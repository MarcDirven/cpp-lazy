#pragma once

#ifndef LZ_ENUMERATE_ITERATOR_HPP
#define LZ_ENUMERATE_ITERATOR_HPP

#include "Lz/detail/fake_ptr_proxy.hpp"
#include "Lz/detail/traits.hpp"
#include "Lz/iterator_base.hpp"

namespace lz {
namespace detail {

template<class Iterator>
using enumerate_tag = iter_cat_decay<iter_cat_t<Iterator>, std::random_access_iterator_tag, std::forward_iterator_tag>;

template<class Iterator, class S, class Arithmetic>
class enumerate_iterator final
    : public iter_base<enumerate_iterator<Iterator, S, Arithmetic>, std::pair<Arithmetic, ref_t<Iterator>>,
                       fake_ptr_proxy<std::pair<Arithmetic, ref_t<Iterator>>>, diff_type<Iterator>, enumerate_tag<Iterator>,
                       sentinel_selector<enumerate_tag<Iterator>, enumerate_iterator<Iterator, S, Arithmetic>, S>> {

    Iterator _iterator{};
    Arithmetic _index{};

    using Traits = std::iterator_traits<Iterator>;

public:
    using value_type = std::pair<Arithmetic, typename Traits::value_type>;
    using reference = std::pair<Arithmetic, typename Traits::reference>;
    using pointer = fake_ptr_proxy<reference>;
    using difference_type = typename Traits::difference_type;

    constexpr enumerate_iterator(const Arithmetic start, Iterator iterator) : _iterator(std::move(iterator)), _index(start) {
    }

    constexpr enumerate_iterator() = default;

    enumerate_iterator& increment() {
        ++_index;
        ++_iterator;
        return *this;
    }

    enumerate_iterator& decrement() {
        --_index;
        --_iterator;
        return *this;
    }

    reference dereference() const {
        return { _index, *_iterator };
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 pointer arrow() const {
        return fake_ptr_proxy<decltype(**this)>(**this);
    }

    LZ_CONSTEXPR_CXX_20 bool eq(const enumerate_iterator& other) const {
        return _iterator == other._iterator;
    }

    LZ_CONSTEXPR_CXX_20 void plus_is(const difference_type n) {
        _index += static_cast<Arithmetic>(n);
        _iterator += n;
    }

    LZ_CONSTEXPR_CXX_20 difference_type difference(const enumerate_iterator& other) const {
        return _iterator - other._iterator;
    }

    LZ_CONSTEXPR_CXX_20 bool eq(const S& s) const {
        return _iterator == s;
    }
};
} // namespace detail
} // namespace lz

#endif
