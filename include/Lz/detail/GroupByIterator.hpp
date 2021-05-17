#pragma once

#ifndef LZ_GROUP_BY_ITERATOR_HPP
#define LZ_GROUP_BY_ITERATOR_HPP

#include <algorithm>

#include "FunctionContainer.hpp"
#include "LzTools.hpp"
#include "BasicIteratorView.hpp"

namespace lz { namespace internal {
#ifdef LZ_HAS_EXECUTION
template<class Iterator, class Comparer, class Execution>
#else // ^^ LZ_HAS_EXECUTION vv !LZ_HAS_EXECUTION

template<class Iterator, class Comparer>
#endif  // end LZ_HAS_EXECUTION
class GroupByIterator {
	Iterator _subRangeEnd{};
	Iterator _subRangeBegin{};
	Iterator _end{};
	mutable FunctionContainer<Comparer> _comparer{};
#ifdef LZ_HAS_EXECUTION
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
		if constexpr (internal::checkForwardAndPolicies<Execution, Iterator>()) {
			_subRangeEnd = std::find_if(std::move(_subRangeEnd), _end, [this, &next](const IterValueType& v) {
				return !_comparer(v, next);
			});
		}
		else {
			_subRangeEnd = std::find_if(_execution, std::move(_subRangeEnd), _end, [this, &next](const IterValueType& v) {
				return !_comparer(v, next);
			});
		}
#else
		_subRangeEnd = std::find_if(std::move(_subRangeEnd), _end, [this, &next](const IterValueType& v) {
			return !_comparer(v, next);
		});
#endif
	}

public:
	using iterator_category =
	typename std::common_type<std::forward_iterator_tag, typename std::iterator_traits<Iterator>::iterator_category>::type;
	using value_type = std::pair<Decay<Ref>, BasicIteratorView<Iterator>>;
	using reference = std::pair<Ref, BasicIteratorView<Iterator>>;
	using pointer = FakePointerProxy<reference>;
	using difference_type = std::ptrdiff_t;

	constexpr GroupByIterator() = default;

#ifdef LZ_HAS_EXECUTION
	LZ_CONSTEXPR_CXX_20 GroupByIterator(Iterator begin, Iterator end, Comparer comparer, Execution execution) :
#else // ^^ LZ_HAS_EXECUTION vv !LZ_HAS_EXECUTION

	GroupByIterator(Iterator begin, Iterator end, Comparer comparer) :
#endif // end LZ_HAS_EXECUTION
		_subRangeEnd(begin),
		_subRangeBegin(begin),
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

	LZ_CONSTEXPR_CXX_17 reference operator*() const {
		return reference(*_subRangeBegin, BasicIteratorView<Iterator>(_subRangeBegin, _subRangeEnd));
	}

	LZ_CONSTEXPR_CXX_17 pointer operator->() const {
		return FakePointerProxy<decltype(**this)>(**this);
	}

	LZ_CONSTEXPR_CXX_20 GroupByIterator& operator++() {
		_subRangeBegin = _subRangeEnd;
		advance();
		return *this;
	}

	LZ_CONSTEXPR_CXX_20 GroupByIterator operator++(int) {
		GroupByIterator tmp(*this);
		++*this;
		return tmp;
	}

	LZ_CONSTEXPR_CXX_17 friend bool operator!=(const GroupByIterator& lhs, const GroupByIterator& rhs) {
		return lhs._subRangeBegin != rhs._subRangeBegin;
	}

	LZ_CONSTEXPR_CXX_17 friend bool operator==(const GroupByIterator& lhs, const GroupByIterator& rhs) {
		return !(lhs != rhs); // NOLINT
	}
};
}}
#endif // LZ_GROUP_BY_ITERATOR_HPP
