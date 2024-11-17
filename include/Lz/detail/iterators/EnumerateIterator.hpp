#pragma once

#ifndef LZ_ENUMERATE_ITERATOR_HPP
#define LZ_ENUMERATE_ITERATOR_HPP

#include "Lz/IterBase.hpp"
#include "Lz/detail/FakePointerProxy.hpp"
#include "Lz/detail/Traits.hpp"

namespace lz {
namespace detail {
template<class Iterator, class Arithmetic>
class EnumerateIterator final
    : public IterBase<EnumerateIterator<Iterator, Arithmetic>, std::pair<Arithmetic, RefType<Iterator>>,
                      FakePointerProxy<std::pair<Arithmetic, RefType<Iterator>>>, DiffType<Iterator>, IterCat<Iterator>> {

    Arithmetic _index{};
    Iterator _iterator{};

    using Traits = std::iterator_traits<Iterator>;

public:
    using value_type = std::pair<Arithmetic, typename Traits::value_type>;
    using reference = std::pair<Arithmetic, typename Traits::reference>;
    using pointer = FakePointerProxy<reference>;
    using difference_type = typename Traits::difference_type;
    using iterator_category = typename Traits::iterator_category;

    constexpr EnumerateIterator(const Arithmetic start, Iterator iterator) : _index(start), _iterator(std::move(iterator)) {
    }

    constexpr EnumerateIterator() = default;

    EnumerateIterator& increment() {
        ++_index;
        ++_iterator;
        return *this;
    }

    EnumerateIterator& decrement() {
        --_index;
        --_iterator;
        return *this;
    }

    reference dereference() const {
        return { _index, *_iterator };
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 pointer arrow() const {
        return FakePointerProxy<decltype(**this)>(**this);
    }

    LZ_CONSTEXPR_CXX_20 bool eq(const EnumerateIterator& other) const {
        return _iterator == other._iterator;
    }

    LZ_CONSTEXPR_CXX_20 void plusIs(const difference_type n) {
        _index += static_cast<Arithmetic>(n);
        _iterator += n;
    }

    LZ_CONSTEXPR_CXX_20 difference_type difference(const EnumerateIterator& other) const {
        return _iterator - other._iterator;
    }
};
} // namespace detail
} // namespace lz

#endif
