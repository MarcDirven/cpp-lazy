#pragma once

#ifndef LZ_LEFT_JOIN_ITERATOR_HPP
#define LZ_LEFT_JOIN_ITERATOR_HPP

#include "LzTools.hpp"
#include "FunctionContainer.hpp"


namespace lz { namespace internal {
	template<class IterA, class IterB, class SelectorA, class SelectorB, class ResultSelector>
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
		mutable FunctionContainer<SelectorA> _selectorA{};
		mutable FunctionContainer<SelectorB> _selectorB{};
		mutable FunctionContainer<ResultSelector> _resultSelector{};

		LZ_CONSTEXPR_CXX_20 void findNext() {
			while (_iterA != _endA) {
				auto&& toFind = _selectorA(*_iterA);
				_iterB = std::lower_bound(std::move(_iterB), _endB, toFind, [this](const ValueTypeB& b, const SelectorARetVal& val) {
					return _selectorB(b) < val;
				});
				if (_iterB != _endB && !(toFind < _selectorB(*_iterB))) { // NOLINT
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

		LZ_CONSTEXPR_CXX_20 JoinWhereIterator(IterA iterA, IterA endA, IterB iterB, IterB endB, SelectorA a, SelectorB b,
											  ResultSelector resultSelector) :
			_iterA(std::move(iterA)),
			_endA(std::move(endA)),
			_iterB(std::move(iterB)),
			_beginB(iterB == endB ? endB : iterB),
			_endB(std::move(endB)),
			_selectorA(std::move(a)),
			_selectorB(std::move(b)),
			_resultSelector(std::move(resultSelector)) {
			if (_iterA == _endA || _iterB == _endB) {
				return;
			}
			findNext();
		}

		constexpr JoinWhereIterator() = default;

		constexpr reference operator*() const {
			return _resultSelector(*_iterA, *_iterB);
		}

		constexpr pointer operator->() const {
			return FakePointerProxy<decltype(**this)>(**this);
		}

		LZ_CONSTEXPR_CXX_20 JoinWhereIterator& operator++() {
			++_iterB;
			findNext();
			return *this;
		}

		LZ_CONSTEXPR_CXX_20 JoinWhereIterator operator++(int) {
			JoinWhereIterator tmp(*this);
			++*this;
			return tmp;
		}

		constexpr friend bool operator==(const JoinWhereIterator& a, const JoinWhereIterator& b) {
			return a._iterA == b._iterA;
		}

		constexpr friend bool operator!=(const JoinWhereIterator& a, const JoinWhereIterator& b) {
			return !(a == b); // NOLINT
		}
	};

}}
#endif //LZ_LEFT_JOIN_ITERATOR_HPP
