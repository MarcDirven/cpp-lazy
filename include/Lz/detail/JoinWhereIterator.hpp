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
        using ValueTypeB = typename IterTraitsB::value_type;
        using RefTypeA = typename IterTraitsA::reference;

        using SelectorARetVal = Decay<FunctionReturnType<SelectorA, RefTypeA>>;

        IterA _iterA{};
        IterA _endA{};
		IterB _iterB{};
        IterB _beginB{};
        IterB _endB{};
		IterB _iterBFound{};
		mutable FunctionContainer<SelectorA> _selectorA{};
		mutable FunctionContainer<SelectorB> _selectorB{};
		mutable FunctionContainer<ResultSelector> _resultSelector{};
#ifdef LZ_HAS_EXECUTION
        Execution _execution{};
#endif

        void findNext() {
			while (_iterA != _endA) {
				auto&& toFind = _selectorA(*_iterA);

				_iterB = std::lower_bound(std::move(_iterB), _endB, toFind, [this](const ValueTypeB& b, const SelectorARetVal& val) {
					return _selectorB(b) < val;
				});

				if (_iterB != _endB && !(toFind < _selectorB(*_iterB))) { // NOLINT
					_iterBFound = _iterB;
					++_iterB;
					return;
				}
				++_iterA;
				_iterB = _beginB;
			}
        }

    public:
        using reference = decltype(_resultSelector(*_iterA, *_iterB));
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
            _iterA(std::move(iterA)),
            _endA(std::move(endA)),
            _iterB(std::move(iterB)),
			_beginB(iterB == endB ? endB : iterB),
            _endB(std::move(endB)),
            _iterBFound(_iterB == _endB ? _endB : _iterB),
            _selectorA(std::move(a)),
            _selectorB(std::move(b)),
            _resultSelector(std::move(resultSelector))
#ifdef LZ_HAS_EXECUTION
            , _execution(execution)
#endif // LZ_HAS_EXECUTION
            {
                if (_iterA == _endA || _iterB == _endB) {
                    return;
                }
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
#else
				if (!std::is_sorted(_iterB, _endB, comparer)) {
					std::sort(_iterB, _endB, comparer);
				}
#endif // LZ_HAS_EXECUTION
                findNext();
            }

        JoinWhereIterator() = default;

        reference operator*() const {
            return _resultSelector(*_iterA, *_iterBFound);
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
            return a._iterA == b._iterA;
        }

        friend bool operator!=(const JoinWhereIterator& a, const JoinWhereIterator& b) {
            return !(a == b); // NOLINT
        }
    };

}}
#endif //LZ_LEFT_JOIN_ITERATOR_HPP
