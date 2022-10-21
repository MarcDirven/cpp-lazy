#pragma once

#ifndef LZ_FILTER_ITERATOR_HPP
#define LZ_FILTER_ITERATOR_HPP

#include "FunctionContainer.hpp"
#include "LzTools.hpp"

#include <algorithm>

namespace lz {
namespace internal {
#ifdef LZ_HAS_EXECUTION
template<class Iterator, class UnaryPredicate, class Execution>
#else  // ^^^lz has execution vvv ! lz has execution
template<class Iterator, class UnaryPredicate>
#endif // LZ_HAS_EXECUTION
class FilterIterator {
    using IterTraits = std::iterator_traits<Iterator>;

public:
    using iterator_category = typename std::common_type<std::forward_iterator_tag, typename IterTraits::iterator_category>::type;
    using value_type = typename IterTraits::value_type;
    using difference_type = typename IterTraits::difference_type;
    using reference = typename IterTraits::reference;
    using pointer = FakePointerProxy<reference>;

    template<class I>
    LZ_CONSTEXPR_CXX_20 I find(I first, I last) {
#ifdef LZ_HAS_EXECUTION
        if constexpr (internal::checkForwardAndPolicies<Execution, Iterator>()) {
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
    LZ_CONSTEXPR_CXX_20 FilterIterator(Iterator iterator, Iterator begin, Iterator end, UnaryPredicate function, Execution execution)
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

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 reference operator*() const {
        return *_iterator;
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 pointer operator->() const {
        return FakePointerProxy<decltype(**this)>(**this);
    }

    LZ_CONSTEXPR_CXX_20 FilterIterator& operator++() {
        ++_iterator;
        _iterator = find(std::move(_iterator), _end);
        return *this;
    }

    LZ_CONSTEXPR_CXX_20 FilterIterator operator++(int) {
        FilterIterator tmp(*this);
        ++*this;
        return tmp;
    }

    LZ_CONSTEXPR_CXX_20 FilterIterator& operator--() {
        std::reverse_iterator<Iterator> iterator(std::move(_iterator));
        std::reverse_iterator<Iterator> rBegin(_begin);
        _iterator = find(std::move(iterator), std::move(rBegin)).base();
        --_iterator;
        return *this;
    }

    LZ_CONSTEXPR_CXX_20 FilterIterator operator--(int) {
        FilterIterator tmp(*this);
        ++*this;
        return *this;
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 friend bool operator!=(const FilterIterator& a, const FilterIterator& b) noexcept {
        return a._iterator != b._iterator;
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 friend bool operator==(const FilterIterator& a, const FilterIterator& b) noexcept {
        return !(a != b); // NOLINT
    }
};
} // namespace internal
} // namespace lz

#endif