#pragma once

#ifndef LZ_LEFT_JOIN_ITERATOR_HPP
#define LZ_LEFT_JOIN_ITERATOR_HPP

#include "LzTools.hpp"
#include "FunctionContainer.hpp"


namespace lz { namespace internal {
#ifdef LZ_HAS_EXECUTION
    template<class IterA, class IterB, class SelectorA, class SelectorB, class ResultSelector, class Execution>
#else
    template<class IterA, class IterB, class SelectorA, class SelectorB, class ResultSelector>
#endif
    class JoinWhereIterator {
    private:
        using IterTraitsA = std::iterator_traits<IterA>;
        using IterTraitsB = std::iterator_traits<IterB>;
        using ValueTypeA = typename IterTraitsA::value_type;
        using ValueTypeB = typename IterTraitsB::value_type;
        using RefTypeA = typename IterTraitsA::reference;
        using RefTypeB = typename IterTraitsB::reference;

        using SelectorARetVal = Decay<FunctionReturnType<SelectorA, RefTypeA>>;
        using SelectorBRetVal = Decay<FunctionReturnType<SelectorB, RefTypeB>>;

        enum class Longest {
            IteratorA,
            IteratorB
        } _longest{};

        IterA _iterA{};
        IterA _endA{};
        IterB _iterB{};
        IterB _endB{};
        IterA _iterAFound{};
        IterB _iterBFound{};
		mutable FunctionContainer<SelectorA> _selectorA{};
		mutable FunctionContainer<SelectorB> _selectorB{};
		mutable FunctionContainer<ResultSelector> _resultSelector{};
#ifdef LZ_HAS_EXECUTION
        Execution _execution{};
#endif

        void findNext() {
            if (_longest == Longest::IteratorA) {
            	for (; _iterA != _endA; ++_iterA) {
                    auto&& toFind = _selectorA(*_iterA);
                    const auto pos = std::lower_bound(_iterB, _endB, toFind, [this](const ValueTypeB& b, const SelectorARetVal& val) {
                        return _selectorB(b) < val;
                    });

                    if (pos != _endB && !(toFind < _selectorB(*pos))) { // NOLINT
                        _iterBFound = pos;
                        _iterAFound = _iterA; // Keep track of where we were
                        ++_iterA; // Increment the iterator by one to prevent never ending loop
                        return;
                    }
                }
                _iterAFound = _endA; // Necessary for operator==
            }
            else {
            	for (; _iterB != _endB; ++_iterB) {
                    auto&& toFind = _selectorB(*_iterB);
                    const auto pos = std::lower_bound(_iterA, _endA, toFind, [this](const ValueTypeA& a, const SelectorBRetVal& val) {
                        return _selectorA(a) < val;
                    });

                    if (pos != _endA && !(toFind < _selectorA(*pos))) { // NOLINT
                        _iterAFound = pos;
                        _iterBFound = _iterB; // Keep track of where we were
                        ++_iterB; // Increment the iterator by one to prevent never ending loop
                        return;
                    }
                }
                _iterBFound = _endB; // Necessary for operator==
            }
        }

    public:
        using reference = FunctionReturnType<ResultSelector, RefTypeA, RefTypeB>;
        using value_type = Decay<reference>;
        using iterator_category = std::forward_iterator_tag;
        using difference_type = std::ptrdiff_t;
        using pointer = FakePointerProxy<reference>;

#ifdef LZ_HAS_EXECUTION
        JoinWhereIterator(IterA iterA, IterA endA, IterB iterB, IterB endB, SelectorA a, SelectorB b, ResultSelector resultSelector,
                         Execution execution) :
#else
        JoinWhereIterator(IterA iterA, IterA endA, IterB iterB, IterB endB, SelectorA a, SelectorB b, ResultSelector resultSelector) :
#endif
            _longest(std::distance(iterA, endA) >= std::distance(iterB, endB) ? Longest::IteratorA : Longest::IteratorB),
            _iterA(std::move(iterA)),
            _endA(std::move(endA)),
            _iterB(std::move(iterB)),
            _endB(std::move(endB)),
            _iterAFound(_iterA == _endA ? _endA : _iterA),
            _iterBFound(_iterB == _endB ? _endB : _iterB),
            _selectorA(std::move(a)),
            _selectorB(std::move(b)),
            _resultSelector(std::move(resultSelector))
#ifdef LZ_HAS_EXECUTION
            , _execution(execution)
#endif // end has cxx 17
            {
                if (_iterA == _endA || _iterB == _endB) {
                    return;
                }

                if (_longest == Longest::IteratorA) {
                    const auto comparer = [this](const ValueTypeB& b1, const ValueTypeB& b2) { return _selectorB(b1) < _selectorB(b2); };

#ifdef LZ_HAS_EXECUTION
                    if constexpr (internal::checkForwardAndPolicies<Execution, IterB>()) {
                        if (!std::is_sorted(_iterB, _endB, comparer)) {
                            std::sort(_iterB, _endB, comparer);
                        }
                    }
                    else {
                        if (!std::is_sorted(_execution, _iterB, _endB, comparer)) {
                            std::sort(_execution, _iterB, _endB, comparer);
                        }
                    }
#else // ^^^has cxx 17 vvv ! has cxx 17
                    if (!std::is_sorted(_iterB, _endB, comparer)) {
                        std::sort(_iterB, _endB, comparer);
                    }
#endif // end has cxx 17
                }
                else if (_longest == Longest::IteratorB) {
                    const auto comparer = [this](const ValueTypeA& a1, const ValueTypeA& a2) { return _selectorA(a1) < _selectorA(a2); };

#ifdef LZ_HAS_EXECUTION
                    if constexpr (internal::checkForwardAndPolicies<Execution, IterA>()) {
                        if (!std::is_sorted(_iterA, _endA, comparer)) {
                            std::sort(_iterA, _endA, comparer);
                        }
                    }
                    else {
                        if (!std::is_sorted(_execution, _iterA, _endA, comparer)) {
                            std::sort(_execution, _iterA, _endA, comparer);
                        }
                    }
#else // ^^^has cxx 17 vvv ! has cxx 17
                    if (!std::is_sorted(_iterA, _endA, comparer)) {
                        std::sort(_iterA, _endA, comparer);
                    }
#endif // end has cxx 17
                }

                findNext();
            }

        JoinWhereIterator() = default;

        reference operator*() const {
            return _resultSelector(*_iterAFound, *_iterBFound);
        }

        pointer operator->() const {
            return FakePointerProxy<decltype(**this)>(**this);
        }

        JoinWhereIterator& operator++() {
            findNext();
            return *this;
        }

        JoinWhereIterator operator++(int) {
            JoinWhereIterator tmp(*this);
            ++*this;
            return tmp;
        }

        friend bool operator==(const JoinWhereIterator& a, const JoinWhereIterator& b) {
            return a._iterAFound == b._iterA || a._iterBFound == b._iterB;
        }

        friend bool operator!=(const JoinWhereIterator& a, const JoinWhereIterator& b) {
            return !(a == b); // NOLINT
        }
    };

}}
#endif //LZ_LEFT_JOIN_ITERATOR_HPP
