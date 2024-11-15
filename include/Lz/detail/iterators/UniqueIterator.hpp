#pragma once

#ifndef LZ_UNIQUE_ITERATOR_HPP
#define LZ_UNIQUE_ITERATOR_HPP

#include "Lz/IterBase.hpp"
#include "Lz/detail/CompilerChecks.hpp"
#include "Lz/detail/FakePointerProxy.hpp"
#include "Lz/detail/FunctionContainer.hpp"

#ifdef LZ_HAS_EXECUTION
#include "Procs.hpp"
#endif

#include <algorithm>

namespace lz {
namespace detail {
#ifdef LZ_HAS_EXECUTION
template<class Execution, class Iterator, class Compare>
class UniqueIterator : public IterBase<UniqueIterator<Execution, Iterator, Compare>, RefType<Iterator>,
                                       FakePointerProxy<RefType<Iterator>>, DiffType<Iterator>, std::forward_iterator_tag> {
#else  // ^^^ lz has execution vvv ! lz has execution
template<class Iterator, class Compare>
class UniqueIterator : public IterBase<UniqueIterator<Iterator, Compare>, RefType<Iterator>,
                                       FakePointerProxy<RefType<Iterator>>, DiffType<Iterator>, std::forward_iterator_tag> {
#endif // LZ_HAS_EXECUTION
    using IterTraits = std::iterator_traits<Iterator>;

    Iterator _iterator{};
    Iterator _end{};
    mutable FunctionContainer<Compare> _compare{};
#ifdef LZ_HAS_EXECUTION
    LZ_NO_UNIQUE_ADDRESS
    Execution _execution;
#endif // LZ_HAS_EXECUTION

public:
    using iterator_category = std::forward_iterator_tag;
    using value_type = typename IterTraits::value_type;
    using difference_type = typename IterTraits::difference_type;
    using reference = typename IterTraits::reference;
    using pointer = FakePointerProxy<reference>;

#ifdef LZ_HAS_EXECUTION
    constexpr UniqueIterator(Iterator begin, Iterator end, Compare compare, Execution execution)
#else  // ^^^ lz has execution vvv ! lz has execution
    constexpr UniqueIterator(Iterator begin, Iterator end, Compare compare)
#endif // LZ_HAS_EXECUTION
        :
        _iterator(std::move(begin)),
        _end(std::move(end)),
        _compare(std::move(compare))
#ifdef LZ_HAS_EXECUTION
        ,
        _execution(execution)
#endif // LZ_HAS_EXECUTION
    {
    }

    constexpr UniqueIterator() = default;

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 reference dereference() const {
        return *_iterator;
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 pointer arrow() const {
        return FakePointerProxy<decltype(**this)>(**this);
    }

    LZ_CONSTEXPR_CXX_20 void increment() {
#ifdef LZ_HAS_EXECUTION
        if constexpr (detail::isCompatibleForExecution<Execution, Iterator>()) {
            _iterator = std::adjacent_find(std::move(_iterator), _end, _compare);
        }
        else {
            _iterator = std::adjacent_find(_execution, std::move(_iterator), _end, _compare);
        }
#else  // ^^^ lz has execution vvv ! lz has execution
        _iterator = std::adjacent_find(std::move(_iterator), _end, _compare);
#endif // LZ_HAS_EXECUTION

        if (_iterator != _end) {
            ++_iterator;
        }
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 bool eq(const UniqueIterator& b) const noexcept {
        return _iterator == b._iterator;
    }
};
} // namespace detail
} // namespace lz

#endif