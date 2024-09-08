#pragma once

#include "Lz/detail/CompilerChecks.hpp"
#include "Lz/detail/FakePointerProxy.hpp"

#include <iterator>

namespace lz {
namespace detail {
template<class Iterator>
class TakeNIterator {
    using IterTraits = std::iterator_traits<Iterator>;

public:
    // random_access_iterator_tag is the highest iterator category. This is because we can only efficiently jump to the (fake) end
    // of the iterator by adding `_n` if it's a random access iterator.
    using iterator_category =
        typename std::common_type<typename IterTraits::iterator_category, std::random_access_iterator_tag>::type;
    using value_type = typename IterTraits::value_type;
    using difference_type = typename IterTraits::difference_type;
    using reference = typename IterTraits::reference;
    using pointer = FakePointerProxy<reference>;

private:
    Iterator _iterator{};
    difference_type _n{};

public:
    constexpr TakeNIterator() = default;

    constexpr TakeNIterator(Iterator iterator, const difference_type n) noexcept : _iterator(iterator), _n(n) {
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 reference operator*() const {
        return *_iterator;
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 pointer operator->() const {
        return FakePointerProxy<decltype(**this)>(**this);
    }

    LZ_CONSTEXPR_CXX_20 TakeNIterator& operator++() {
        ++_iterator;
        ++_n;
        return *this;
    }

    LZ_CONSTEXPR_CXX_20 TakeNIterator operator++(int) {
        TakeNIterator tmp(*this);
        ++*this;
        return tmp;
    }

    LZ_CONSTEXPR_CXX_20 TakeNIterator& operator--() {
        --_iterator;
        --_n;
        return *this;
    }

    LZ_CONSTEXPR_CXX_20 TakeNIterator operator--(int) {
        TakeNIterator tmp(*this);
        --*this;
        return tmp;
    }

    LZ_CONSTEXPR_CXX_20 TakeNIterator& operator+=(const difference_type offset) {
        _iterator += offset;
        _n += offset;
        return *this;
    }

    LZ_CONSTEXPR_CXX_20 TakeNIterator& operator-=(const difference_type offset) {
        _iterator -= offset;
        _n -= offset;
        return *this;
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 TakeNIterator operator+(const difference_type offset) const {
        TakeNIterator tmp(*this);
        tmp += offset;
        return tmp;
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 TakeNIterator operator-(const difference_type offset) const {
        TakeNIterator tmp(*this);
        tmp -= offset;
        return tmp;
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 friend difference_type operator-(const TakeNIterator& a, const TakeNIterator& b) {
        return a._n - b._n;
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 reference operator[](const difference_type offset) const {
        return *(*this + offset);
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 friend bool operator==(const TakeNIterator& a, const TakeNIterator& b) noexcept {
        return !(a != b); // NOLINT
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 friend bool operator!=(const TakeNIterator& a, const TakeNIterator& b) noexcept {
        return a._n != b._n;
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 friend bool operator<(const TakeNIterator& a, const TakeNIterator& b) noexcept {
        return a._n < b._n;
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 friend bool operator>(const TakeNIterator& a, const TakeNIterator& b) noexcept {
        return a._n > b._n;
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 friend bool operator<=(const TakeNIterator& a, const TakeNIterator& b) noexcept {
        return a._n <= b._n;
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 friend bool operator>=(const TakeNIterator& a, const TakeNIterator& b) noexcept {
        return a._n >= b._n;
    }
};
}; // namespace detail
} // namespace lz