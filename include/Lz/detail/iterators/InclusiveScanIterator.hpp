#pragma once

#ifndef LZ_INCLUSIVE_SCAN_ITERATOR_HPP
#define LZ_INCLUSIVE_SCAN_ITERATOR_HPP

#include "Lz/IterBase.hpp"
#include "Lz/detail/FakePointerProxy.hpp"
#include "Lz/detail/FunctionContainer.hpp"

namespace lz {
namespace detail {
template<class Iterator, class T, class BinaryOp>
class InclusiveScanIterator : public IterBase<InclusiveScanIterator<Iterator, T, BinaryOp>, T&, FakePointerProxy<T&>,
                                              std::ptrdiff_t, std::forward_iterator_tag> {
    mutable T _reducer{};
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

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 reference dereference() const {
        return _reducer;
    }

    LZ_CONSTEXPR_CXX_20 void increment() {
        ++_iterator;
        if (_iterator != _end) {
            _reducer = _binaryOp(std::move(_reducer), *_iterator);
        }
    }

    LZ_NODISCARD constexpr bool eq(const InclusiveScanIterator& b) const {
        return _iterator == b._iterator;
    }
};
} // namespace detail
} // namespace lz

#endif