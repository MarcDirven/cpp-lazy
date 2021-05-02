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
		mutable FunctionContainer<KeySelector> _keySelector{};
#ifdef LZ_HAS_EXECUTION
		Execution _execution{};
#endif // end LZ_HAS_EXECUTION

		using IterValueType = ValueType<Iterator>;
		using FnRetType = decltype(_keySelector(*_subRangeBegin));

		void advance() {
			if (_subRangeEnd == _end) {
				return;
			}
			FnRetType next = _keySelector(*_subRangeEnd);
			++_subRangeEnd;
#ifdef LZ_HAS_EXECUTION
			if constexpr (internal::checkForwardAndPolicies<Execution, Iterator>()) {
				_subRangeEnd = std::find_if(std::move(_subRangeEnd), _end, [this, &next](const IterValueType& v) {
					return !(_keySelector(v) == next); // NOLINT
				});
			}
			else {
				_subRangeEnd = std::find_if(_execution, std::move(_subRangeEnd), _end, [this, &next](const IterValueType& v) {
					return !(_keySelector(v) == next); // NOLINT
				});
			}
#else
			_subRangeEnd = std::find_if(std::move(_subRangeEnd), _end, [this, &next](const IterValueType& v) {
				return !(_keySelector(v) == next); // NOLINT
			});
#endif
		}

	public:
		using iterator_category = std::input_iterator_tag;
		using value_type = std::pair<Decay<FnRetType>, BasicIteratorView<Iterator>>;
		using reference = std::pair<FnRetType, BasicIteratorView<Iterator>>;
		using pointer = FakePointerProxy<reference>;
		using difference_type = std::ptrdiff_t;

		GroupByIterator() = default;

#ifdef LZ_HAS_EXECUTION
		GroupByIterator(Iterator begin, Iterator end, KeySelector keySelector, Execution execution):
#else // ^^ LZ_HAS_EXECUTION vv !LZ_HAS_EXECUTION
		GroupByIterator(Iterator begin, Iterator end, KeySelector keySelector):
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
