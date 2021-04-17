#pragma once

#ifndef LZ_EXCLUDE_HPP
#define LZ_EXCLUDE_HPP

#include "detail/BasicIteratorView.hpp"
#include "detail/ExcludeIterator.hpp"

namespace lz {
	template<class Iterator>
	class Exclude final : public internal::BasicIteratorView<internal::ExcludeIterator<Iterator>> {
	public:
		using iterator = internal::ExcludeIterator<Iterator>;
		using const_iterator = iterator;

		Exclude() = default;

		Exclude(Iterator begin, Iterator end, const internal::DiffType<Iterator> from,
				const internal::DiffType<Iterator> to):
			internal::BasicIteratorView<iterator>(iterator(begin, begin, end, from, to),
										 		  iterator(end, begin, end, from, to))
		{}
	};

	template<LZ_CONCEPT_ITERATOR Iterator>
	Exclude<Iterator> excludeRange(Iterator begin, Iterator end, const internal::DiffType<Iterator> from,
								   const internal::DiffType<Iterator> to) {
		LZ_ASSERT(std::distance(begin, end) - to >= 0, "cannot access elements after end or before begin");
		return Exclude<Iterator>(std::move(begin), std::move(end), from, to);
	}

	template<LZ_CONCEPT_ITERABLE Iterable, class Iterator = internal::IterTypeFromIterable<Iterable>>
	Exclude<Iterator> exclude(Iterable&& iterable, const internal::DiffType<Iterator> from, const internal::DiffType<Iterator> to) {
		return excludeRange(internal::begin(std::forward<Iterable>(iterable)), internal::end(std::forward<Iterable>(iterable)), from, to);
	}
}


#endif // LZ_SKIP_HPP
