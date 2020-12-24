#pragma once

#ifndef LZ_GROUP_BY_HPP
#define LZ_GROUP_BY_HPP

#include "detail/GroupByIterator.hpp"

namespace lz {

#ifdef LZ_HAS_EXECUTION
	template<LZ_CONCEPT_ITERATOR Iterator, class KeySelector, class Execution>
	class GroupBy : public internal::BasicIteratorView<internal::GroupByIterator<Iterator, KeySelector, Execution>> {
		using iterator = internal::GroupByIterator<Iterator, KeySelector, Execution>;
#else // ^^ LZ_HAS_EXECUTION vv !LZ_HAS_EXECUTION
	template<class Iterator, class KeySelector>
	class GroupBy final : public internal::BasicIteratorView<internal::GroupByIterator<Iterator, KeySelector>> {
		using iterator = internal::GroupByIterator<Iterator, KeySelector>;
#endif
		using const_iterator = iterator;

	public:
#ifdef LZ_HAS_EXECUTION
		GroupBy(Iterator begin, Iterator end, KeySelector keySelector, Execution execution, const bool sort):
			internal::BasicIteratorView<iterator>(iterator(begin, end, keySelector, execution, sort),
										 		  iterator(end, end, keySelector, execution, sort))
#else // ^^ LZ_HAS_EXECUTION vv !LZ_HAS_EXECUTION
		GroupBy(Iterator begin, Iterator end, KeySelector keySelector, const bool sort):
			internal::BasicIteratorView<iterator>(iterator(begin, end, keySelector, sort), iterator(end, end, keySelector, sort))
#endif // LZ_HAS_EXECUTION
		{}

		GroupBy() = default;
	};

#ifdef LZ_HAS_EXECUTION
	template<LZ_CONCEPT_ITERATOR Iterator, class KeySelector, class Execution = std::execution::sequenced_policy>
	GroupBy<Iterator, KeySelector, Execution>
	groupByRange(Iterator begin, Iterator end, KeySelector keySelector, Execution execution = std::execution::seq,
			     const bool sort = true) {
		return GroupBy<Iterator, KeySelector, Execution>(std::move(begin), std::move(end), std::move(keySelector), execution, sort);
	}

	template<LZ_CONCEPT_ITERABLE Iterable, class KeySelector, class Execution = std::execution::sequenced_policy>
	GroupBy<internal::IterTypeFromIterable<Iterable>, KeySelector, Execution>
	groupBy(Iterable&& iterable, KeySelector keySelector, Execution execution = std::execution::seq, const bool sort = true) {
		return groupByRange(internal::begin(std::forward<Iterable>(iterable)), internal::end(std::forward<Iterable>(iterable)),
					  		std::move(keySelector), execution, sort);
	}
#else // ^^ LZ_HAS_EXECUTION vv !LZ_HAS_EXECUTION
	template<class Iterator, class KeySelector>
	GroupBy<Iterator, KeySelector> groupByRange(Iterator begin, Iterator end, KeySelector keySelector, const bool sort = true) {
		return GroupBy<Iterator, KeySelector>(std::move(begin), std::move(end), std::move(keySelector), sort);
	}

	template<class Iterable, class KeySelector>
	GroupBy<internal::IterTypeFromIterable<Iterable>, KeySelector>
	groupBy(Iterable&& iterable, KeySelector keySelector, const bool sort = true) {
		return groupByRange(internal::begin(std::forward<Iterable>(iterable)), internal::end(std::forward<Iterable>(iterable)),
					  		std::move(keySelector), sort);
	}
#endif // end LZ_HAS_EXECUTION
}

#endif // LZ_GROUP_BY_HPP
