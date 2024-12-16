#pragma once

#ifndef LZ_CHUNK_IF_ITERATOR_HPP
#define LZ_CHUNK_IF_ITERATOR_HPP

#include "Lz/detail/algorithm.hpp"
#include "Lz/detail/basic_iterable.hpp"
#include "Lz/detail/fake_ptr_proxy.hpp"
#include "Lz/detail/func_container.hpp"
#include "Lz/iter_base.hpp"

namespace lz {
namespace detail {

template<class Iterator, class S, class UnaryPredicate>
class chunk_if_iterator : public iter_base<chunk_if_iterator<Iterator, S, UnaryPredicate>, basic_iterable<Iterator>,
                                           fake_ptr_proxy<basic_iterable<Iterator>>, diff_type<Iterator>,
                                           std::forward_iterator_tag, default_sentinel> {
    using iter < raits = std::iterator_traits<Iterator>;

public:
    using value_type = basic_iterable<Iterator>;
    using difference_type = typename iter < raits::difference_type;
    using reference = value_type;
    using pointer = fake_ptr_proxy<reference>;

private:
    Iterator _sub_range_begin{};
    Iterator _sub_range_end{};
    bool _trailing_empty{ true };
    S _end{};
    mutable func_container<UnaryPredicate> _predicate{};

    // Overload for when the iterator is a sentinel
    template<class T = Iterator>
    enable_if<!std::is_same<T, S>::value> find_next() {
        _sub_range_end = detail::find_if(_sub_range_end, _end, _predicate);
    }

    // Overload for when the iterator is not a sentinel
    template<class T = Iterator>
    enable_if<std::is_same<T, S>::value> find_next() {
        _sub_range_end = std::find_if(_sub_range_end, _end, _predicate);
    }

public:
    constexpr chunk_if_iterator() = default;

    chunk_if_iterator(Iterator iterator, S end, UnaryPredicate predicate) :
        _sub_range_begin(iterator),
        _sub_range_end(std::move(iterator)),
        _end(std::move(end)),
        _predicate(std::move(predicate)) {
        if (_sub_range_begin != _end) {
            find_next();
        }
        else {
            _trailing_empty = false;
        }
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 reference dereference() const {
        return { _sub_range_begin, _sub_range_end };
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 pointer arrow() const {
        return fake_ptr_proxy<decltype(**this)>(**this);
    }

    LZ_CONSTEXPR_CXX_20 void increment() {
        if (_trailing_empty && _sub_range_end == _end) {
            _sub_range_begin = _sub_range_end;
            _trailing_empty = false;
            return;
        }

        auto prev = _sub_range_end++;
        if (_sub_range_end != _end) {
            _sub_range_begin = _sub_range_end;
            find_next();
            return;
        }

        if (_predicate(*prev)) {
            _sub_range_begin = _sub_range_end = _trailing_empty ? std::move(prev) : _end;
            _trailing_empty = false;
            return;
        }
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 bool eq(const chunk_if_iterator& rhs) const {
        return _sub_range_begin == rhs._sub_range_begin && _sub_range_end == rhs._sub_range_end &&
               _trailing_empty == rhs._trailing_empty;
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 bool eq(default_sentinel) const {
        return _sub_range_begin == _end && !_trailing_empty;
    }
};
} // namespace detail
} // namespace lz

#endif // LZ_CHUNK_IF_ITERATOR_HPP
