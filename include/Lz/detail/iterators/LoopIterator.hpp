#pragma once

#ifndef LZ_LOOP_ITERATOR_HPP
#define LZ_LOOP_ITERATOR_HPP

#include "Lz/IterBase.hpp"
#include "Lz/detail/CompilerChecks.hpp"
#include "Lz/detail/FakePointerProxy.hpp"
#include "Lz/detail/Traits.hpp"

namespace lz {
namespace detail {
template<class Iterator>
class LoopIterator : public IterBase<LoopIterator<Iterator>, RefType<Iterator>, FakePointerProxy<RefType<Iterator>>,
                                     DiffType<Iterator>, IterCat<Iterator>> {
    using IterTraits = std::iterator_traits<Iterator>;

    Iterator _begin{};
    Iterator _iterator{};
    Iterator _end{};

public:
    using reference = typename IterTraits::reference;
    using value_type = typename IterTraits::value_type;
    using pointer = FakePointerProxy<reference>;
    using difference_type = typename IterTraits::difference_type;
    using iterator_category = typename IterTraits::iterator_category;

    LZ_CONSTEXPR_CXX_20 LoopIterator(Iterator iterator, Iterator begin, Iterator end) :
        _begin(std::move(begin)),
        _iterator(std::move(iterator)),
        _end(std::move(end)) {
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 reference dereference() const {
        return *_iterator;
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 pointer arrow() const {
        return FakePointerProxy<decltype(**this)>(**this);
    }

    LZ_CONSTEXPR_CXX_20 void increment() {
        ++_iterator;
        if (_iterator == _end) {
            _iterator = _begin;
        }
    }

    LZ_CONSTEXPR_CXX_20 void decrement() {
        if (_iterator == _begin) {
            _iterator = _end;
        }
        --_iterator;
    }

    LZ_CONSTEXPR_CXX_20 void plusIs(difference_type offset) {
        if (offset >= 0) {
            _iterator += offset % (_end - _begin);
            return;
        }
        offset *= -1;
        const auto dist = _end - _begin;
        offset %= dist;
        _iterator -= offset == 0 ? dist : offset;
    }

    LZ_NODISCARD constexpr difference_type difference(const LoopIterator& it) const noexcept {
        if (it._iterator == _end) {
            return 0;
        }
        return (std::numeric_limits<difference_type>::max)();
    }

    LZ_NODISCARD constexpr bool eq(const LoopIterator&) const noexcept {
        return false;
    }
};
} // namespace detail
} // namespace lz

#endif // LZ_LOOP_ITERATOR_HPP