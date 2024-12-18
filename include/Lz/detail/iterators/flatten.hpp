#pragma once

#ifndef LZ_FLATTEN_ITERATOR_HPP
#define LZ_FLATTEN_ITERATOR_HPP

#include "Lz/detail/fake_ptr_proxy.hpp"
#include "Lz/iterator_base.hpp"

namespace lz {
namespace detail {
template<class, class U>
struct alias_wrapper_helper {
    using type = U;
};

template<class T, class U = void>
using alias_wrapper = typename alias_wrapper_helper<T, U>::type;

template<class, class = void>
struct has_value_type : std::false_type {};

template<class T>
struct has_value_type<T, alias_wrapper<typename T::value_type>> : std::true_type {};

template<class, class = void>
struct has_diff_type : std::false_type {};

template<class T>
struct has_diff_type<T, alias_wrapper<typename T::difference_type>> : std::true_type {};

template<class, class = void>
struct has_ptr : std::false_type {};

template<class T>
struct has_ptr<T, alias_wrapper<typename T::pointer>> : std::true_type {};

template<class, class = void>
struct has_iter_cat : std::false_type {};

template<class T>
struct has_iter_cat<T, alias_wrapper<typename T::iterator_category>> : std::true_type {};

template<class, class = void>
struct has_ref : std::false_type {};

template<class T>
struct has_ref<T, alias_wrapper<typename T::reference>> : std::true_type {};

template<class T>
struct is_iterator {
    static constexpr bool value =
        has_value_type<T>::value && has_diff_type<T>::value && has_ptr<T>::value && has_ref<T>::value && has_iter_cat<T>::value;
};

template<class T, class = void>
struct traits_or_underlying_type_helper {
    using type = T;
};

template<class T>
struct traits_or_underlying_type_helper<T, alias_wrapper<typename T::iterator>> {
    using type = std::iterator_traits<typename T::iterator>;
};

template<class T, class U = void>
using traits_or_underlying_type = typename traits_or_underlying_type_helper<T, U>::type;

template<bool B>
struct count_dims_helper;

template<>
struct count_dims_helper<true> {
    template<class T>
    using inner = traits_or_underlying_type<typename T::value_type>;

    template<class T>
    using type =
        std::integral_constant<std::size_t, 1 + count_dims_helper<is_iterator<inner<T>>::value>::template type<inner<T>>::value>;
};

template<>
struct count_dims_helper<false> {
    template<class>
    using type = std::integral_constant<std::size_t, 0>;
};

template<class T>
using count_dims = typename count_dims_helper<is_iterator<T>::value>::template type<T>;

// Improvement of https://stackoverflow.com/a/21076724/8729023
template<class Iterator, class S>
class flatten_wrapper
    : public iter_base<
          flatten_wrapper<Iterator, S>, ref_t<Iterator>, fake_ptr_proxy<ref_t<Iterator>>, diff_type<Iterator>,
          common_type<std::bidirectional_iterator_tag, iter_cat_t<Iterator>>,
          sentinel_selector<common_type<std::bidirectional_iterator_tag, iter_cat_t<Iterator>>, flatten_wrapper<Iterator, S>>> {

    Iterator _begin{};
    Iterator _current{};
    S _end{};

    using traits = std::iterator_traits<Iterator>;

public:
    using reference = typename traits::reference;
    using pointer = fake_ptr_proxy<reference>;
    using value_type = typename traits::value_type;
    using difference_type = typename traits::difference_type;

    constexpr flatten_wrapper() = default;

    LZ_CONSTEXPR_CXX_20 flatten_wrapper(Iterator current, Iterator begin, S end) :
        _begin(std::move(begin)),
        _current(std::move(current)),
        _end(std::move(end)) {
    }

    LZ_CONSTEXPR_CXX_20 bool has_some() const noexcept {
        return _current != _end;
    }

    LZ_CONSTEXPR_CXX_20 bool has_prev() const noexcept {
        return _current != _begin;
    }

    LZ_CONSTEXPR_CXX_20 bool eq(const flatten_wrapper& b) const noexcept {
        return _current == b._current;
    }

    LZ_CONSTEXPR_CXX_20 bool eq(default_sentinel) const noexcept {
        return _current == _end;
    }

    LZ_CONSTEXPR_CXX_20 reference dereference() const {
        return *_current;
    }

    LZ_CONSTEXPR_CXX_20 pointer arrow() const {
        return fake_ptr_proxy<decltype(**this)>(**this);
    }

    LZ_CONSTEXPR_CXX_20 void increment() {
        ++_current;
    }

    LZ_CONSTEXPR_CXX_20 void decrement() {
        --_current;
    }
};

template<class, class, int>
class flatten_iterator;

template<class Iterator, int N>
using inner =
    flatten_iterator<decltype(std::begin(*std::declval<Iterator>())), decltype(std::end(*std::declval<Iterator>())), N - 1>;

template<class Iterator, class S, int N>
class flatten_iterator : public iter_base<flatten_iterator<Iterator, S, N>, ref_t<inner<Iterator, N>>,
                                          fake_ptr_proxy<ref_t<inner<Iterator, N>>>, diff_type<inner<Iterator, N>>,
                                          common_type<std::bidirectional_iterator_tag, iter_cat_t<inner<Iterator, N>>>,
                                          sentinel_selector<iter_cat_t<inner<Iterator, N>>, flatten_iterator<Iterator, S, N>>> {

    using this_inner = inner<Iterator, N>;

public:
    using reference = typename this_inner::reference;
    using pointer = typename this_inner::pointer;
    using value_type = typename this_inner::value_type;
    using difference_type = typename this_inner::difference_type;

private:
    LZ_CONSTEXPR_CXX_20 void advance() {
        if (_inner_iter.has_some()) {
            return;
        }
        for (++_outer_iter; _outer_iter.has_some(); ++_outer_iter) {
            const auto begin = std::begin(*_outer_iter);
            _inner_iter = this_inner(begin, begin, std::end(*_outer_iter));
            if (_inner_iter.has_some()) {
                return;
            }
        }
        _inner_iter = {};
    }

    flatten_wrapper<Iterator, S> _outer_iter;
    this_inner _inner_iter;

public:
    constexpr flatten_iterator() = default;

    LZ_CONSTEXPR_CXX_20 flatten_iterator(Iterator it, Iterator begin, S end) :
        _outer_iter(std::move(it), std::move(begin), std::move(end)) {
        if (_outer_iter.has_some()) {
            const auto beg = std::begin(*_outer_iter);
            _inner_iter = this_inner(beg, beg, std::end(*_outer_iter));
            this->advance();
        }
    }

    LZ_CONSTEXPR_CXX_20 bool has_some() const {
        return _outer_iter.has_some();
    }

    LZ_CONSTEXPR_CXX_20 bool has_prev() const {
        return _inner_iter.has_prev() || _outer_iter.has_prev();
    }

    LZ_CONSTEXPR_CXX_20 bool eq(const flatten_iterator& b) const noexcept {
        return _outer_iter == b._outer_iter && _inner_iter == b._inner_iter;
    }

    LZ_CONSTEXPR_CXX_20 bool eq(default_sentinel) const noexcept {
        return !has_some();
    }

    LZ_CONSTEXPR_CXX_20 reference dereference() const {
        return *_inner_iter;
    }

    LZ_CONSTEXPR_CXX_20 pointer arrow() const {
        return fake_ptr_proxy<decltype(**this)>(**this);
    }

    LZ_CONSTEXPR_CXX_20 void increment() {
        ++_inner_iter;
        this->advance();
    }

    LZ_CONSTEXPR_CXX_20 void decrement() {
        if (_inner_iter.has_prev()) {
            --_inner_iter;
            return;
        }
        while (_outer_iter.has_prev()) {
            --_outer_iter;
            const auto end = std::end(*_outer_iter);
            _inner_iter = this_inner(end, std::begin(*_outer_iter), end);
            if (_inner_iter.has_prev()) {
                --_inner_iter;
                return;
            }
        }
    }
};

template<class Iterator, class S>
class flatten_iterator<Iterator, S, 0>
    : public iter_base<flatten_iterator<Iterator, S, 0>, ref_t<flatten_wrapper<Iterator, S>>,
                       fake_ptr_proxy<ref_t<flatten_wrapper<Iterator, S>>>, diff_type<flatten_wrapper<Iterator, S>>,
                       common_type<std::bidirectional_iterator_tag, iter_cat_t<flatten_wrapper<Iterator, S>>>,
                       sentinel_selector<common_type<iter_cat_t<flatten_wrapper<Iterator, S>>, std::bidirectional_iterator_tag>,
                                         flatten_iterator<Iterator, S, 0>>> {

    flatten_wrapper<Iterator, S> _range;
    using traits = std::iterator_traits<Iterator>;

public:
    using pointer = typename traits::pointer;
    using reference = typename traits::reference;
    using value_type = typename traits::value_type;
    using difference_type = typename traits::difference_type;

    constexpr flatten_iterator() = default;

    constexpr flatten_iterator(Iterator it, Iterator begin, S end) : _range(std::move(it), std::move(begin), std::move(end)) {
    }

    LZ_CONSTEXPR_CXX_20 bool has_prev() const noexcept {
        return _range.has_prev();
    }

    LZ_CONSTEXPR_CXX_20 bool has_some() const noexcept {
        return _range.has_some();
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 reference dereference() const {
        return *_range;
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 pointer arrow() const {
        return fake_ptr_proxy<decltype(**this)>(**this);
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 bool eq(const flatten_iterator& b) const noexcept {
        return _range == b._range;
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 bool eq(default_sentinel) const noexcept {
        return _range.eq(default_sentinel{});
    }

    LZ_CONSTEXPR_CXX_20 void increment() {
        ++_range;
    }

    LZ_CONSTEXPR_CXX_20 void decrement() {
        --_range;
    }
};
} // namespace detail
} // namespace lz

#endif // LZ_FLATTEN_ITERATOR_HPP
