#pragma once

#ifndef LZ_MAP_ITERATOR_HPP
#define LZ_MAP_ITERATOR_HPP

#include "FunctionContainer.hpp"
#include "LzTools.hpp"

namespace lz {
namespace internal {
template<LZ_CONCEPT_ITERATOR Iterator, class Function>
class MapIterator {
    Iterator _iterator{};
    mutable FunctionContainer<Function> _function{};

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

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 reference operator*() const {
        return _function(*_iterator);
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 pointer operator->() const {
        return FakePointerProxy<decltype(**this)>(**this);
    }

    LZ_CONSTEXPR_CXX_20 MapIterator& operator++() {
        ++_iterator;
        return *this;
    }

    LZ_CONSTEXPR_CXX_20 MapIterator operator++(int) {
        MapIterator tmp(*this);
        ++*this;
        return tmp;
    }

    LZ_CONSTEXPR_CXX_20 MapIterator& operator--() {
        --_iterator;
        return *this;
    }

    LZ_CONSTEXPR_CXX_20 MapIterator operator--(int) {
        MapIterator tmp(*this);
        --*this;
        return tmp;
    }

    LZ_CONSTEXPR_CXX_20 MapIterator& operator+=(const difference_type offset) {
        _iterator += offset;
        return *this;
    }

    LZ_CONSTEXPR_CXX_20 MapIterator& operator-=(const difference_type offset) {
        _iterator -= offset;
        return *this;
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 MapIterator operator+(const difference_type offset) const {
        MapIterator tmp(*this);
        tmp += offset;
        return tmp;
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 MapIterator operator-(const difference_type offset) const {
        MapIterator tmp(*this);
        tmp -= offset;
        return tmp;
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 friend difference_type operator-(const MapIterator& a, const MapIterator& b) {
        return a._iterator - b._iterator;
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 reference operator[](const difference_type offset) const {
        return *(*this + offset);
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 friend bool operator==(const MapIterator& a, const MapIterator& b) {
        return !(a != b); // NOLINT
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 friend bool operator!=(const MapIterator& a, const MapIterator& b) {
        return a._iterator != b._iterator;
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 friend bool operator<(const MapIterator& a, const MapIterator& b) {
        return a._iterator < b._iterator;
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 friend bool operator>(const MapIterator& a, const MapIterator& b) {
        return b < a;
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 friend bool operator<=(const MapIterator& a, const MapIterator& b) {
        return !(b < a); // NOLINT
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 friend bool operator>=(const MapIterator& a, const MapIterator& b) {
        return !(a < b); // NOLINT
    }
};
} // namespace internal
} // namespace lz

#endif