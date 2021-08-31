#pragma once

#ifndef LZ_CARTESIAN_PRODUCT_ITERATOR_HPP
#    define LZ_CARTESIAN_PRODUCT_ITERATOR_HPP

#    include "LzTools.hpp"

#    include <numeric>

namespace lz {
namespace internal {
// Edited version of https://github.com/mirandaconrado/product-iterator
template<class... Iterators>
class CartesianProductIterator {
    static_assert(sizeof...(Iterators) > 1, "The size of the iterators must be greater than 1");

public:
    using value_type = std::tuple<ValueType<Iterators>...>;
    using reference = std::tuple<RefType<Iterators>...>;
    using pointer = FakePointerProxy<reference>;
    using iterator_category = std::random_access_iterator_tag;
    using difference_type = typename std::common_type<DiffType<Iterators>...>::type;

private:
    std::tuple<Iterators...> _begin{};
    std::tuple<Iterators...> _iterator{};
    std::tuple<Iterators...> _end{};

#    ifndef __cpp_if_constexpr
    template<std::size_t I>
    constexpr EnableIf<I == 0> next() const noexcept {
    }

    template<std::size_t I>
    constexpr EnableIf<I == 0> previous() const noexcept {
    }

#        ifdef LZ_MSVC
#            pragma warning(push)
#            pragma warning(disable : 4127)
#        endif // LZ_MSVC

    template<std::size_t I>
    LZ_CONSTEXPR_CXX_20 EnableIf<(I > 0)> next() noexcept {
        auto& prev = std::get<I - 1>(_iterator);
        ++prev;

        if (prev == std::get<I - 1>(_end)) {
            if (I != 1) {
                prev = std::get<I - 1>(_begin);
                next<I - 1>();
            }
        }
    }

    template<std::size_t I, class Iter = Decay<decltype(std::get<I>(_iterator))>>
    LZ_CONSTEXPR_CXX_20 EnableIf<IsBidirectional<Iter>::value> doPrev() noexcept {
        --std::get<I>(_iterator);
    }

    template<std::size_t I, class Iter = Decay<decltype(std::get<I>(_iterator))>>
    LZ_CONSTEXPR_CXX_20 EnableIf<!IsBidirectional<Iter>::value> doPrev() {
        using lz::next;
        using std::next;
        const auto& beg = std::get<I>(_begin);
        const auto distance = getIterLength(beg, std::get<I>(_iterator));
        std::get<I>(_iterator) = next(beg, distance - 1);
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

#        ifdef LZ_MSVC
#            pragma warning(pop)
#        endif // LZ_MSVC

    template<std::size_t I>
    LZ_CONSTEXPR_CXX_20 EnableIf<I == 0> operatorPlusImpl(const difference_type offset) {
        using lz::next;
        using std::next;
        auto& iterator = std::get<0>(_iterator);
        iterator = next(std::move(iterator), offset);
    }

    template<std::size_t I>
    EnableIf<(I > 0)> operatorPlusImpl(const difference_type offset) {
        using lz::next;
        using std::next;
        auto& iterator = std::get<I>(_iterator);
        const auto& begin = std::get<I>(_begin);
        const auto& end = std::get<I>(_end);
        difference_type dist;
        if (offset < 0) {
            if (iterator == begin) {
                iterator = end;
                dist = getIterLength(begin, iterator);
            }
            else {
                dist = getIterLength(begin, iterator) + 1;
            }
        }
        else {
            dist = getIterLength(iterator, end);
        }
        const auto offsets = std::lldiv(offset, dist);
        iterator = next(std::move(iterator), static_cast<difference_type>(offsets.rem));
        operatorPlusImpl<I - 1>(static_cast<difference_type>(offsets.quot));
    }

#    else
    template<std::size_t I>
    LZ_CONSTEXPR_CXX_20 void next() {
        if constexpr (I == 0) {
            return;
        }
        else {
            auto& prev = std::get<I - 1>(_iterator);
            ++prev;

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
        using lz::next;
        using std::next;

        auto& iterator = std::get<I>(_iterator);
        if constexpr (I == 0) {
            iterator = next(std::move(iterator), offset);
        }
        else {
            using lz::next;
            using std::next;
            const auto& begin = std::get<I>(_begin);
            const auto& end = std::get<I>(_end);
            difference_type dist;
            if (offset < 0) {
                if (iterator == begin) {
                    iterator = end;
                    dist = getIterLength(begin, iterator);
                }
                else {
                    dist = getIterLength(begin, iterator) + 1;
                }
            }
            else {
                dist = getIterLength(iterator, end);
            }
            const auto [quot, rem] = std::lldiv(offset, dist);
            iterator = next(std::move(iterator), static_cast<difference_type>(rem));
            operatorPlusImpl<I - 1>(static_cast<difference_type>(quot));
        }
    }

    template<std::size_t I>
    LZ_CONSTEXPR_CXX_20 void doPrev() {
        using Iter = Decay<decltype(std::get<I>(_iterator))>;
        if constexpr (IsBidirectional<Iter>::value) {
            --std::get<I>(_iterator);
        }
        else {
            const auto& beg = std::get<I>(_begin);
            const auto distance = std::distance(beg, std::get<I>(_iterator));
            std::get<I>(_iterator) = std::next(beg, std::max(difference_type{ 0 }, distance - 1));
        }
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

#    endif // __cpp_if_constexpr

    template<std::size_t... Is>
    LZ_CONSTEXPR_CXX_20 reference dereference(IndexSequence<Is...>) const {
        return { *std::get<Is>(_iterator)... };
    }

    template<std::size_t... Is>
    LZ_CONSTEXPR_CXX_20 difference_type distanceImpl(IndexSequence<Is...>, const CartesianProductIterator& c) const {
#    ifdef LZ_HAS_CXX_11
        const auto mulFn = std::multiplies<difference_type>();
#    else
        const auto mulFn = std::multiplies<>();
#    endif // LZ_HAS_CXX_11
        const difference_type distances[] = { static_cast<difference_type>(
            getIterLength(std::get<Is>(_iterator), std::get<Is>(c._iterator)))... };
        return std::accumulate(std::begin(distances), std::end(distances), difference_type{ 1 }, mulFn);
    }

    template<std::size_t... Is>
    LZ_CONSTEXPR_CXX_20 bool lessThan(IndexSequence<Is...>, const CartesianProductIterator& c) const {
        const bool allLessThan[] = { (std::get<Is>(_iterator) < std::get<Is>(c._iterator))... };
        const auto end = std::end(allLessThan);
        return std::find(std::begin(allLessThan), end, false) == end;
    }

    using IndexSequenceForThis = MakeIndexSequence<sizeof...(Iterators)>;

    void checkEnd() {
        if (std::get<0>(_iterator) == std::get<0>(_end)) {
            _iterator = _end;
        }
    }

public:
    constexpr CartesianProductIterator() = default;

    LZ_CONSTEXPR_CXX_20
    CartesianProductIterator(std::tuple<Iterators...> iterator, std::tuple<Iterators...> begin, std::tuple<Iterators...> end) :
        _begin(std::move(begin)),
        _iterator(std::move(iterator)),
        _end(std::move(end)) {
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 reference operator*() const {
        return dereference(IndexSequenceForThis());
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 pointer operator->() const {
        return FakePointerProxy<decltype(**this)>(**this);
    }

    LZ_CONSTEXPR_CXX_20 CartesianProductIterator& operator++() {
        next<sizeof...(Iterators)>();
        checkEnd();
        return *this;
    }

    LZ_CONSTEXPR_CXX_20 CartesianProductIterator operator++(int) {
        CartesianProductIterator tmp(*this);
        ++*this;
        return tmp;
    }

    LZ_CONSTEXPR_CXX_20 CartesianProductIterator& operator--() {
        previous<sizeof...(Iterators)>();
        return *this;
    }

    LZ_CONSTEXPR_CXX_20 CartesianProductIterator operator--(int) {
        CartesianProductIterator tmp(*this);
        --*this;
        return tmp;
    }

    LZ_CONSTEXPR_CXX_20 CartesianProductIterator& operator+=(const difference_type offset) {
        operatorPlusImpl<sizeof...(Iterators) - 1>(offset);
        checkEnd();
        return *this;
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 CartesianProductIterator operator+(const difference_type offset) const {
        CartesianProductIterator tmp(*this);
        tmp += offset;
        return tmp;
    }

    LZ_CONSTEXPR_CXX_20 CartesianProductIterator& operator-=(const difference_type offset) {
        return *this += -offset;
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 CartesianProductIterator operator-(const difference_type offset) const {
        CartesianProductIterator tmp(*this);
        tmp -= offset;
        return tmp;
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 friend bool
    operator==(const CartesianProductIterator& lhs, const CartesianProductIterator& rhs) noexcept {
        return lhs._iterator == rhs._iterator;
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 friend bool
    operator!=(const CartesianProductIterator& lhs, const CartesianProductIterator& rhs) noexcept {
        return !(lhs == rhs); // NOLINT
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 friend difference_type
    operator-(const CartesianProductIterator& a, const CartesianProductIterator& b) {
        return b.distanceImpl(IndexSequenceForThis(), a);
    }

    LZ_NODISCARD constexpr friend bool operator<(const CartesianProductIterator& a, const CartesianProductIterator& b) noexcept {
        return a.lessThan(IndexSequenceForThis(), b);
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 friend bool
    operator>(const CartesianProductIterator& a, const CartesianProductIterator& b) noexcept {
        return b < a;
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 friend bool
    operator<=(const CartesianProductIterator& a, const CartesianProductIterator& b) noexcept {
        return !(b < a); // NOLINT
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 friend bool
    operator>=(const CartesianProductIterator& a, const CartesianProductIterator& b) noexcept {
        return !(a < b); // NOLINT
    }
};
} // namespace internal

/**
 * Calculates the distance of a cartesian product iterator. If all iterators are random access iterators, time complexity is O(I),
 * where I is the amount of iterators (`sizeof...(Iterators)`).
 * @param begin Beginning of the sequence.
 * @param end Ending of the sequence
 * @return The difference between a and b.
 */
template<LZ_CONCEPT_ITERATOR... Iterators>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 typename internal::CartesianProductIterator<Iterators...>::difference_type
distance(const internal::CartesianProductIterator<Iterators...>& begin,
         const internal::CartesianProductIterator<Iterators...>& end) {
    return end - begin;
}

/**
 * Gets the nth value from a cartesian product iterator. If all iterators are random access iterators, time complexity is O(I),
 * where I is the amount of iterators (`sizeof...(Iterators)`).
 * @param iter Cartesian product iterator instance.
 * @param offset The amount to add.
 * @return An iterator that contains `iter + offset` value.
 */
template<LZ_CONCEPT_ITERATOR... Iterators>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 internal::CartesianProductIterator<Iterators...>
next(const internal::CartesianProductIterator<Iterators...>& iter,
     const internal::DiffType<internal::CartesianProductIterator<Iterators...>> offset) {
    LZ_ASSERT(offset >= 0, "Cartesian product iterator is not random access/bidirectional, offset must be >= 0");
    return iter + offset;
}
} // namespace lz

#endif // LZ_CARTESIAN_PRODUCT_ITERATOR_HPP
