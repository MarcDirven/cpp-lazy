#pragma once

#ifndef LZ_FILTER_ITERATOR_HPP
#define LZ_FILTER_ITERATOR_HPP

#include <algorithm>

#include "LzTools.hpp"
#include "FunctionContainer.hpp"

namespace lz { namespace internal {
#ifdef LZ_HAS_EXECUTION
template<LZ_CONCEPT_ITERATOR Iterator, class UnaryPredicate, class Execution>
#else // ^^^lz has execution vvv ! lz has execution

template<LZ_CONCEPT_ITERATOR Iterator, class UnaryPredicate>
#endif // LZ_HAS_EXECUTION
class FilterIterator {
	using IterTraits = std::iterator_traits<Iterator>;

public:
	using iterator_category = typename std::common_type<std::forward_iterator_tag, typename IterTraits::iterator_category>::type;
	using value_type = typename IterTraits::value_type;
	using difference_type = typename IterTraits::difference_type;
	using reference = typename IterTraits::reference;
	using pointer = FakePointerProxy<reference>;

	LZ_CONSTEXPR_CXX_20 void find() {
#ifdef LZ_HAS_EXECUTION
		if constexpr (internal::checkForwardAndPolicies<Execution, Iterator>()) { // prevent verbose errors when iter cat < forward
			_iterator = std::find_if(std::move(_iterator), _end, _predicate);
		}
		else {
			_iterator = std::find_if(_execution, std::move(_iterator), _end, _predicate);
		}
#else // ^^^lz has execution vvv ! lz has execution
		_iterator = std::find_if(std::move(_iterator), _end, _predicate);
#endif // LZ_HAS_EXECUTION
	}

private:
	Iterator _iterator{};
	Iterator _end{};
	mutable FunctionContainer<UnaryPredicate> _predicate{};
#ifdef LZ_HAS_EXECUTION
	Execution _execution{};
#endif // LZ_HAS_EXECUTION

public:
#ifdef LZ_HAS_EXECUTION
	LZ_CONSTEXPR_CXX_20 FilterIterator(Iterator begin, Iterator end, UnaryPredicate function, Execution execution)
#else // ^^^lz has execution vvv ! lz has execution

	FilterIterator(Iterator begin, Iterator end, UnaryPredicate function)
#endif // LZ_HAS_EXECUTION
		:
		_iterator(std::move(begin)),
		_end(std::move(end)),
		_predicate(std::move(function))
#ifdef LZ_HAS_EXECUTION
	,
	_execution(execution)
#endif // LZ_HAS_EXECUTION
	{
		find();
	}

	constexpr FilterIterator() = default;

	LZ_NODISCARD LZ_CONSTEXPR_CXX_17 reference operator*() const {
		return *_iterator;
	}

	LZ_NODISCARD LZ_CONSTEXPR_CXX_17 pointer operator->() const {
		return FakePointerProxy<decltype(**this)>(**this);
	}

	LZ_CONSTEXPR_CXX_20 FilterIterator& operator++() {
		++_iterator;
		find();
		return *this;
	}

	LZ_CONSTEXPR_CXX_20 FilterIterator operator++(int) {
		FilterIterator tmp(*this);
		++*this;
		return tmp;
	}

	LZ_NODISCARD LZ_CONSTEXPR_CXX_17 friend bool operator!=(const FilterIterator& a, const FilterIterator& b) {
		return a._iterator != b._iterator;
	}

	LZ_NODISCARD LZ_CONSTEXPR_CXX_17 friend bool operator==(const FilterIterator& a, const FilterIterator& b) {
		return !(a != b); // NOLINT
	}
};
}}

#endif