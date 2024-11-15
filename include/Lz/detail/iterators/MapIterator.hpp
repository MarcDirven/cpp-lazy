#pragma once

#ifndef LZ_MAP_ITERATOR_HPP
#define LZ_MAP_ITERATOR_HPP

#include "Lz/IterBase.hpp"
#include "Lz/detail/CompilerChecks.hpp"
#include "Lz/detail/FakePointerProxy.hpp"
#include "Lz/detail/FunctionContainer.hpp"

namespace lz {
namespace detail {
template<class Iterator, class Function>
class MapIterator : public IterBase<MapIterator<Iterator, Function>, IteratorFnRetT<Function, Iterator>,
                                    FakePointerProxy<IteratorFnRetT<Function, Iterator>>, DiffType<Iterator>, IterCat<Iterator>> {
    Iterator _iterator{};
    FunctionContainer<Function> _function{};

    using IterTraits = std::iterator_traits<Iterator>;

public:
    using reference = decltype(_function(*_iterator));
    using value_type = Decay<reference>;
    using iterator_category = typename IterTraits::iterator_category;
    using difference_type = typename IterTraits::difference_type;
    using pointer = FakePointerProxy<reference>;

    LZ_CONSTEXPR_CXX_20 MapIterator(Iterator iterator, Function function) :
        _iterator(std::move(iterator)),
        _function(std::move(function)) {
    }

    constexpr MapIterator() = default;

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 reference dereference() const {
        return _function(*_iterator);
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 pointer arrow() const {
        return FakePointerProxy<decltype(**this)>(**this);
    }

    LZ_CONSTEXPR_CXX_20 void increment() {
        ++_iterator;
    }

    LZ_CONSTEXPR_CXX_20 void decrement() {
        --_iterator;
    }

    LZ_CONSTEXPR_CXX_20 void plusIs(const difference_type offset) {
        _iterator += offset;
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 difference_type difference(const MapIterator& b) const {
        return _iterator - b._iterator;
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 bool eq(const MapIterator& b) const noexcept {
        return _iterator == b._iterator;
    }
};
} // namespace detail
} // namespace lz

#endif