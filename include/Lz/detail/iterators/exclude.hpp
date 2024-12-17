#pragma once

#ifndef LZ_EXCLUDE_ITERATOR_HPP
#define LZ_EXCLUDE_ITERATOR_HPP

#include "Lz/detail/fake_ptr_proxy.hpp"
#include "Lz/detail/traits.hpp"
#include "Lz/iter_base.hpp"

namespace lz {
namespace detail {
// TODO can we make bidirectional?
template<class Iterator, class S>
class exclude_iterator : public iter_base<exclude_iterator<Iterator, S>, ref_t<Iterator>, fake_ptr_proxy<ref_t<Iterator>>,
                                          diff_type<Iterator>, std::forward_iterator_tag, S> {

    using traits = std::iterator_traits<Iterator>;

public:
    using value_type = typename traits::value_type;
    using difference_type = typename traits::difference_type;
    using reference = typename traits::reference;
    using pointer = fake_ptr_proxy<reference>;

private:
    Iterator _iterator{};
    difference_type _index{};
    difference_type _from{};
    difference_type _to{};

public:
    LZ_CONSTEXPR_CXX_20
    exclude_iterator(Iterator begin, S end, const difference_type from, const difference_type to) :
        _iterator(std::move(begin)),
        _from(from),
        _to(to) {
        if (_iterator != end && _from == 0) {
            _iterator = std::next(std::move(_iterator), _to);
            _index = _to;
        }
    }

    constexpr exclude_iterator() = default;

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 reference dereference() const {
        return *_iterator;
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 pointer arrow() const {
        return fake_ptr_proxy<decltype(**this)>(**this);
    }

    LZ_CONSTEXPR_CXX_20 void increment() {
        ++_iterator;
        ++_index;
        if (_index == _from) {
            _iterator = std::next(std::move(_iterator), _to - _from);
        }
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 bool eq(const exclude_iterator& b) const noexcept {
        LZ_ASSERT(_to == b._to && _from == b._from, "incompatible iterator types: from and to must be equal");
        return _iterator == b._iterator;
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 bool eq(const S& s) const noexcept {
        return _iterator == s;
    }
};
} // namespace detail
} // namespace lz

#endif // LZ_EXCLUDE_ITERATOR_HPP
