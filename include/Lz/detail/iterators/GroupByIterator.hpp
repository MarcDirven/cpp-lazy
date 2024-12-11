#pragma once

#ifndef LZ_GROUP_BY_ITERATOR_HPP
#define LZ_GROUP_BY_ITERATOR_HPP

#include "Lz/IterBase.hpp"
#include "Lz/detail/BasicIterable.hpp"
#include "Lz/detail/FakePointerProxy.hpp"
#include "Lz/detail/FunctionContainer.hpp"
#include "Lz/detail/Traits.hpp"

#include <algorithm>

namespace lz {
namespace detail {
#ifdef LZ_HAS_EXECUTION
template<class Iterator, class Comparer, class Execution>
class GroupByIterator
    : public IterBase<GroupByIterator<Iterator, Comparer, Execution>, std::pair<RefType<Iterator>, BasicIterable<Iterator>>,
                      FakePointerProxy<std::pair<RefType<Iterator>, BasicIterable<Iterator>>>, std::ptrdiff_t,
                      std::forward_iterator_tag> {
#else  // ^^ LZ_HAS_EXECUTION vv !LZ_HAS_EXECUTION

template<class Iterator, class Comparer>
class GroupByIterator
    : public IterBase<GroupByIterator<Iterator, Comparer>, std::pair<RefType<Iterator>, BasicIterable<Iterator>>,
                      FakePointerProxy<std::pair<RefType<Iterator>, BasicIterable<Iterator>>>, std::ptrdiff_t,
                      std::forward_iterator_tag> {
#endif // end LZ_HAS_EXECUTION

    Iterator _subRangeEnd{};
    Iterator _subRangeBegin{};
    Iterator _end{};
    mutable FunctionContainer<Comparer> _comparer{};
#ifdef LZ_HAS_EXECUTION
    LZ_NO_UNIQUE_ADDRESS
    Execution _execution{};
#endif // end LZ_HAS_EXECUTION

    using IterValueType = ValueType<Iterator>;
    using Ref = RefType<Iterator>;

    LZ_CONSTEXPR_CXX_20 void advance() {
        if (_subRangeEnd == _end) {
            return;
        }
        Ref next = *_subRangeEnd;
        ++_subRangeEnd;
#ifdef LZ_HAS_EXECUTION
        if constexpr (detail::isCompatibleForExecution<Execution, Iterator>()) {
            _subRangeEnd = std::find_if(std::move(_subRangeEnd), _end,
                                        [this, &next](const IterValueType& v) { return !_comparer(v, next); });
        }
        else {
            _subRangeEnd = std::find_if(_execution, std::move(_subRangeEnd), _end,
                                        [this, &next](const IterValueType& v) { return !_comparer(v, next); });
        }
#else
        _subRangeEnd =
            std::find_if(std::move(_subRangeEnd), _end, [this, &next](const IterValueType& v) { return !_comparer(v, next); });
#endif
    }

public:
    using iterator_category = std::forward_iterator_tag;
    using value_type = std::pair<Decay<Ref>, BasicIterable<Iterator>>;
    using reference = std::pair<Ref, BasicIterable<Iterator>>;
    using pointer = FakePointerProxy<reference>;
    using difference_type = std::ptrdiff_t;

    constexpr GroupByIterator() = default;

#ifdef LZ_HAS_EXECUTION
    LZ_CONSTEXPR_CXX_20 GroupByIterator(Iterator begin, Iterator end, Comparer comparer, Execution execution) :
#else  // ^^ LZ_HAS_EXECUTION vv !LZ_HAS_EXECUTION

    GroupByIterator(Iterator begin, Iterator end, Comparer comparer) :
#endif // end LZ_HAS_EXECUTION
        _subRangeEnd(begin),
        _subRangeBegin(std::move(begin)),
        _end(std::move(end)),
        _comparer(std::move(comparer))
#ifdef LZ_HAS_EXECUTION
        ,
        _execution(execution)
#endif // end LZ_HAS_EXECUTION
    {
        if (_subRangeBegin == _end) {
            return;
        }
        advance();
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 reference dereference() const {
        return { *_subRangeBegin, { _subRangeBegin, _subRangeEnd } };
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 pointer arrow() const {
        return FakePointerProxy<decltype(**this)>(**this);
    }

    LZ_CONSTEXPR_CXX_20 void increment() {
        _subRangeBegin = _subRangeEnd;
        advance();
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 bool eq(const GroupByIterator& rhs) const noexcept {
        return _subRangeBegin == rhs._subRangeBegin;
    }
};
} // namespace detail
} // namespace lz
#endif // LZ_GROUP_BY_ITERATOR_HPP
