#pragma once

#ifndef LZ_EXCLUDE_ITERATOR_HPP
#define LZ_EXCLUDE_ITERATOR_HPP

#include "Lz/IterBase.hpp"
#include "Lz/detail/FakePointerProxy.hpp"
#include "Lz/detail/Traits.hpp"

namespace lz {
namespace detail {
template<class Iterator, class S>
class ExcludeIterator : public IterBase<ExcludeIterator<Iterator, S>, RefType<Iterator>, FakePointerProxy<RefType<Iterator>>,
                                        DiffType<Iterator>, std::forward_iterator_tag, S> {

    using IterTraits = std::iterator_traits<Iterator>;

public:
    using value_type = typename IterTraits::value_type;
    using difference_type = typename IterTraits::difference_type;
    using reference = typename IterTraits::reference;
    using pointer = FakePointerProxy<reference>;

private:
    Iterator _iterator{};
    difference_type _index{};
    difference_type _from{};
    difference_type _to{};

public:
    LZ_CONSTEXPR_CXX_20
    ExcludeIterator(Iterator begin, S end, const difference_type from, const difference_type to) :
        _iterator(std::move(begin)),
        _from(from),
        _to(to) {
        if (_iterator != end && _from == 0) {
            _iterator = std::next(std::move(_iterator), _to);
            _index = _to;
        }
    }

    constexpr ExcludeIterator() = default;

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 reference dereference() const {
        return *_iterator;
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 pointer arrow() const {
        return FakePointerProxy<decltype(**this)>(**this);
    }

    LZ_CONSTEXPR_CXX_20 void increment() {
        ++_iterator;
        ++_index;
        if (_index == _from) {
            _iterator = std::next(std::move(_iterator), _to - _from);
        }
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 bool eq(const ExcludeIterator& b) const noexcept {
        LZ_ASSERT(_to == b._to && _from == b._from, "incompatible iterator types: from and to must be equal");
        return _iterator == b._iterator;
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 bool eq(const S& s) const noexcept {
        return _iterator == s;
    }
};
} // namespace detail
} // namespace lz

#endif // LZ_EXCLUDE_ITERATOR_HPP
