#pragma once

#ifndef LZ_EXCLUSIVE_SCAN_ITERATOR_HPP
#define LZ_EXCLUSIVE_SCAN_ITERATOR_HPP

#include "Lz/IterBase.hpp"
#include "Lz/detail/FakePointerProxy.hpp"
#include "Lz/detail/FunctionContainer.hpp"

namespace lz {
namespace detail {
template<class Iterator, class T, class BinaryOp>
class ExclusiveScanIterator : public IterBase<ExclusiveScanIterator<Iterator, T, BinaryOp>, T&, FakePointerProxy<T&>,
                                              DiffType<Iterator>, std::forward_iterator_tag> {
    mutable T _reducer{};
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

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 reference dereference() const {
        return _reducer;
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 pointer arrow() const {
        return FakePointerProxy<decltype(**this)>(**this);
    }

    LZ_CONSTEXPR_CXX_20 void increment() {
        _reducer = _binaryOp(std::move(_reducer), *_iterator);
        ++_iterator;
    }

    LZ_NODISCARD constexpr bool eq(const ExclusiveScanIterator& b) const {
        return _iterator == b._iterator;
    }
};
} // namespace detail
} // namespace lz

#endif