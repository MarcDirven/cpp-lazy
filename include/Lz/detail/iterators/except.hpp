#pragma once

#ifndef LZ_EXCEPT_ITERATOR_HPP
#define LZ_EXCEPT_ITERATOR_HPP

#include "Lz/detail/fake_ptr_proxy.hpp"
#include "Lz/detail/func_container.hpp"
#include "Lz/iter_base.hpp"

#include <algorithm>

namespace lz {
namespace detail {
template<class Iterator, class S, class IteratorToExcept, class S2, class BinaryPredicate>
class except_iterator
    : public iter_base<except_iterator<Iterator, S, IteratorToExcept, S2, BinaryPredicate>, ref_t<Iterator>,
                       fake_ptr_proxy<ref_t<Iterator>>, diff_type<Iterator>, std::forward_iterator_tag, default_sentinel> {

    using iter_traits = std::iterator_traits<Iterator>;

public:
    using value_type = typename iter_traits::value_type;
    using difference_type = typename iter_traits::difference_type;
    using reference = typename iter_traits::reference;
    using pointer = fake_ptr_proxy<reference>;

private:
    Iterator _iterator{};
    IteratorToExcept _to_except_begin{};
    S2 _to_except_end{};
    S _end{};
    mutable func_container<BinaryPredicate> _predicate{};

    template<class T = Iterator>
    LZ_CONSTEXPR_CXX_20 enable_if<std::is_same<T, S>::value && std::is_same<IteratorToExcept, S2>::value> find_next() {
        _iterator = std::find_if(std::move(_iterator), _end, [this](const value_type& value) {
            return !std::binary_search(_to_except_begin, _to_except_end, value, _predicate);
        });
    }

    template<class T = Iterator>
    LZ_CONSTEXPR_CXX_20 enable_if<!std::is_same<T, S>::value && std::is_same<IteratorToExcept, S2>::value> find_next() {
        _iterator = detail::find_if(std::move(_iterator), _end, [this](const value_type& value) {
            return !std::binary_search(_to_except_begin, _to_except_end, value, _predicate);
        });
    }

    template<class T = Iterator>
    LZ_CONSTEXPR_CXX_20 enable_if<std::is_same<T, S>::value && !std::is_same<IteratorToExcept, S2>::value> find_next() {
        _iterator = std::find_if(std::move(_iterator), _end, [this](const value_type& value) {
            return !detail::binary_search(_to_except_begin, _to_except_end, value, _predicate);
        });
    }

    template<class T = Iterator>
    LZ_CONSTEXPR_CXX_20 enable_if<!std::is_same<T, S>::value && !std::is_same<IteratorToExcept, S2>::value> find_next() {
        _iterator = detail::find_if(std::move(_iterator), _end, [this](const value_type& value) {
            return !detail::binary_search(_to_except_begin, _to_except_end, value, _predicate);
        });
    }

public:
    constexpr except_iterator() = default;

    except_iterator(Iterator begin, S end, IteratorToExcept except_begin, S2 except_end, BinaryPredicate compare) :
        _iterator(std::move(begin)),
        _to_except_begin(std::move(except_begin)),
        _to_except_end(std::move(except_end)),
        _end(std::move(end)),
        _predicate(std::move(compare)) {
        if (_to_except_begin == _to_except_end) {
            return;
        }
        find_next();
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 reference dereference() const {
        return *_iterator;
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 pointer arrow() const {
        return fake_ptr_proxy<decltype(**this)>(**this);
    }

    LZ_CONSTEXPR_CXX_20 void increment() {
        ++_iterator;
        find_next();
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 bool eq(const except_iterator& b) const {
        return _iterator == b._iterator;
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 bool eq(default_sentinel) const {
        return _iterator == _end;
    }
};
} // namespace detail
} // namespace lz

#endif