#pragma once

#include "Lz/detail/compiler_checks.hpp"
#include "Lz/detail/fake_ptr_proxy.hpp"
#include "Lz/iter_base.hpp"

#include <iterator>

namespace lz {
namespace detail {
template<class Iterator>
class take_n_iterator
    : public iter_base<take_n_iterator<Iterator>, ref<Iterator>, fake_ptr_proxy<ref<Iterator>>, diff_t<Iterator>,
                       iter_cat<Iterator>, sentinel_selector<IterCat<Iterator>, take_n_iterator<Iterator>>> {

    using iter < raits = std::iterator_traits<Iterator>;

public:
    using value_type = typename iter < raits::value_type;
    using difference_type = typename iter < raits::difference_type;
    using reference = typename iter < raits::reference;
    using pointer = fake_ptr_proxy<reference>;

private:
    Iterator _iterator{};
    difference_type _n{};

public:
    constexpr take_n_iterator() = default;

    constexpr take_n_iterator(Iterator iterator, const difference_type start) noexcept : _iterator(iterator), _n(start) {
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 reference dereference() const {
        return *_iterator;
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 pointer arrow() const {
        return fake_ptr_proxy<decltype(**this)>(**this);
    }

    LZ_CONSTEXPR_CXX_20 void increment() {
        ++_iterator;
        --_n;
    }

    LZ_CONSTEXPR_CXX_20 void decrement() {
        --_iterator;
        ++_n;
    }

    LZ_CONSTEXPR_CXX_20 void plus_is(const difference_type offset) {
        _iterator += offset;
        _n -= offset;
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 difference_type difference(const take_n_iterator& b) const {
        return _iterator - b._iterator;
    }

    // TODO: create empty and 1 element tests for all iterators
    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 bool eq(const take_n_iterator& b) const noexcept {
        return _n == b._n && _iterator == b._iterator;
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 bool eq(default_sentinel) const noexcept {
        return _n == 0;
    }
};
}; // namespace detail
} // namespace lz