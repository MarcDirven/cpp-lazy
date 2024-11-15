#pragma once

#ifndef LZ_REPEAT_ITERATOR_HPP
#define LZ_REPEAT_ITERATOR_HPP

#include "Lz/IterBase.hpp"
#include "Lz/detail/CompilerChecks.hpp"

#include <limits>

namespace lz {
namespace detail {
template<class T>
class RepeatIterator : public IterBase<RepeatIterator<T>, T&, T*, std::ptrdiff_t, std::random_access_iterator_tag> {
    mutable T _toRepeat{};
    std::size_t _amount{};
    std::size_t _iterator{};

public:
    using iterator_category = std::random_access_iterator_tag;
    using value_type = T;
    using difference_type = std::ptrdiff_t;
    using pointer = T*;
    using reference = T&;

    constexpr RepeatIterator(T toRepeat, const std::size_t start, const std::size_t amount) :
        _toRepeat(std::move(toRepeat)),
        _amount(amount),
        _iterator(start) {
    }

    constexpr RepeatIterator() = default;

    LZ_NODISCARD constexpr reference dereference() const noexcept {
        return _toRepeat;
    }

    LZ_NODISCARD constexpr pointer arrow() const noexcept {
        return &_toRepeat;
    }

    LZ_CONSTEXPR_CXX_14 void increment() noexcept {
        if (_amount != (std::numeric_limits<std::size_t>::max)()) {
            ++_iterator;
        }
    }

    LZ_CONSTEXPR_CXX_14 void decrement() noexcept {
        if (_amount != (std::numeric_limits<std::size_t>::max)()) {
            --_iterator;
        }
    }

    LZ_CONSTEXPR_CXX_14 void plusIs(const difference_type offset) noexcept {
        if (_amount != (std::numeric_limits<std::size_t>::max)()) {
            _iterator += static_cast<std::size_t>(offset);
        }
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 difference_type difference(const RepeatIterator& b) const noexcept {
        LZ_ASSERT(_amount == b._amount, "incompatible iterator types: amount of times to repeat not the same");
        return static_cast<difference_type>(_iterator - b._iterator);
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 bool eq(const RepeatIterator& b) const noexcept {
        LZ_ASSERT(_amount == b._amount, "incompatible iterator types: amount of times to repeat not the same");
        return _iterator == b._iterator;
    }

    LZ_NODISCARD constexpr reference operator[](const difference_type) const noexcept {
        return **this;
    }
};
} // namespace detail
} // namespace lz

#endif