#pragma once

#ifndef LZ_EXCLUSIVE_SCAN_ITERATOR_HPP
#define LZ_EXCLUSIVE_SCAN_ITERATOR_HPP

#include "Lz/detail/FakePointerProxy.hpp"
#include "Lz/detail/FunctionContainer.hpp"

namespace lz {
namespace detail {
template<class Iterator, class T, class BinaryOp>
class ExclusiveScanIterator {
    T _reducer{};
    FunctionContainer<BinaryOp> _binaryOp{};
    Iterator _iterator{};

    using IterTraits = std::iterator_traits<Iterator>;

public:
    using reference = T&;
    using value_type = Decay<reference>;
    using pointer = FakePointerProxy<reference>;
    using difference_type = typename IterTraits::difference_type;
    using iterator_category = std::forward_iterator_tag;

    constexpr ExclusiveScanIterator() = default;

    LZ_CONSTEXPR_CXX_14 ExclusiveScanIterator(Iterator iterator, T init, BinaryOp binaryOp) :
        _reducer(std::move(init)),
        _binaryOp(std::move(binaryOp)),
        _iterator(std::move(iterator)) {
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 typename std::remove_reference<reference>::type const& operator*() const {
        return _reducer;
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 reference operator*() {
        return _reducer;
    }

    LZ_CONSTEXPR_CXX_20 ExclusiveScanIterator& operator++() {
        _reducer = _binaryOp(std::move(_reducer), *_iterator);
        ++_iterator;
        return *this;
    }

    LZ_CONSTEXPR_CXX_20 ExclusiveScanIterator operator++(int) {
        ExclusiveScanIterator tmp(*this);
        ++*this;
        return tmp;
    }

    LZ_NODISCARD constexpr friend bool operator!=(const ExclusiveScanIterator& a, const ExclusiveScanIterator& b) {
        return a._iterator != b._iterator;
    }

    LZ_NODISCARD constexpr friend bool operator==(const ExclusiveScanIterator& a, const ExclusiveScanIterator& b) {
        return !(a != b); // NOLINT
    }
};
} // namespace detail
} // namespace lz

#endif