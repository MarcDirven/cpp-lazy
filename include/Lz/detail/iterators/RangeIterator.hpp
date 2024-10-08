#pragma once

#ifndef LZ_RANGE_ITERATOR_HPP
#define LZ_RANGE_ITERATOR_HPP

#include "Lz/detail/FakePointerProxy.hpp"
#include "Lz/detail/Traits.hpp"

#include <cmath>
#include <iterator>

namespace lz {
namespace detail {
#ifdef __cpp_if_constexpr
template<class ValueType>
std::ptrdiff_t LZ_CONSTEXPR_CXX_20 plusImpl(const ValueType difference,
                                            const ValueType step) noexcept(!std::is_floating_point_v<ValueType>) {
    if constexpr (std::is_floating_point_v<ValueType>) {
        return static_cast<std::ptrdiff_t>(std::ceil(difference / step));
    }
    else {
        return roundEven<std::ptrdiff_t>(difference, step);
    }
}
#else
template<class ValueType>
EnableIf<std::is_floating_point<ValueType>::value, std::ptrdiff_t> plusImpl(const ValueType difference, const ValueType step) {
    return static_cast<std::ptrdiff_t>(std::ceil(difference / step));
}

template<class ValueType>
LZ_CONSTEXPR_CXX_14 EnableIf<!std::is_floating_point<ValueType>::value, std::ptrdiff_t>
plusImpl(const ValueType difference, const ValueType step) noexcept {
    return roundEven<std::ptrdiff_t>(difference, step);
}
#endif // __cpp_if_constexpr

template<class Arithmetic>
class RangeIterator {
    Arithmetic _iterator{};
    Arithmetic _step{};

public:
    using iterator_category = std::random_access_iterator_tag;
    using value_type = Arithmetic;
    using difference_type = std::ptrdiff_t;
    using pointer = Arithmetic;
    using reference = Arithmetic;

    constexpr RangeIterator(const Arithmetic iterator, const Arithmetic step) noexcept : _iterator(iterator), _step(step) {
    }

    constexpr RangeIterator() = default;

    LZ_NODISCARD constexpr value_type operator*() const noexcept {
        return _iterator;
    }

    LZ_NODISCARD constexpr pointer operator->() const noexcept {
        return FakePointerProxy<value_type>(**this);
    }

    LZ_CONSTEXPR_CXX_14 RangeIterator& operator++() noexcept {
        _iterator += _step;
        return *this;
    }

    LZ_CONSTEXPR_CXX_14 RangeIterator operator++(int) noexcept {
        RangeIterator tmp(*this);
        ++*this;
        return tmp;
    }

    LZ_CONSTEXPR_CXX_14 RangeIterator& operator--() noexcept {
        _iterator -= _step;
        return *this;
    }

    LZ_CONSTEXPR_CXX_14 RangeIterator operator--(int) noexcept {
        RangeIterator tmp(*this);
        --*this;
        return tmp;
    }

    LZ_NODISCARD friend std::ptrdiff_t LZ_CONSTEXPR_CXX_20
    operator-(const RangeIterator& a, const RangeIterator& b) noexcept(!std::is_floating_point<Arithmetic>::value) {
        LZ_ASSERT(a._step == b._step, "incompatible iterator types: difference step size");
        const auto difference = a._iterator - b._iterator;

        if LZ_CONSTEXPR_IF (std::is_floating_point<Arithmetic>::value) {
            const auto result = plusImpl(static_cast<Arithmetic>(difference), a._step);
            return std::abs(result);
        }
        else {
            const auto result = plusImpl(static_cast<std::ptrdiff_t>(difference), static_cast<std::ptrdiff_t>(a._step));
            return result < 0 ? -result : result;
        }
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 reference operator[](const difference_type offset) const noexcept {
        return *(*this + offset);
    }

    LZ_CONSTEXPR_CXX_14 RangeIterator& operator+=(const difference_type value) noexcept {
        _iterator += static_cast<Arithmetic>(value) * _step;
        return *this;
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 RangeIterator operator+(const difference_type value) const noexcept {
        RangeIterator tmp(*this);
        tmp += value;
        return tmp;
    }

    LZ_CONSTEXPR_CXX_14 RangeIterator operator-=(const difference_type value) noexcept {
        _iterator -= static_cast<Arithmetic>(value) * _step;
        return *this;
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 RangeIterator operator-(const difference_type value) const noexcept {
        RangeIterator tmp(*this);
        tmp -= value;
        return tmp;
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 friend bool operator!=(const RangeIterator& a, const RangeIterator& b) noexcept {
        LZ_ASSERT(a._step == b._step, "incompatible iterator types: difference step size");
        return a._step < 0 ? a._iterator > b._iterator : a._iterator < b._iterator;
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 friend bool operator==(const RangeIterator& a, const RangeIterator& b) noexcept {
        return !(a != b); // NOLINT
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 friend bool operator<(const RangeIterator& a, const RangeIterator& b) noexcept {
        return a._iterator < b._iterator;
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 friend bool operator>(const RangeIterator& a, const RangeIterator& b) noexcept {
        return b < a; // NOLINT
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 friend bool operator<=(const RangeIterator& a, const RangeIterator& b) noexcept {
        return !(b < a); // NOLINT
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 friend bool operator>=(const RangeIterator& a, const RangeIterator& b) noexcept {
        return !(a < b); // NOLINT
    }
};
} // namespace detail
} // namespace lz

#endif