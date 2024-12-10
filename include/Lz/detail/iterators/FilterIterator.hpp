#pragma once

#ifndef LZ_FILTER_ITERATOR_HPP
#define LZ_FILTER_ITERATOR_HPP

#include "Lz/IterBase.hpp"
#include "Lz/detail/Algorithm.hpp"
#include "Lz/detail/FakePointerProxy.hpp"
#include "Lz/detail/FunctionContainer.hpp"

#include <algorithm>

namespace lz {
namespace detail {
template<class Iterator, class S, class UnaryPredicate>
class FilterIterator;

template<class Iterator, class S, class UnaryPredicate>
using FilterIteratorSentinelSelector = SentinelSelector<IterCat<Iterator>, FilterIterator<Iterator, S, UnaryPredicate>>;

template<class Iterator, class S, class UnaryPredicate>
class FilterIterator
    : public IterBase<FilterIterator<Iterator, S, UnaryPredicate>, RefType<Iterator>, FakePointerProxy<RefType<Iterator>>,
                      DiffType<Iterator>, CommonType<std::bidirectional_iterator_tag, IterCat<Iterator>>,
                      FilterIteratorSentinelSelector<Iterator, S, UnaryPredicate>> {

    using IterTraits = std::iterator_traits<Iterator>;

public:
    using value_type = typename IterTraits::value_type;
    using difference_type = typename IterTraits::difference_type;
    using reference = typename IterTraits::reference;
    using pointer = FakePointerProxy<reference>;

    template<class I = Iterator>
    LZ_CONSTEXPR_CXX_20 EnableIf<std::is_same<I, S>::value, I> find(I first, I last) {
        return std::find_if(std::move(first), std::move(last), _predicate);
    }

    template<class I = Iterator>
    LZ_CONSTEXPR_CXX_20 EnableIf<!std::is_same<I, S>::value, I> find(I first, S last) {
        return detail::findIf(std::move(first), std::move(last), _predicate);
    }

private:
    Iterator _begin{};
    Iterator _iterator{};
    S _end{};
    mutable FunctionContainer<UnaryPredicate> _predicate{};

public:
    LZ_CONSTEXPR_CXX_20 FilterIterator(Iterator iterator, Iterator begin, S end, UnaryPredicate function) :
        _begin(std::move(begin)),
        _iterator(std::move(iterator)),
        _end(std::move(end)),
        _predicate(std::move(function)) {
        if (_iterator == _begin) {
            _iterator = find(std::move(_iterator), _end);
        }
    }

    constexpr FilterIterator() = default;

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 reference dereference() const {
        return *_iterator;
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 pointer arrow() const {
        return FakePointerProxy<decltype(**this)>(**this);
    }

    LZ_CONSTEXPR_CXX_20 void increment() {
        ++_iterator;
        _iterator = find(std::move(_iterator), _end);
    }

    LZ_CONSTEXPR_CXX_20 void decrement() {
        do {
            --_iterator;
        } while (!_predicate(*_iterator) && _iterator != _begin);
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 bool eq(const FilterIterator& b) const noexcept {
        return _iterator == b._iterator;
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 bool eq(DefaultSentinel) const noexcept {
        return _iterator == _end;
    }
};
} // namespace detail
} // namespace lz

#endif