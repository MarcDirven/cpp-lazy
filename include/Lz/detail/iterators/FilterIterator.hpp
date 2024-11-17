#pragma once

#ifndef LZ_FILTER_ITERATOR_HPP
#define LZ_FILTER_ITERATOR_HPP

#include "Lz/IterBase.hpp"
#include "Lz/detail/FakePointerProxy.hpp"
#include "Lz/detail/FunctionContainer.hpp"

#ifdef LZ_HAS_EXECUTION
#include "Lz/detail/Procs.hpp"
#endif

#include <algorithm>

namespace lz {
namespace detail {
#ifdef LZ_HAS_EXECUTION
template<class Iterator, class UnaryPredicate, class Execution>
class FilterIterator : public IterBase<FilterIterator<Iterator, UnaryPredicate, Execution>, RefType<Iterator>,
                                       FakePointerProxy<RefType<Iterator>>, DiffType<Iterator>, std::forward_iterator_tag> {
#else  // ^^^lz has execution vvv ! lz has execution
template<class Iterator, class UnaryPredicate>
class FilterIterator : public IterBase<FilterIterator<Iterator, UnaryPredicate>, RefType<Iterator>,
                                       FakePointerProxy<RefType<Iterator>>, DiffType<Iterator>, std::forward_iterator_tag> {
#endif // LZ_HAS_EXECUTION

    using IterTraits = std::iterator_traits<Iterator>;

public:
    using iterator_category = std::forward_iterator_tag;
    using value_type = typename IterTraits::value_type;
    using difference_type = typename IterTraits::difference_type;
    using reference = typename IterTraits::reference;
    using pointer = FakePointerProxy<reference>;

    template<class I>
    LZ_CONSTEXPR_CXX_20 I find(I first, I last) {
#ifdef LZ_HAS_EXECUTION
        if constexpr (detail::isCompatibleForExecution<Execution, Iterator>()) {
            return std::find_if(std::move(first), std::move(last), _predicate);
        }
        else {
            return std::find_if(_execution, std::move(first), std::move(last), _predicate);
        }
#else  // ^^^lz has execution vvv ! lz has execution
        return std::find_if(std::move(first), std::move(last), _predicate);
#endif // LZ_HAS_EXECUTION
    }

private:
    Iterator _begin{};
    Iterator _iterator{};
    Iterator _end{};
    mutable FunctionContainer<UnaryPredicate> _predicate{};
#ifdef LZ_HAS_EXECUTION
    LZ_NO_UNIQUE_ADDRESS
    Execution _execution{};
#endif // LZ_HAS_EXECUTION

public:
#ifdef LZ_HAS_EXECUTION
    LZ_CONSTEXPR_CXX_20
    FilterIterator(Iterator iterator, Iterator begin, Iterator end, UnaryPredicate function, Execution execution)
#else  // ^^^lz has execution vvv ! lz has execution
    FilterIterator(Iterator iterator, Iterator begin, Iterator end, UnaryPredicate function)
#endif // LZ_HAS_EXECUTION
        :
        _begin(std::move(begin)),
        _iterator(std::move(iterator)),
        _end(std::move(end)),
        _predicate(std::move(function))
#ifdef LZ_HAS_EXECUTION
        ,
        _execution(execution)
#endif // LZ_HAS_EXECUTION
    {
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

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 bool eq(const FilterIterator& b) const noexcept {
        return _iterator == b._iterator;
    }
};
} // namespace detail
} // namespace lz

#endif