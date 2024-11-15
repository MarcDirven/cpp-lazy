#pragma once

#ifndef LZ_RANGE_ITERATOR_HPP
#define LZ_RANGE_ITERATOR_HPP

#include "Lz/IterBase.hpp"
#include "Lz/detail/FakePointerProxy.hpp"
#include "Lz/detail/Traits.hpp"

#include <cmath>
#include <iterator>

namespace lz {
namespace detail {
template<class Arithmetic>
class RangeIterator : public IterBase<RangeIterator<Arithmetic>, Arithmetic, FakePointerProxy<Arithmetic>, std::ptrdiff_t,
                                      std::random_access_iterator_tag> {
    Arithmetic _iterator{};
    Arithmetic _step{};

public:
    using iterator_category = std::random_access_iterator_tag;
    using value_type = Arithmetic;
    using difference_type = std::ptrdiff_t;
    using pointer = FakePointerProxy<Arithmetic>;
    using reference = Arithmetic;

    constexpr RangeIterator(const Arithmetic iterator, const Arithmetic step) noexcept : _iterator(iterator), _step(step) {
    }

    constexpr RangeIterator() = default;

    LZ_NODISCARD constexpr value_type dereference() const noexcept {
        return _iterator;
    }

    LZ_NODISCARD constexpr pointer arrow() const noexcept {
        return FakePointerProxy<value_type>(**this);
    }

    LZ_CONSTEXPR_CXX_14 void increment() noexcept {
        _iterator += _step;
    }

    LZ_CONSTEXPR_CXX_14 void decrement() noexcept {
        _iterator -= _step;
    }

    LZ_NODISCARD std::ptrdiff_t LZ_CONSTEXPR_CXX_20 difference(const RangeIterator& b) const {
        LZ_ASSERT(_step == b._step, "incompatible iterator types: difference step size");
        return static_cast<difference_type>((_iterator - b._iterator) / _step);
    }

    LZ_CONSTEXPR_CXX_14 void plusIs(const difference_type value) noexcept {
        _iterator += static_cast<Arithmetic>(value) * _step;
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 bool eq(const RangeIterator& b) const noexcept {
        LZ_ASSERT(_step == b._step, "incompatible iterator types: difference step size");
        return _step < 0 ? _iterator <= b._iterator : _iterator >= b._iterator;
    }
};
} // namespace detail
} // namespace lz

#endif