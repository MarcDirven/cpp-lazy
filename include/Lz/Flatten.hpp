#ifndef LZ_FLATTEN_HPP
#define LZ_FLATTEN_HPP

#include "detail/FlattenIterator.hpp"
#include "detail/BasicIteratorView.hpp"


namespace lz {
template<class Iterator, int Dims>
class Flatten final : public internal::BasicIteratorView<internal::FlattenIterator<Iterator, Dims>> {
public:
	using iterator = internal::FlattenIterator<Iterator, Dims>;
	using const_iterator = iterator;
	using value_type = typename internal::FlattenIterator<Iterator, 0>::value_type;

private:
	using Base = internal::BasicIteratorView<iterator>;

public:
	constexpr Flatten() = default;

	constexpr Flatten(Iterator begin, Iterator end) :
		Base(iterator(begin, begin, end), iterator(end, begin, end)) {}
};

/**
 * @addtogroup ItFns
 * @{
 */

/**
 * This function returns a view object that flattens an n-dimensional array.
 * @param begin The beginning of the iterable.
 * @param end The ending of the iterable.
 * @return A flatten view object, where its iterator is a forward iterator.
 */
template<LZ_CONCEPT_ITERATOR Iterator, int Dims = internal::CountDims<std::iterator_traits<Iterator>>::value - 1>
constexpr Flatten<Iterator, Dims> flatten(Iterator begin, Iterator end) {
	static_assert(std::is_default_constructible<Iterator>::value, "underlying iterator needs to be default constructible");
	return Flatten<Iterator, Dims>(std::move(begin), std::move(end));
}

/**
 * This function returns a view object that flattens an n-dimensional array.
 * @param iterable The iterable to flatten.
 * @return A flatten view object, where its iterator is a forward iterator.
 */
template<LZ_CONCEPT_ITERABLE Iterable,
	class Iterator = internal::IterTypeFromIterable<Iterable>,
	int Dims = internal::CountDims<std::iterator_traits<Iterator>>::value - 1>
constexpr Flatten<Iterator, Dims> flatten(Iterable&& iterable) {
	return flatten<Iterator, Dims>(internal::begin(std::forward<Iterable>(iterable)), internal::end(std::forward<Iterable>(iterable)));
}

template<class, class = void>
struct Dimensions;

template<LZ_CONCEPT_ITERABLE Iterable>
struct Dimensions<Iterable, internal::EnableIf<!std::is_array<Iterable>::value>> {
	static constexpr int value = internal::CountDims<internal::IterTypeFromIterable<Iterable>>::value;
};

template<LZ_CONCEPT_ITERABLE Iterable>
struct Dimensions<Iterable, internal::EnableIf<std::is_array<Iterable>::value>> {
	static constexpr int value = static_cast<int>(std::rank<Iterable>::value);
};

#ifdef LZ_HAS_CXX_17
template<LZ_CONCEPT_ITERABLE Iterable>
inline constexpr int DimensionsV = Dimensions<Iterable>::value;
#endif


// End of group
/**
 * @}
 */
} // lz

#endif // LZ_FLATTEN_HPP
