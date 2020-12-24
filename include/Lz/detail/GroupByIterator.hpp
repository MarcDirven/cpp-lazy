#pragma once

#ifndef LZ_GROUP_BY_ITERATOR_HPP
#define LZ_GROUP_BY_ITERATOR_HPP

#include <algorithm>

#include "FunctionContainer.hpp"
#include "LzTools.hpp"
#include "BasicIteratorView.hpp"


namespace lz { namespace internal {
#ifdef LZ_HAS_EXECUTION
	template<class Iterator, class KeySelector, class Execution>
#else // ^^ LZ_HAS_EXECUTION vv !LZ_HAS_EXECUTION
	template<class Iterator, class KeySelector>
#endif  // end LZ_HAS_EXECUTION
	class GroupByIterator {
		Iterator _subRangeEnd{};
		Iterator _subRangeBegin{};
		Iterator _end{};
		FunctionContainer<KeySelector> _keySelector{};
#ifdef LZ_HAS_EXECUTION
		Execution _execution{};
#endif // end LZ_HAS_EXECUTION

		using IterValueType = ValueType<Iterator>;
		using FnRetType = FunctionReturnType<KeySelector, RefType<Iterator>>;

		void advance() {
			if (_subRangeEnd == _end) {
				return;
			}

			FnRetType next = _keySelector(*_subRangeEnd);
#ifdef LZ_HAS_EXECUTION
			_subRangeEnd = std::find_if(_execution, ++_subRangeEnd, _end, [this, &next](const IterValueType& v) {
				return !(_keySelector(v) == next);
			});
#else
			_subRangeEnd = std::find_if(++_subRangeEnd, _end, [this, &next](const IterValueType& v) {
				return !(_keySelector(v) == next); // NOLINT
			});
#endif
		}

	public:
		using iterator_category = std::output_iterator_tag;
		using value_type = std::pair<FunctionReturnType<KeySelector, RefType<Iterator>>, BasicIteratorView<Iterator>>;
		using reference = value_type;
		using pointer = FakePointerProxy<value_type>;
		using difference_type = std::ptrdiff_t;

		GroupByIterator() = default;

#ifdef LZ_HAS_EXECUTION
		GroupByIterator(Iterator begin, Iterator end, KeySelector keySelector, Execution execution, const bool sort):
#else // ^^ LZ_HAS_EXECUTION vv !LZ_HAS_EXECUTION
		GroupByIterator(Iterator begin, Iterator end, KeySelector keySelector, const bool sort):
#endif // end LZ_HAS_EXECUTION
			_subRangeEnd(begin),
			_subRangeBegin(begin),
			_end(std::move(end)),
			_keySelector(std::move(keySelector))
#ifdef LZ_HAS_EXECUTION
			, _execution(execution)
#endif // end LZ_HAS_EXECUTION
		{
			if (_subRangeBegin == _end) {
				return;
			}

			if (!sort) {
				return;
			}

			auto sortFunc = [this](const IterValueType& a, const IterValueType& b) {
				return _keySelector(a) < _keySelector(b);
			};
#ifdef LZ_HAS_EXECUTION
			if constexpr (internal::checkForwardAndPolicies<Execution, Iterator>()) {
				if (!std::is_sorted(_execution, _subRangeBegin, _end, sortFunc)) {
					std::sort(_execution, _subRangeBegin, _end, sortFunc);
				}
			}
#else // ^^ LZ_HAS_EXECUTION vv !LZ_HAS_EXECUTION
			if (!std::is_sorted(_subRangeBegin, _end, sortFunc)) {
				std::sort(_subRangeBegin, _end, sortFunc);
			}
#endif // end LZ_HAS_EXECUTION
			advance();
		}

		reference operator*() const {
			return reference(_keySelector(*_subRangeBegin), BasicIteratorView<Iterator>(_subRangeBegin, _subRangeEnd));
		}

		pointer operator->() const {
			return FakePointerProxy<decltype(**this)>(**this);
		}

		GroupByIterator& operator++() {
			_subRangeBegin = _subRangeEnd;
			advance();
			return *this;
		}

		GroupByIterator operator++(int) {
			GroupByIterator tmp(*this);
			++*this;
			return tmp;
		}

		friend bool operator!=(const GroupByIterator& lhs, const GroupByIterator& rhs) {
			return lhs._subRangeBegin != rhs._subRangeBegin;
		}

		friend bool operator==(const GroupByIterator& lhs, const GroupByIterator& rhs) {
			return !(lhs != rhs); // NOLINT
		}
	};
}}
#endif // LZ_GROUP_BY_ITERATOR_HPP
