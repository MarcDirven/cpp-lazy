#pragma once

#ifndef LZ_EXCEPT_ITERATOR_HPP
#define LZ_EXCEPT_ITERATOR_HPP

#include "FunctionContainer.hpp"

#include <algorithm>

namespace lz {
namespace internal {
#ifdef LZ_HAS_EXECUTION
template<LZ_CONCEPT_ITERATOR Iterator, LZ_CONCEPT_ITERATOR IteratorToExcept, class Compare, class Execution>
#else  // ^^^ has execution vvv ! has execution
template<LZ_CONCEPT_ITERATOR Iterator, LZ_CONCEPT_ITERATOR IteratorToExcept, class Compare>
#endif // LZ_HAS_EXECUTION
class ExceptIterator {
    using IterTraits = std::iterator_traits<Iterator>;

public:
    using iterator_category = std::forward_iterator_tag;
    using value_type = typename IterTraits::value_type;
    using difference_type = typename IterTraits::difference_type;
    using reference = typename IterTraits::reference;
    using pointer = FakePointerProxy<reference>;

private:
    Iterator _iterator{};
    Iterator _end{};
    IteratorToExcept _toExceptBegin{};
    IteratorToExcept _toExceptEnd{};
    mutable FunctionContainer<Compare> _compare{};
#ifdef LZ_HAS_EXECUTION
    LZ_NO_UNIQUE_ADDRESS
    Execution _execution{};
#endif // LZ_HAS_EXECUTION
    LZ_CONSTEXPR_CXX_20 void find() {
#ifdef LZ_HAS_EXECUTION
        if constexpr (internal::checkForwardAndPolicies<Execution, Iterator>()) {
            _iterator = std::find_if(std::move(_iterator), _end, [this](const value_type& value) {
                return !std::binary_search(_toExceptBegin, _toExceptEnd, value, _compare);
            });
        }
        else { // NOLINT
            _iterator = std::find_if(_execution, std::move(_iterator), _end, [this](const value_type& value) {
                return !std::binary_search(_toExceptBegin, _toExceptEnd, value, _compare);
            });
        }
#else  // ^^^ has execution vvv ! has execution
        _iterator = std::find_if(std::move(_iterator), _end, [this](const value_type& value) {
            return !std::binary_search(_toExceptBegin, _toExceptEnd, value, _compare);
        });
#endif // LZ_HAS_EXECUTION
    }

public:
    constexpr ExceptIterator() = default;

#ifdef LZ_HAS_EXECUTION
    LZ_CONSTEXPR_CXX_20 ExceptIterator(Iterator begin, Iterator end, IteratorToExcept toExceptBegin, IteratorToExcept toExceptEnd,
                                       Compare compare, Execution execution) :
#else  // ^^^ has execution vvv ! has execution
    ExceptIterator(Iterator begin, Iterator end, IteratorToExcept toExceptBegin, IteratorToExcept toExceptEnd, Compare compare) :
#endif // LZ_HAS_EXECUTION
        _iterator(std::move(begin)),
        _end(std::move(end)),
        _toExceptBegin(std::move(toExceptBegin)),
        _toExceptEnd(std::move(toExceptEnd)),
        _compare(std::move(compare))
#ifdef LZ_HAS_EXECUTION
        ,
        _execution(execution)
#endif // LZ_HAS_EXECUTION
    {
        if (_toExceptBegin == _toExceptEnd) {
            return;
        }
        find();
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 reference operator*() const {
        return *_iterator;
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 pointer operator->() const {
        return FakePointerProxy<decltype(**this)>(**this);
    }

    LZ_CONSTEXPR_CXX_20 ExceptIterator& operator++() {
        ++_iterator;
        find();
        return *this;
    }

    LZ_CONSTEXPR_CXX_20 ExceptIterator operator++(int) {
        ExceptIterator tmp(*this);
        ++*this;
        return tmp;
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 friend bool operator!=(const ExceptIterator& a, const ExceptIterator& b) noexcept {
        return a._iterator != b._iterator;
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 friend bool operator==(const ExceptIterator& a, const ExceptIterator& b) noexcept {
        return !(a != b); // NOLINT
    }
};
} // namespace internal
} // namespace lz

#endif