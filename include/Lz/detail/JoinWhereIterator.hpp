#pragma once

#ifndef LZ_LEFT_JOIN_ITERATOR_HPP
#define LZ_LEFT_JOIN_ITERATOR_HPP

#include "FunctionContainer.hpp"
#include "LzTools.hpp"

namespace lz {
namespace internal {
#ifdef LZ_HAS_EXECUTION
template<class IterA, class IterB, class SelectorA, class SelectorB, class ResultSelector, class Execution>
#else

template<class IterA, class IterB, class SelectorA, class SelectorB, class ResultSelector>
#endif // LZ_HAS_EXECUTION
class JoinWhereIterator {
private:
    using IterTraitsA = std::iterator_traits<IterA>;
    using IterTraitsB = std::iterator_traits<IterB>;
    using ValueTypeB = typename IterTraitsB::value_type;
    using RefTypeA = typename IterTraitsA::reference;

    using SelectorARetVal = Decay<FunctionReturnType<SelectorA, RefTypeA>>;

    IterA _iterA{};
    IterA _endA{};
    IterB _iterB{};
    IterB _beginB{};
    IterB _endB{};
#ifdef LZ_HAS_EXECUTION
    Execution _exec{};
#endif // LZ_HAS_EXECUTION
    mutable FunctionContainer<SelectorA> _selectorA{};
    mutable FunctionContainer<SelectorB> _selectorB{};
    mutable FunctionContainer<ResultSelector> _resultSelector{};

    LZ_CONSTEXPR_CXX_20 void findNext() {
#ifdef LZ_HAS_EXECUTION
        if constexpr (checkForwardAndPolicies<Execution, IterA>()) {
            std::mutex mutex;
            _iterA = std::find_if(_exec, _iterA, _endA, [this, &mutex](const ValueType<IterA>& a) {
                std::lock_guard guard(mutex);
                auto&& toFind = _selectorA(a);
                _iterB =
                    std::lower_bound(std::move(_iterB), _endB, toFind,
                                     [this](const ValueTypeB& b, const SelectorARetVal& val) { return _selectorB(b) < val; });
                if (_iterB != _endB && !(toFind < _selectorB(*_iterB))) { // NOLINT
                    return true;
                }
                _iterB = _beginB;
                return false;
            });
        }
        else {
            _iterA = std::find_if(_iterA, _endA, [this](const ValueType<IterA>& a) {
                auto&& toFind = _selectorA(a);
                _iterB =
                    std::lower_bound(std::move(_iterB), _endB, toFind,
                                     [this](const ValueTypeB& b, const SelectorARetVal& val) { return _selectorB(b) < val; });
                if (_iterB != _endB && !(toFind < _selectorB(*_iterB))) { // NOLINT
                    return true;
                }
                _iterB = _beginB;
                return false;
            });
        }
#else
        _iterA = std::find_if(_iterA, _endA, [this](const ValueType<IterA>& a) {
            auto&& toFind = _selectorA(a);
            _iterB = std::lower_bound(std::move(_iterB), _endB, toFind,
                                      [this](const ValueTypeB& b, const SelectorARetVal& val) { return _selectorB(b) < val; });
            if (_iterB != _endB && !(toFind < _selectorB(*_iterB))) { // NOLINT
                return true;
            }
            _iterB = _beginB;
            return false;
        });
#endif // LZ_HAS_EXECUTION
    }

public:
    using reference = decltype(_resultSelector(*_iterA, *_iterB));
    using value_type = Decay<reference>;
    using iterator_category = std::forward_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using pointer = FakePointerProxy<reference>;

#ifdef LZ_HAS_EXECUTION
    JoinWhereIterator(IterA iterA, IterA endA, IterB iterB, IterB endB, SelectorA a, SelectorB b, ResultSelector resultSelector,
                      Execution execution)
#else
    LZ_CONSTEXPR_CXX_20
    JoinWhereIterator(IterA iterA, IterA endA, IterB iterB, IterB endB, SelectorA a, SelectorB b, ResultSelector resultSelector)
#endif // LZ_HAS_EXECUTION
        :
        _iterA(std::move(iterA)),
        _endA(std::move(endA)),
        _iterB(iterB),
        _beginB(iterB == endB ? endB : std::move(iterB)),
        _endB(std::move(endB)),
#ifdef LZ_HAS_EXECUTION
        _exec(execution),
#endif // LZ_HAS_EXECUTION
        _selectorA(std::move(a)),
        _selectorB(std::move(b)),
        _resultSelector(std::move(resultSelector)) {
        if (_iterA == _endA || _iterB == _endB) {
            return;
        }
        findNext();
    }

    constexpr JoinWhereIterator() = default;

    LZ_NODISCARD LZ_CONSTEXPR_CXX_17 reference operator*() const {
        return _resultSelector(*_iterA, *_iterB);
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_17 pointer operator->() const {
        return FakePointerProxy<decltype(**this)>(**this);
    }

    LZ_CONSTEXPR_CXX_17 JoinWhereIterator& operator++() {
        ++_iterB;
        findNext();
        return *this;
    }

    LZ_CONSTEXPR_CXX_17 JoinWhereIterator operator++(int) {
        JoinWhereIterator tmp(*this);
        ++*this;
        return tmp;
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_17 friend bool operator==(const JoinWhereIterator& a, const JoinWhereIterator& b) {
        return a._iterA == b._iterA;
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_17 friend bool operator!=(const JoinWhereIterator& a, const JoinWhereIterator& b) {
        return !(a == b); // NOLINT
    }
};

} // namespace internal
} // namespace lz
#endif // LZ_LEFT_JOIN_ITERATOR_HPP
