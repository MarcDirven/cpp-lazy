#ifndef LZ_FLATTEN_HPP
#define LZ_FLATTEN_HPP

#include "detail/BasicIteratorView.hpp"
#include "detail/iterators/FlattenIterator.hpp"

namespace lz {

LZ_MODULE_EXPORT_SCOPE_BEGIN

template<class Iterator, class S, int Dims>
class Flatten final : public detail::BasicIteratorView<detail::FlattenIterator<Iterator, S, Dims>> {
public:
    using iterator = detail::FlattenIterator<Iterator, S, Dims>;
    using const_iterator = iterator;
    using value_type = typename detail::FlattenIterator<Iterator, S, 0>::value_type;

private:
    using Base = detail::BasicIteratorView<iterator>;

public:
    constexpr Flatten() = default;

    LZ_CONSTEXPR_CXX_20 Flatten(Iterator begin, S end) : Base(iterator(begin, begin, end), iterator(end, begin, end)) {
    }
};

/**
 * @addtogroup ItFns
 * @{
 */

/**
 * This function returns a view object that flattens an n-dimensional array.
 * @param iterable The iterable to flatten.
 * @return A flatten view object, where its iterator is a forward iterator.
 */
template<LZ_CONCEPT_ITERABLE Iterable, int Dims = detail::CountDims<std::iterator_traits<IterT<Iterable>>>::value - 1>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 Flatten<IterT<Iterable>, SentinelT<Iterable>, Dims> flatten(Iterable&& iterable) {
    static_assert(std::is_default_constructible<IterT<Iterable>>::value, "underlying iterator needs to be default constructible");
    return Flatten<IterT<Iterable>, SentinelT<Iterable>, Dims>(detail::begin(std::forward<Iterable>(iterable)),
                                                               detail::end(std::forward<Iterable>(iterable)));
}

template<class, class = void>
struct Dimensions;

template<LZ_CONCEPT_ITERABLE Iterable>
struct Dimensions<Iterable, detail::EnableIf<!std::is_array<Iterable>::value>> : detail::CountDims<IterT<Iterable>> {};

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
