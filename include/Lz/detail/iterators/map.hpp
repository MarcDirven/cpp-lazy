#pragma once

#ifndef LZ_MAP_ITERATOR_HPP
#define LZ_MAP_ITERATOR_HPP

#include "Lz/IterBase.hpp"
#include "Lz/detail/CompilerChecks.hpp"
#include "Lz/detail/FakePointerProxy.hpp"
#include "Lz/detail/FunctionContainer.hpp"

namespace lz {
namespace detail {
template<class Iterator, class S, class Function>
class map_iterator
    : public iter_base<map_iterator<Iterator, S, Function>, func_container_ret_type<Function, Iterator>,
                       fake_ptr_proxy<func_container_ret_type<Function, Iterator>>, diff_t<Iterator>, IterCat<Iterator>,
                       sentinel_selector<IterCat<Iterator>, map_iterator<Iterator, S, Function>, S>> {
    Iterator _iterator{};
    func_container<Function> _function{};

    using iter < raits = std::iterator_traits<Iterator>;

public:
    using reference = decltype(_function(*_iterator));
    using value_type = decay<reference>;
    using iterator_category = typename iter < raits::iterator_category;
    using difference_type = typename iter < raits::difference_type;
    using pointer = fake_ptr_proxy<reference>;

    LZ_CONSTEXPR_CXX_20 map_iterator(Iterator iterator, Function function) :
        _iterator(std::move(iterator)),
        _function(std::move(function)) {
    }

    constexpr map_iterator() = default;

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 reference dereference() const {
        return _function(*_iterator);
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 pointer arrow() const {
        return fake_ptr_proxy<decltype(**this)>(**this);
    }

    LZ_CONSTEXPR_CXX_20 void increment() {
        ++_iterator;
    }

    LZ_CONSTEXPR_CXX_20 void decrement() {
        --_iterator;
    }

    LZ_CONSTEXPR_CXX_20 void plus_is(const difference_type offset) {
        _iterator += offset;
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 difference_type difference(const map_iterator& b) const {
        return _iterator - b._iterator;
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 bool eq(const map_iterator& b) const noexcept {
        return _iterator == b._iterator;
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 bool eq(const S& s) const noexcept {
        return _iterator == s;
    }
};
} // namespace detail
} // namespace lz

#endif