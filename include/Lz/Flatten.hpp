#ifndef LZ_FLATTEN_HPP
#define LZ_FLATTEN_HPP

#include "detail/basic_iterable.hpp"
#include "detail/iterators/flatten.hpp"

namespace lz {

namespace detail {
template<class T>
using remove_cvref = typename std::remove_cvref<T>::type;

template<class T>
using is_c_array = std::is_array<remove_cvref<T>>;
} // namespace detail

LZ_MODULE_EXPORT_SCOPE_BEGIN

template<class Iterator, class S, int Dims>
class flatten_iterable final : public detail::basic_iterable<detail::flatten_iterator<Iterator, S, Dims>,
                                                             typename detail::flatten_iterator<Iterator, S, Dims>::sentinel> {
public:
    using iterator = detail::flatten_iterator<Iterator, S, Dims>;
    using const_iterator = iterator;
    using value_type = typename detail::flatten_iterator<Iterator, S, 0>::value_type;

private:
    using Base = detail::basic_iterable<iterator, typename iterator::sentinel>;

    LZ_CONSTEXPR_CXX_20 flatten_iterable(Iterator begin, S end, std::forward_iterator_tag) : Base(iterator(begin, begin, end)) {
    }

    LZ_CONSTEXPR_CXX_20 flatten_iterable(Iterator begin, S end, std::bidirectional_iterator_tag) :
        Base(iterator(begin, begin, end), iterator(end, begin, end)) {
    }

public:
    constexpr flatten_iterable() = default;

    LZ_CONSTEXPR_CXX_20 flatten_iterable(Iterator begin, S end) :
        flatten_iterable(std::move(begin), std::move(end), iter_cat_t<iterator>{}) {
    }
};

/**
 * @addtogroup ItFns
 * @{
 */

template<class, class = void>
struct dimensions;

template<LZ_CONCEPT_ITERABLE Iterable>
struct dimensions<Iterable, detail::enable_if<!detail::is_c_array<Iterable>::value>> : detail::count_dims<iter_t<Iterable>> {};

template<LZ_CONCEPT_ITERABLE Iterable>
struct dimensions<Iterable, detail::enable_if<detail::is_c_array<Iterable>::value>>
    : std::integral_constant<std::size_t, std::rank<detail::remove_cvref<Iterable>>::value> {};

#ifdef LZ_HAS_CXX_17
template<LZ_CONCEPT_ITERABLE Iterable>
inline constexpr std::size_t dimensions_v = dimensions<Iterable>::value;
#endif

/**
 * This function returns a view object that flattens an n-dimensional array.
 * @param iterable The iterable to flatten.
 * @return A flatten view object, where its iterator is a forward iterator.
 */
template<LZ_CONCEPT_ITERABLE Iterable>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20
// clang-format off
flatten_iterable<iter_t<Iterable>, sentinel_t<Iterable>, dimensions<Iterable>::value - !detail::is_c_array<Iterable>::value>
    flatten(Iterable&& iterable) {
    static constexpr auto dims = dimensions<Iterable>::value - !detail::is_c_array<Iterable>::value;
    static_assert(std::is_default_constructible<iter_t<Iterable>>::value,
                  "underlying iterator needs to be default constructible");
    return flatten_iterable<iter_t<Iterable>, sentinel_t<Iterable>, dims>(detail::begin(std::forward<Iterable>(iterable)),
                                                                          detail::end(std::forward<Iterable>(iterable)));
}

// clang-format on

// End of group
/**
 * @}
 */

LZ_MODULE_EXPORT_SCOPE_END

} // namespace lz

#endif // LZ_FLATTEN_HPP
