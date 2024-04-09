#pragma once

#ifndef LZ_C_STRING_ITERATOR_HPP
#define LZ_C_STRING_ITERATOR_HPP

#include "LzTools.hpp"

namespace lz {
namespace internal {
template<class C, bool IsRandomAccess>
class CStringIterator {
    const C* _it{ nullptr };

public:
    using iterator_category = Conditional<IsRandomAccess, std::random_access_iterator_tag, std::forward_iterator_tag>;
    using value_type = C;
    using difference_type = std::ptrdiff_t;
    using pointer = const C*;
    using reference = const C&;

    constexpr CStringIterator(const C* it) noexcept : _it(it) {
    }

    constexpr CStringIterator() = default;

    LZ_NODISCARD constexpr reference operator*() const noexcept {
        return *_it;
    }

    LZ_NODISCARD constexpr pointer operator->() const noexcept {
        return _it;
    }

    LZ_CONSTEXPR_CXX_14 CStringIterator& operator++() noexcept {
        ++_it;
        return *this;
    }

    LZ_CONSTEXPR_CXX_14 CStringIterator operator++(int) noexcept {
        CStringIterator tmp(*this);
        ++*this;
        return tmp;
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 friend bool operator!=(const CStringIterator& a, const CStringIterator& b) noexcept {
        if (b._it == nullptr) {
            if (a._it == nullptr) {
                return false;
            }
            return *a._it != '\0';
        }
        return a._it != b._it;
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 friend bool operator==(const CStringIterator& a, const CStringIterator& b) noexcept {
        return !(a != b); // NOLINT
    }

    LZ_CONSTEXPR_CXX_14 CStringIterator& operator--() noexcept {
        --_it;
        return *this;
    }

    LZ_CONSTEXPR_CXX_14 CStringIterator operator--(int) noexcept {
        CStringIterator tmp(*this);
        --*this;
        return tmp;
    }

    LZ_CONSTEXPR_CXX_14 CStringIterator& operator+=(const difference_type offset) noexcept {
        _it += offset;
        return *this;
    }

    LZ_CONSTEXPR_CXX_14 CStringIterator& operator-=(const difference_type offset) noexcept {
        _it -= offset;
        return *this;
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 CStringIterator operator+(const difference_type offset) const noexcept {
        CStringIterator tmp(*this);
        tmp += offset;
        return tmp;
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 CStringIterator operator-(const difference_type offset) const noexcept {
        CStringIterator tmp(*this);
        tmp -= offset;
        return tmp;
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 friend difference_type operator-(const CStringIterator& a, const CStringIterator& b) noexcept {
        LZ_ASSERT(a._it != nullptr && b._it != nullptr, "Incompatible iterator types");
        return a._it - b._it;
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 value_type operator[](const difference_type offset) const noexcept {
        return *(*this + offset);
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 friend bool operator<(const CStringIterator& a, const CStringIterator& b) noexcept {
        LZ_ASSERT(a._it != nullptr && b._it != nullptr, "Incompatible iterator types");
        return a._it < b._it;
    }

    LZ_NODISCARD constexpr friend bool operator>(const CStringIterator& a, const CStringIterator& b) noexcept {
        return b < a;
    }

    LZ_NODISCARD constexpr friend bool operator<=(const CStringIterator& a, const CStringIterator& b) noexcept {
        return !(b < a); // NOLINT
    }

    LZ_NODISCARD constexpr friend bool operator>=(const CStringIterator& a, const CStringIterator& b) noexcept {
        return !(a < b); // NOLINT
    }

    LZ_NODISCARD constexpr explicit operator bool() const noexcept {
        return _it != nullptr && *_it != '\0';
    }
};
} // namespace internal
} // namespace lz

#endif // LZ_C_STRING_ITERATOR_HPP