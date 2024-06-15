#pragma once

#ifndef LZ_INCLUSIVE_SCAN_ITERATOR_HPP
#define LZ_INCLUSIVE_SCAN_ITERATOR_HPP

#include "FunctionContainer.hpp"

namespace lz {
namespace internal {
template<class Iterator, class T, class BinaryOp>
class InclusiveScanIterator {
    T _reducer{};
    FunctionContainer<BinaryOp> _binaryOp{};
    Iterator _iterator{};
    Iterator _end{};

    using IterTraits = std::iterator_traits<Iterator>;

public:
    using reference = T&;
    using value_type = Decay<reference>;
    using pointer = FakePointerProxy<reference>;
    using difference_type = typename IterTraits::difference_type;
    using iterator_category = std::forward_iterator_tag;

    constexpr InclusiveScanIterator() = default;

    LZ_CONSTEXPR_CXX_14 InclusiveScanIterator(Iterator iterator, Iterator end, T init, BinaryOp binaryOp) :
        _reducer(std::move(init)),
        _binaryOp(std::move(binaryOp)),
        _iterator(std::move(iterator)),
        _end(std::move(end)) {
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 typename std::remove_reference<reference>::type const& operator*() const {
        return _reducer;
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 reference operator*() {
        return _reducer;
    }

    LZ_CONSTEXPR_CXX_20 InclusiveScanIterator& operator++() {
        ++_iterator;
        if (_iterator != _end) {
            _reducer = _binaryOp(std::move(_reducer), *_iterator);
        }
        return *this;
    }

    LZ_CONSTEXPR_CXX_20 InclusiveScanIterator operator++(int) {
        InclusiveScanIterator tmp(*this);
        ++*this;
        return tmp;
    }

    LZ_NODISCARD constexpr friend bool operator!=(const InclusiveScanIterator& a, const InclusiveScanIterator& b) {
        return a._iterator != b._iterator;
    }

    LZ_NODISCARD constexpr friend bool operator==(const InclusiveScanIterator& a, const InclusiveScanIterator& b) {
        return !(a != b); // NOLINT
    }
};
} // namespace internal
} // namespace lz

#endif