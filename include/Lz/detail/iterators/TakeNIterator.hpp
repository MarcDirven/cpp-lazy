#pragma once

#include "Lz/IterBase.hpp"
#include "Lz/detail/CompilerChecks.hpp"
#include "Lz/detail/FakePointerProxy.hpp"

#include <iterator>

namespace lz {

template<class DiffType>
struct TakeNSentinel : DefaultSentinel {
    DiffType count{};

    constexpr TakeNSentinel() = default;

    constexpr TakeNSentinel(DiffType count) noexcept : count(count) {
    }
};

namespace detail {

template<class Iterator, class S>
class TakeNIterator;

template<class Iterator>
using TakeNCat = IterCatDecay<IterCat<Iterator>, std::random_access_iterator_tag, std::forward_iterator_tag>;

template<class Iterator, class S>
using TakeNIteratorSentinelSelector =
    SentinelSelector<TakeNCat<Iterator>, TakeNIterator<Iterator, S>, TakeNSentinel<DiffType<Iterator>>>;

template<class Iterator, class S>
class TakeNIterator : public IterBase<TakeNIterator<Iterator, S>, RefType<Iterator>, FakePointerProxy<RefType<Iterator>>,
                                      DiffType<Iterator>, TakeNCat<Iterator>, TakeNIteratorSentinelSelector<Iterator, S>> {

    using IterTraits = std::iterator_traits<Iterator>;

public:
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

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 reference dereference() const {
        return *_iterator;
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 pointer arrow() const {
        return FakePointerProxy<decltype(**this)>(**this);
    }

    LZ_CONSTEXPR_CXX_20 void increment() {
        ++_iterator;
        ++_n;
    }

    LZ_CONSTEXPR_CXX_20 void decrement() {
        --_iterator;
        --_n;
    }

    LZ_CONSTEXPR_CXX_20 void plusIs(const difference_type offset) {
        _iterator += offset;
        _n += offset;
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 difference_type difference(const TakeNIterator& b) const {
        return _iterator - b._iterator;
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 bool eq(const TakeNIterator& b) const noexcept {
        return _n == b._n && _iterator == b._iterator;
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 bool eq(TakeNSentinel<difference_type> sentinel) const noexcept {
        return _n == sentinel.count;
    }
};
}; // namespace detail
} // namespace lz