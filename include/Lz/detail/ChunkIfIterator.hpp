#pragma once

#ifndef LZ_CHUNK_IF_ITERATOR_HPP
#define LZ_CHUNK_IF_ITERATOR_HPP

#include "FunctionContainer.hpp"
#include "BasicIteratorView.hpp"

namespace lz { namespace internal {
#ifdef LZ_HAS_EXECUTION
template<class Iterator, class UnaryPredicate, class Execution>
#else // ^^ LZ_HAS_EXECUTION vv !LZ_HAS_EXECUTION

template<class Iterator, class UnaryPredicate>
#endif // LZ_HAS_EXECUTION
class ChunkIfIterator {
	Iterator _subRangeBegin{};
	Iterator _subRangeEnd{};
	Iterator _end{};
	mutable FunctionContainer<UnaryPredicate> _predicate{};
#ifdef LZ_HAS_EXECUTION
	Execution _execution{};
#endif // LZ_HAS_EXECUTION
	using IterTraits = std::iterator_traits<Iterator>;

public:
	using value_type = BasicIteratorView<Iterator>;
	using difference_type = typename IterTraits::difference_type;
	using iterator_category = typename std::common_type<std::forward_iterator_tag, typename IterTraits::iterator_category>::type;
	using reference = value_type;
	using pointer = FakePointerProxy<reference>;

private:
	LZ_CONSTEXPR_CXX_20 void findNext() {
#ifdef LZ_HAS_EXECUTION
		if constexpr (internal::checkForwardAndPolicies<Execution, Iterator>()) {
			_subRangeEnd = std::find_if(_execution, _subRangeBegin, _end, _predicate);
		}
		else {
			_subRangeEnd = std::find_if(_subRangeBegin, _end, _predicate);
		}
#else // ^^ LZ_HAS_EXECUTION vv !LZ_HAS_EXECUTION
		_subRangeEnd = std::find_if(_subRangeBegin, _end, _predicate);
#endif // LZ_HAS_EXECUTION
	}

public:
	constexpr ChunkIfIterator() = default;

#ifdef LZ_HAS_EXECUTION
	LZ_CONSTEXPR_CXX_20 ChunkIfIterator(Iterator begin, Iterator end, UnaryPredicate predicate, Execution execution) :
#else // ^^ LZ_HAS_EXECUTION vv !LZ_HAS_EXECUTION

	ChunkIfIterator(Iterator begin, Iterator end, UnaryPredicate predicate) :
#endif // LZ_HAS_EXECUTION
		_subRangeBegin(begin),
		_subRangeEnd(begin),
		_end(std::move(end)),
		_predicate(std::move(predicate))
#ifdef LZ_HAS_EXECUTION
	,
	_execution(execution)
#endif // LZ_HAS_EXECUTION
	{
		if (begin == _end) {
			return;
		}
		findNext();
	}

	LZ_NODISCARD LZ_CONSTEXPR_CXX_20 reference operator*() const {
		return reference(_subRangeBegin, _subRangeEnd);
	}

	LZ_NODISCARD LZ_CONSTEXPR_CXX_20 pointer operator->() const {
		return FakePointerProxy<decltype(**this)>(**this);
	}

	LZ_CONSTEXPR_CXX_20 ChunkIfIterator& operator++() {
		if (_subRangeEnd != _end) {
			_subRangeBegin = ++_subRangeEnd;
			findNext();
		}
		else {
			_subRangeBegin = _end;
		}

		return *this;
	}

	LZ_CONSTEXPR_CXX_20 ChunkIfIterator& operator++(int) {
		ChunkIfIterator tmp(*this);
		++*this;
		return tmp;
	}

	LZ_NODISCARD LZ_CONSTEXPR_CXX_20 friend bool operator==(const ChunkIfIterator& lhs, const ChunkIfIterator& rhs) {
		return lhs._subRangeBegin == rhs._subRangeBegin;
	}

	LZ_NODISCARD LZ_CONSTEXPR_CXX_20 friend bool operator!=(const ChunkIfIterator& lhs, const ChunkIfIterator& rhs) {
		return !(lhs == rhs); // NOLINT
	}
};
}}

#endif //LZ_CHUNK_IF_ITERATOR_HPP
