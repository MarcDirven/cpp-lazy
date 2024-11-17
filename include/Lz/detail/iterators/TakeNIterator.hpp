#pragma once

#include "Lz/IterBase.hpp"
#include "Lz/detail/CompilerChecks.hpp"
#include "Lz/detail/FakePointerProxy.hpp"

#include <iterator>

namespace lz {
namespace detail {
template<class Iterator>
class TakeNIterator : public IterBase<TakeNIterator<Iterator>, RefType<Iterator>, FakePointerProxy<RefType<Iterator>>,
                                      DiffType<Iterator>, CommonType<IterCat<Iterator>, std::random_access_iterator_tag>> {
    using IterTraits = std::iterator_traits<Iterator>;

public:
    // random_access_iterator_tag is the highest iterator category. This is because we can only efficiently jump to the (fake) end
    // of the iterator by adding `_n` if it's a random access iterator.
    using iterator_category = CommonType<IterCat<Iterator>, std::random_access_iterator_tag>;
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
        return _n - b._n;
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 bool eq(const TakeNIterator& b) const noexcept {
        return _n == b._n;
    }
};
}; // namespace detail
} // namespace lz