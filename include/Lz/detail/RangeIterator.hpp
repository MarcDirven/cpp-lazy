#pragma once

#ifndef LZ_RANGE_ITERATOR_HPP
#define LZ_RANGE_ITERATOR_HPP

#include <cmath>
#include <iterator>

namespace lz {
namespace internal {
template<class Arithmetic>
class RangeIterator {
    Arithmetic _iterator{};
    Arithmetic _step{};

public:
    using iterator_category = std::forward_iterator_tag;
    using value_type = Arithmetic;
    using difference_type = typename std::make_signed<Arithmetic>::type;
    using pointer = Arithmetic*;
    using reference = Arithmetic&;

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

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 friend bool operator!=(const RangeIterator& a, const RangeIterator& b) noexcept {
        LZ_ASSERT(a._step == b._step, "incompatible iterator types: difference step size");
        if (a._step < 0) {
            return a._iterator > b._iterator;
        }
        return a._iterator < b._iterator;
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 friend bool operator==(const RangeIterator& a, const RangeIterator& b) noexcept {
        return !(a != b); // NOLINT
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 friend difference_type operator-(const RangeIterator& a, const RangeIterator& b) {
        LZ_ASSERT(a._step == b._step, "incompatible iterator types: difference step size");
        const auto difference = static_cast<difference_type>(a._iterator) - static_cast<difference_type>(b._iterator);
        if (a._step == 1) {
            return difference;
        }
        return static_cast<difference_type>(std::ceil(difference / static_cast<float>(a._step)));
    }

    LZ_NODISCARD constexpr RangeIterator operator+(const Arithmetic value) const noexcept {
        return RangeIterator(_iterator + value * _step, _step);
    }
};
} // namespace internal

/**
 * Gets the distance between begin and end. Distance is always O(1).
 * @param begin Beginning of the sequence.
 * @param end Ending of the sequence.
 * @return The distance between begin and end.
 */
template<LZ_CONCEPT_ARITHMETIC Arithmetic>
LZ_NODISCARD LZ_CONSTEXPR_CXX_14 typename internal::RangeIterator<Arithmetic>::difference_type
distance(const internal::RangeIterator<Arithmetic>& a, const internal::RangeIterator<Arithmetic>& b) {
    return b - a;
}

/**
 * Gets the nth value from iter. Next is always O(1).
 * @param iter A chunks iterator instance.
 * @param value The amount to add.
 * @return A chunks iterator with offset iter + value.
 */
template<LZ_CONCEPT_ARITHMETIC Arithmetic>
LZ_NODISCARD constexpr internal::RangeIterator<Arithmetic>
next(const internal::RangeIterator<Arithmetic>& iter, const internal::DiffType<internal::RangeIterator<Arithmetic>> value) {
    LZ_ASSERT(value >= 0, "offset must be greater than 0 since this is not a bidirectional/random access iterator");
    return iter + value;
}
} // namespace lz

#endif