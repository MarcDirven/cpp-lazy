#ifndef LZ_FLATTEN_HPP
#define LZ_FLATTEN_HPP

#include "detail/BasicIteratorView.hpp"
#include "detail/iterators/FlattenIterator.hpp"

namespace lz {

LZ_MODULE_EXPORT_SCOPE_BEGIN

template<class Iterator, int Dims>
class Flatten final : public detail::BasicIteratorView<detail::FlattenIterator<Iterator, Dims>> {
public:
    using iterator = detail::FlattenIterator<Iterator, Dims>;
    using const_iterator = iterator;
    using value_type = typename detail::FlattenIterator<Iterator, 0>::value_type;

private:
    using Base = detail::BasicIteratorView<iterator>;

public:
    constexpr Flatten() = default;

    LZ_CONSTEXPR_CXX_20 Flatten(Iterator begin, Iterator end) : Base(iterator(begin, begin, end), iterator(end, begin, end)) {
    }
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
template<LZ_CONCEPT_ITERATOR Iterator, int Dims = detail::CountDims<std::iterator_traits<Iterator>>::value - 1>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 Flatten<Iterator, Dims> flattenRange(Iterator begin, Iterator end) {
    static_assert(std::is_default_constructible<Iterator>::value, "underlying iterator needs to be default constructible");
    return { std::move(begin), std::move(end) };
}

/**
 * This function returns a view object that flattens an n-dimensional array.
 * @param iterable The iterable to flatten.
 * @return A flatten view object, where its iterator is a forward iterator.
 */
template<LZ_CONCEPT_ITERABLE Iterable, class Iterator = detail::IterTypeFromIterable<Iterable>,
         int Dims = detail::CountDims<std::iterator_traits<Iterator>>::value - 1>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 Flatten<Iterator, Dims> flatten(Iterable&& iterable) {
    return flattenRange<Iterator, Dims>(detail::begin(std::forward<Iterable>(iterable)),
                                        detail::end(std::forward<Iterable>(iterable)));
}

template<class, class = void>
struct Dimensions;

template<LZ_CONCEPT_ITERABLE Iterable>
struct Dimensions<Iterable, detail::EnableIf<!std::is_array<Iterable>::value>>
    : detail::CountDims<detail::IterTypeFromIterable<Iterable>> {};

template<LZ_CONCEPT_ITERABLE Iterable>
struct Dimensions<Iterable, detail::EnableIf<std::is_array<Iterable>::value>>
    : std::integral_constant<int, static_cast<int>(std::rank<Iterable>::value)> {};

#ifdef LZ_HAS_CXX_17
template<LZ_CONCEPT_ITERABLE Iterable>
inline constexpr int DimensionsV = Dimensions<Iterable>::value;
#endif

// End of group
/**
 * @}
 */

LZ_MODULE_EXPORT_SCOPE_END

} // namespace lz

#endif // LZ_FLATTEN_HPP
