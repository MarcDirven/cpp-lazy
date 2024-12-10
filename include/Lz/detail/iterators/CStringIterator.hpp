#pragma once

#ifndef LZ_C_STRING_ITERATOR_HPP
#define LZ_C_STRING_ITERATOR_HPP

#include "Lz/IterBase.hpp"
#include "Lz/detail/Traits.hpp"

namespace lz {
namespace detail {

template<class C, class Tag>
class CStringIterator;

template<class C, class Tag>
using CStringSentinelSelector = SentinelSelector<Tag, CStringIterator<C, Tag>>;

template<class C, class Tag>
class CStringIterator
    : public IterBase<CStringIterator<C, Tag>, const C&, const C*, std::ptrdiff_t, Tag, CStringSentinelSelector<C, Tag>> {
    const C* _it{ nullptr };

public:
    using value_type = C;
    using difference_type = std::ptrdiff_t;
    using pointer = const C*;
    using reference = const C&;

    constexpr CStringIterator(const C* it) noexcept : _it(it) {
    }

    constexpr CStringIterator() = default;

    LZ_NODISCARD constexpr reference dereference() const noexcept {
        return *_it;
    }

    LZ_NODISCARD constexpr pointer arrow() const noexcept {
        return _it;
    }

    LZ_CONSTEXPR_CXX_14 void increment() noexcept {
        ++_it;
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 bool eq(const CStringIterator& b) const noexcept {
        if (b._it == nullptr) {
            if (_it == nullptr) {
                return true;
            }
            return *_it == '\0';
        }
        return _it == b._it;
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 bool eq(DefaultSentinel) const noexcept {
        return _it != nullptr && *_it == '\0';
    }

    LZ_CONSTEXPR_CXX_14 void decrement() noexcept {
        --_it;
    }

    LZ_CONSTEXPR_CXX_14 void plusIs(const difference_type offset) noexcept {
        _it += offset;
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 difference_type difference(const CStringIterator& b) const noexcept {
        LZ_ASSERT(_it != nullptr && b._it != nullptr, "Incompatible iterator types");
        return _it - b._it;
    }

    LZ_NODISCARD constexpr explicit operator bool() const noexcept {
        return _it != nullptr && *_it != '\0';
    }
};
} // namespace detail
} // namespace lz

#endif // LZ_C_STRING_ITERATOR_HPP