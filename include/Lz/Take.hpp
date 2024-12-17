#pragma once

#ifndef LZ_TAKE_HPP
#define LZ_TAKE_HPP

#include "detail/basic_iterable.hpp"
#include "detail/iterators/take.hpp"

namespace lz {

LZ_MODULE_EXPORT_SCOPE_BEGIN

template<class Iterator>
class take_iterable
    : public detail::basic_iterable<detail::take_iterator<Iterator>, typename detail::take_iterator<Iterator>::sentinel> {

public:
    using iterator = detail::take_iterator<Iterator>;
    using const_iterator = iterator;
    using value_type = typename iterator::value_type;

private:
    LZ_CONSTEXPR_CXX_20
    take_iterable(Iterator begin, typename iterator::difference_type n, std::forward_iterator_tag /*unused*/) :
        detail::basic_iterable<iterator, default_sentinel>(iterator(std::move(begin), n)) {
    }

    LZ_CONSTEXPR_CXX_20
    take_iterable(Iterator begin, typename iterator::difference_type n, std::random_access_iterator_tag /*unused*/) :
        detail::basic_iterable<iterator>(iterator(begin, n), iterator(begin + n, 0)) {
    }

public:
    LZ_CONSTEXPR_CXX_20 take_iterable(Iterator begin, typename iterator::difference_type n) :
        take_iterable(std::move(begin), n, iter_cat_t<Iterator>{}) {
    }

    constexpr take_iterable() = default;
};

// Start of group
/**
 * @defgroup ItFns Iterator free functions.
 * These are the iterator functions and can all be used to iterate over in a
 * `for (auto var : lz::someiterator(...))`-like fashion. Also, all objects contain a `to_vector`, `to<container>`, to_map,
 * to_unordered_map`. These functions are used to convert the iterator to a container.
 * @{
 */

/**
 * @brief Takes the first `n` elements from the iterable.
 * @param iterable The iterable to take elements from.
 * @param n The amount of elements to take.
 * @return A take_iterable object containing the first `n` elements.
 */
template<LZ_CONCEPT_ITERABLE Iterable>
LZ_NODISCARD LZ_CONSTEXPR_CXX_14 take_iterable<iter_t<Iterable>> take(Iterable&& iterable, const diff_iterable_t<Iterable> n) {
    return take_iterable<iter_t<Iterable>>(detail::begin(std::forward<Iterable>(iterable)), n);
}

// End of group
/**
 * @}
 */

LZ_MODULE_EXPORT_SCOPE_END

} // namespace lz

#endif