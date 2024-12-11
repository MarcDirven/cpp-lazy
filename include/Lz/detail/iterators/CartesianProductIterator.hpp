#pragma once

#ifndef LZ_CARTESIAN_PRODUCT_ITERATOR_HPP
#define LZ_CARTESIAN_PRODUCT_ITERATOR_HPP

#include "Lz/IterBase.hpp"
#include "Lz/detail/FakePointerProxy.hpp"
#include "Lz/detail/Traits.hpp"

#include <numeric>

namespace lz {
namespace detail {
struct Vt {}; // ValueType
struct Rt {}; // ReferenceType
struct Ic {}; // IterCat
struct Dt {}; // DiffType

template<class IterTuple, class Tag>
struct IterTupleType;

template<class... Iterators>
struct IterTupleType<std::tuple<Iterators...>, Vt> {
    using type = std::tuple<ValueType<Iterators>...>;
};

template<class... Iterators>
struct IterTupleType<std::tuple<Iterators...>, Rt> {
    using type = std::tuple<RefType<Iterators>...>;
};

template<class... Iterators>
struct IterTupleType<std::tuple<Iterators...>, Ic> {
    using type = CommonType<IterCat<Iterators>...>;
};

template<class... Iterators>
struct IterTupleType<std::tuple<Iterators...>, Dt> {
    using type = CommonType<DiffType<Iterators>...>;
};

template<class IterTuple>
using ValueTypeIterTupleT = typename IterTupleType<IterTuple, Vt>::type;

template<class IterTuple>
using RefTypeIterTupleT = typename IterTupleType<IterTuple, Rt>::type;

template<class IterTuple>
using IterCatIterTupleT = typename IterTupleType<IterTuple, Ic>::type;

template<class IterTuple>
using DiffTypeIterTupleT = typename IterTupleType<IterTuple, Dt>::type;

template<class IterTuple, class SentinelTuple>
class CartesianProductIterator;

template<class IterTuple, class SentinelTuple>
using DefaultSentinelSelector =
    SentinelSelector<IterCatIterTupleT<IterTuple>, CartesianProductIterator<IterTuple, SentinelTuple>>;

// Edited version of https://github.com/mirandaconrado/product-iterator
template<class IterTuple, class SentinelTuple>
class CartesianProductIterator
    : public IterBase<CartesianProductIterator<IterTuple, SentinelTuple>, RefTypeIterTupleT<IterTuple>,
                      FakePointerProxy<RefTypeIterTupleT<IterTuple>>, DiffTypeIterTupleT<IterTuple>, IterCatIterTupleT<IterTuple>,
                      DefaultSentinelSelector<IterTuple, SentinelTuple>> {

    static constexpr std::size_t Size = std::tuple_size<IterTuple>::value;
    static_assert(Size > 1, "The size of the iterators must be greater than 1");

    using TupleElement1 = TupleElement<0, IterTuple>;

public:
    using value_type = ValueTypeIterTupleT<IterTuple>;
    using reference = RefTypeIterTupleT<IterTuple>;
    using pointer = FakePointerProxy<reference>;
    using difference_type = DiffTypeIterTupleT<IterTuple>;

private:
    IterTuple _begin{};
    IterTuple _iterator{};
    SentinelTuple _end{};

#ifndef __cpp_if_constexpr
    template<std::size_t I>
    LZ_CONSTEXPR_CXX_14 EnableIf<I == 0, void> next() const noexcept {
    }

    template<std::size_t I>
    LZ_CONSTEXPR_CXX_14 EnableIf<I == 0, void> previous() const noexcept {
    }

#ifdef LZ_MSVC
#pragma warning(push)
#pragma warning(disable : 4127)
#endif // LZ_MSVC

    template<std::size_t I>
    LZ_CONSTEXPR_CXX_20 EnableIf<(I > 0), void> next() {
        auto& prev = std::get<I - 1>(_iterator);
        ++prev;

        auto& first = std::get<0>(_iterator);
        auto peekFirst = std::next(first);
        if (std::get<Size - 1>(_iterator) == std::get<Size - 1>(_end) && peekFirst == std::get<0>(_end)) {
            first = std::move(peekFirst);
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
    LZ_CONSTEXPR_CXX_20 void doPrev() {
        --std::get<I>(_iterator);
    }

    template<std::size_t I>
    LZ_CONSTEXPR_CXX_20 EnableIf<I == 0> doPrevAll() {
        doPrev<0>();
    }

    template<std::size_t I>
    LZ_CONSTEXPR_CXX_20 EnableIf<(I > 0)> doPrevAll() {
        doPrev<I>();
        doPrevAll<I - 1>();
    }

    template<std::size_t I>
    LZ_CONSTEXPR_CXX_20 EnableIf<(I > 0)> previous() {
        if (_iterator == _end) {
            doPrevAll<I - 1>();
        }
        else {
            auto& prev = std::get<I - 1>(_iterator);
            if (prev == std::get<I - 1>(_begin)) {
                if (I != 1) {
                    prev = std::get<I - 1>(_end);
                    doPrev<I - 1>();
                    previous<I - 1>();
                }
            }
            else {
                doPrev<I - 1>();
            }
        }
    }

#ifdef LZ_MSVC
#pragma warning(pop)
#endif // LZ_MSVC

    template<std::size_t I>
    LZ_CONSTEXPR_CXX_20 EnableIf<I == 0> operatorPlusImpl(const difference_type offset) {
        auto& iterator = std::get<0>(_iterator);
        iterator = std::next(std::move(iterator), offset);
    }

    template<std::size_t I>
    EnableIf<(I > 0)> operatorPlusImpl(const difference_type offset) {
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
        operatorPlusImpl<I - 1>(static_cast<difference_type>(offsets.quot));
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
            auto peekFirst = std::next(first);
            if (std::get<Size - 1>(_iterator) == std::get<Size - 1>(_end) && peekFirst == std::get<0>(_end)) {
                first = std::move(peekFirst);
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
    void operatorPlusImpl(const difference_type offset) {
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
            operatorPlusImpl<I - 1>(static_cast<difference_type>(quot));
        }
    }

    template<std::size_t I>
    LZ_CONSTEXPR_CXX_20 void doPrev() {
        --std::get<I>(_iterator);
    }

    template<std::size_t I>
    LZ_CONSTEXPR_CXX_20 void doPrevAll() {
        doPrev<I>();
        if constexpr (I > 0) {
            doPrevAll<I - 1>();
        }
    }

    template<std::size_t I>
    LZ_CONSTEXPR_CXX_20 void previous() {
        if constexpr (I == 0) {
            return;
        }
        if (_iterator == _end) {
            doPrevAll<I - 1>();
        }
        else {
            auto& prev = std::get<I - 1>(_iterator);
            if (prev == std::get<I - 1>(_begin)) {
                if constexpr (I != 1) {
                    prev = std::get<I - 1>(_end);
                    doPrev<I - 1>();
                    previous<I - 1>();
                }
            }
            else {
                doPrev<I - 1>();
            }
        }
    }

#endif // __cpp_if_constexpr

    template<std::size_t... Is>
    LZ_CONSTEXPR_CXX_20 reference dereference(IndexSequence<Is...>) const {
        return reference{ *std::get<Is>(_iterator)... };
    }

    template<std::size_t... Is>
    LZ_CONSTEXPR_CXX_20 difference_type distanceImpl(IndexSequence<Is...>, const CartesianProductIterator& c) const {
        const difference_type distances[] = { static_cast<difference_type>(std::get<Is>(c._iterator) -
                                                                           std::get<Is>(_iterator))... };
        return std::accumulate(std::begin(distances), std::end(distances), difference_type{ 1 },
                               std::multiplies<difference_type>{});
    }

    using IndexSequenceForThis = MakeIndexSequence<Size>;

    void checkEnd() {
        if (std::get<0>(_iterator) == std::get<0>(_end)) {
            _iterator = _end;
        }
    }

public:
    constexpr CartesianProductIterator() = default;

    LZ_CONSTEXPR_CXX_20
    CartesianProductIterator(IterTuple iterator, IterTuple begin, SentinelTuple end) :
        _begin(std::move(begin)),
        _iterator(std::move(iterator)),
        _end(std::move(end)) {
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 reference dereference() const {
        return dereference(IndexSequenceForThis());
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 pointer arrow() const {
        return FakePointerProxy<decltype(**this)>(**this);
    }

    void increment() {
        next<Size>();
    }

    void decrement() {
        previous<Size>();
    }

    void plusIs(const difference_type n) {
        operatorPlusImpl<Size - 1>(n);
        checkEnd();
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 bool eq(const CartesianProductIterator& other) const {
        return _iterator == other._iterator;
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 bool eq(DefaultSentinel) const {
        return _iterator == _end;
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 difference_type difference(const CartesianProductIterator& other) const {
        return other.distanceImpl(IndexSequenceForThis(), *this);
    }
};
} // namespace detail
} // namespace lz

#endif // LZ_CARTESIAN_PRODUCT_ITERATOR_HPP
