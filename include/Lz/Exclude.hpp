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

	constexpr Exclude() = default;

	LZ_CONSTEXPR_CXX_17 Exclude(Iterator begin, Iterator end, const internal::DiffType<Iterator> from,
								const internal::DiffType<Iterator> to) :
		internal::BasicIteratorView<iterator>(iterator(begin, begin, end, from, to), iterator(end, begin, end, from, to)) {}
};

/**
 * @addtogroup ItFns
 * @{
 */

/**
 * Excludes a range within [begin, end) where index is [from, to).
 * @param begin The beginning of the sequence.
 * @param end The ending of the sequence.
 * @param from Index to start excluding elements from.
 * @param to Index to stop at, note that its underlying element at this index is included.
 * @return An Exclude iterator view object.
 */
template<LZ_CONCEPT_ITERATOR Iterator>
LZ_CONSTEXPR_CXX_17 Exclude<Iterator> excludeRange(Iterator begin, Iterator end, const internal::DiffType<Iterator> from,
												   const internal::DiffType<Iterator> to) {
	using lz::distance; using std::distance;
	LZ_ASSERT(distance(begin, end) - to >= 0, "cannot access elements after end or before begin");
	return Exclude<Iterator>(std::move(begin), std::move(end), from, to);
}

/**
 * Excludes a range within `iterable` where index is [from, to).
 * @param iterable The iterable sequence.
 * @param from Index to start excluding elements from.
 * @param to Index to stop at, note that its underlying element at this index is included.
 * @return An Exclude iterator view object.
 */
template<LZ_CONCEPT_ITERABLE Iterable, class Iterator = internal::IterTypeFromIterable<Iterable>>
LZ_CONSTEXPR_CXX_17 Exclude<Iterator> exclude(Iterable&& iterable, const internal::DiffType<Iterator> from,
											  const internal::DiffType<Iterator> to) {
	return excludeRange(internal::begin(std::forward<Iterable>(iterable)), internal::end(std::forward<Iterable>(iterable)), from, to);
}

// End of group
/**
 * @}
 */
} // lz


#endif // LZ_SKIP_HPP
