#pragma once

#ifndef LZ_CARTESIAN_PRODUCT_ITERATOR_HPP
#define LZ_CARTESIAN_PRODUCT_ITERATOR_HPP

#include "Lz/detail/fake_ptr_proxy.hpp"
#include "Lz/detail/traits.hpp"
#include "Lz/iter_base.hpp"

#include <numeric>

namespace lz {
namespace detail {
struct vt {}; // value_type
struct rt {}; // reference type
struct ic {}; // iter category
struct dt {}; // diff type

template<class IterTuple, class Tag>
    struct iter < uple_type;

template<class... Iterators>
    struct iter < uple_type<std::tuple<Iterators...>, vt> {
    using type = std::tuple<value_type<Iterators>...>;
};

template<class... Iterators>
    struct iter < uple_type<std::tuple<Iterators...>, rt> {
    using type = std::tuple<ref<Iterators>...>;
};

template<class... Iterators>
    struct iter < uple_type<std::tuple<Iterators...>, ic> {
    using type = common_type<iter_cat<Iterators>...>;
};

template<class... Iterators>
    struct iter < uple_type<std::tuple<Iterators...>, dt> {
    using type = common_type<diff_type<Iterators>...>;
};

template<class IterTuple>
using value_type_t = typename iter < uple_type<IterTuple, vt>::type;

template<class IterTuple>
    using ref_type_iter < uple = typename iter < uple_type<IterTuple, rt>::type;

template<class IterTuple>
    using iter_cat_iter < uple = typename iter < uple_type<IterTuple, ic>::type;

template<class IterTuple>
    using diff_type_iter < uple = typename iter < uple_type<IterTuple, dt>::type;

template<class IterTuple, class SentinelTuple>
class cartesian_product_iterator;

template<class IterTuple, class SentinelTuple>
using default_sentinel_selector =
    sentinel_selector < iter_cat_iter<uple<IterTuple>, cartesian_product_iterator<IterTuple, SentinelTuple>>;

// Edited version of https://github.com/mirandaconrado/product-iterator
template<class IterTuple, class SentinelTuple>
    class cartesian_product_iterator : public iter_base < cartesian_product_iterator<IterTuple, SentinelTuple>,
    ref_type_iter < uple<IterTuple>, fake_ptr_proxy < ref_type_iter<uple<IterTuple>>, diff_type_iter < uple<IterTuple>,
    iter_cat_iter<uple<IterTuple>, default_sentinel_selector<IterTuple, SentinelTuple>> {

    static constexpr std::size_t tup_size = std::tuple_size<IterTuple>::value;
    static_assert(tup_size > 1, "The size of the iterators must be greater than 1");

public:
    using value_type = value_type_tIterTupleT<IterTuple>;
    using reference = ref_type_iter < uple<IterTuple>;
    using pointer = fake_ptr_proxy<reference>;
    using difference_type = diff_type_iter < uple<IterTuple>;

private:
    IterTuple _begin{};
    IterTuple _iterator{};
    SentinelTuple _end{};

#ifndef __cpp_if_constexpr
    template<std::size_t I>
    LZ_CONSTEXPR_CXX_14 enable_if<I == 0, void> next() const noexcept {
    }

    template<std::size_t I>
    LZ_CONSTEXPR_CXX_14 enable_if<I == 0, void> previous() const noexcept {
    }

#ifdef LZ_MSVC
#pragma warning(push)
#pragma warning(disable : 4127)
#endif // LZ_MSVC

    template<std::size_t I>
    LZ_CONSTEXPR_CXX_20 enable_if<(I > 0), void> next() {
        auto& prev = std::get<I - 1>(_iterator);
        ++prev;

        auto& first = std::get<0>(_iterator);
        auto peek_first = std::next(first);
        if (std::get<tup_size - 1>(_iterator) == std::get<tup_size - 1>(_end) && peek_first == std::get<0>(_end)) {
            first = std::move(peek_first);
            return;
        }

        if (prev == std::get<I - 1>(_end)) {
            if (I != 1) {
                prev = std::get<I - 1>(_begin);
                next<I - 1>();
            }
        }
    }

    template<std::size_t I>
    LZ_CONSTEXPR_CXX_20 void do_prev() {
        --std::get<I>(_iterator);
    }

    template<std::size_t I>
    LZ_CONSTEXPR_CXX_20 enable_if<I == 0> do_prev_all() {
        do_prev<0>();
    }

    template<std::size_t I>
    LZ_CONSTEXPR_CXX_20 enable_if<(I > 0)> do_prev_all() {
        do_prev<I>();
        do_prev_all<I - 1>();
    }

    template<std::size_t I>
    LZ_CONSTEXPR_CXX_20 enable_if<(I > 0)> previous() {
        if (_iterator == _end) {
            do_prev_all<I - 1>();
        }
        else {
            auto& prev = std::get<I - 1>(_iterator);
            if (prev == std::get<I - 1>(_begin)) {
                if (I != 1) {
                    prev = std::get<I - 1>(_end);
                    do_prev<I - 1>();
                    previous<I - 1>();
                }
            }
            else {
                do_prev<I - 1>();
            }
        }
    }

#ifdef LZ_MSVC
#pragma warning(pop)
#endif // LZ_MSVC

    template<std::size_t I>
    LZ_CONSTEXPR_CXX_20 enable_if<I == 0> operator_plus_impl(const difference_type offset) {
        auto& iterator = std::get<0>(_iterator);
        iterator = std::next(std::move(iterator), offset);
    }

    template<std::size_t I>
    enable_if<(I > 0)> operator_plus_impl(const difference_type offset) {
        auto& iterator = std::get<I>(_iterator);
        const auto& begin = std::get<I>(_begin);
        const auto& end = std::get<I>(_end);
        difference_type dist;
        if (offset < 0) {
            if (iterator == begin) {
                iterator = end;
                dist = iterator - begin;
            }
            else {
                dist = iterator - begin + 1;
            }
        }
        else {
            dist = end - iterator;
        }
        const auto offsets = std::lldiv(offset, dist);
        iterator += static_cast<difference_type>(offsets.rem);
        operator_plus_impl<I - 1>(static_cast<difference_type>(offsets.quot));
    }

#else
    template<std::size_t I>
    LZ_CONSTEXPR_CXX_20 void next() {
        if constexpr (I == 0) {
            return;
        }
        else {
            auto& prev = std::get<I - 1>(_iterator);
            ++prev;

            auto& first = std::get<0>(_iterator);
            auto peek_first = std::next(first);
            if (std::get<tup_size - 1>(_iterator) == std::get<tup_size - 1>(_end) && peek_first == std::get<0>(_end)) {
                first = std::move(peek_first);
                return;
            }

            if (prev == std::get<I - 1>(_end)) {
                if constexpr (I != 1) {
                    prev = std::get<I - 1>(_begin);
                    next<I - 1>();
                }
            }
        }
    }

    template<std::size_t I>
    void operator_plus_impl(const difference_type offset) {
        auto& iterator = std::get<I>(_iterator);
        if constexpr (I == 0) {
            iterator = iterator + offset;
        }
        else {
            const auto& begin = std::get<I>(_begin);
            const auto& end = std::get<I>(_end);
            difference_type dist;
            if (offset < 0) {
                if (iterator == begin) {
                    iterator = end;
                    dist = iterator - begin;
                }
                else {
                    dist = iterator - begin + 1;
                }
            }
            else {
                dist = end - iterator;
            }
            const auto [quot, rem] = std::lldiv(offset, dist);
            iterator += static_cast<difference_type>(rem);
            operator_plus_impl<I - 1>(static_cast<difference_type>(quot));
        }
    }

    template<std::size_t I>
    LZ_CONSTEXPR_CXX_20 void do_prev() {
        --std::get<I>(_iterator);
    }

    template<std::size_t I>
    LZ_CONSTEXPR_CXX_20 void do_prev_all() {
        do_prev<I>();
        if constexpr (I > 0) {
            do_prev_all<I - 1>();
        }
    }

    template<std::size_t I>
    LZ_CONSTEXPR_CXX_20 void previous() {
        if constexpr (I == 0) {
            return;
        }
        if (_iterator == _end) {
            do_prev_all<I - 1>();
        }
        else {
            auto& prev = std::get<I - 1>(_iterator);
            if (prev == std::get<I - 1>(_begin)) {
                if constexpr (I != 1) {
                    prev = std::get<I - 1>(_end);
                    do_prev<I - 1>();
                    previous<I - 1>();
                }
            }
            else {
                do_prev<I - 1>();
            }
        }
    }

#endif // __cpp_if_constexpr

    template<std::size_t... Is>
    LZ_CONSTEXPR_CXX_20 reference dereference(index_sequence<Is...>) const {
        return reference{ *std::get<Is>(_iterator)... };
    }

    template<std::size_t... Is>
    LZ_CONSTEXPR_CXX_20 difference_type distance_impl(index_sequence<Is...>, const cartesian_product_iterator& c) const {
        const difference_type distances[] = { static_cast<difference_type>(std::get<Is>(c._iterator) -
                                                                           std::get<Is>(_iterator))... };
        return std::accumulate(std::begin(distances), std::end(distances), difference_type{ 1 },
                               std::multiplies<difference_type>{});
    }

    using index_sequence_for_this = make_index_sequence<tup_size>;

    void check_end() {
        if (std::get<0>(_iterator) == std::get<0>(_end)) {
            _iterator = _end;
        }
    }

public:
    constexpr cartesian_product_iterator() = default;

    LZ_CONSTEXPR_CXX_20
    cartesian_product_iterator(IterTuple iterator, IterTuple begin, SentinelTuple end) :
        _begin(std::move(begin)),
        _iterator(std::move(iterator)),
        _end(std::move(end)) {
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 reference dereference() const {
        return dereference(index_sequence_for_this());
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 pointer arrow() const {
        return fake_ptr_proxy<decltype(**this)>(**this);
    }

    void increment() {
        next<tup_size>();
    }

    void decrement() {
        previous<tup_size>();
    }

    void plus_is(const difference_type n) {
        operator_plus_impl<tup_size - 1>(n);
        check_end();
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 bool eq(const cartesian_product_iterator& other) const {
        return _iterator == other._iterator;
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 bool eq(default_sentinel) const {
        return _iterator == _end;
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 difference_type difference(const cartesian_product_iterator& other) const {
        return other.distance_impl(index_sequence_for_this(), *this);
    }
};
} // namespace detail
} // namespace lz

#endif // LZ_CARTESIAN_PRODUCT_ITERATOR_HPP
