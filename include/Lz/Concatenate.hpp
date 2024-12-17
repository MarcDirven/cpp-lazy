#pragma once

#ifndef LZ_CONCATENATE_HPP
#define LZ_CONCATENATE_HPP

#include "detail/basic_iterable.hpp"
#include "detail/iterators/concatenate.hpp"

namespace lz {

LZ_MODULE_EXPORT_SCOPE_BEGIN

template<class IterTuple, class SentinelTuple>
class concatenate_iterable final
    : public detail::basic_iterable<detail::concatenate_iterator<IterTuple, SentinelTuple>,
                                    typename detail::concatenate_iterator<IterTuple, SentinelTuple>::sentinel> {

    LZ_CONSTEXPR_CXX_20 concatenate_iterable(IterTuple begin, SentinelTuple end, std::forward_iterator_tag /* unused */) :
        detail::basic_iterable<iterator, default_sentinel>(iterator(begin, begin, end)) {
    }

    LZ_CONSTEXPR_CXX_20 concatenate_iterable(IterTuple begin, SentinelTuple end, std::bidirectional_iterator_tag /* unused */) :
        detail::basic_iterable<iterator>(iterator(begin, begin, end), iterator(end, begin, end)) {
    }

public:
    using iterator = detail::concatenate_iterator<IterTuple, SentinelTuple>;
    using const_iterator = iterator;
    using value_type = typename iterator::value_type;

    LZ_CONSTEXPR_CXX_20 concatenate_iterable(IterTuple begin, SentinelTuple end) :
        concatenate_iterable(std::move(begin), std::move(end), iter_cat_t<iterator>{}) {
    }

    constexpr concatenate_iterable() = default;
};

/**
 * @addtogroup ItFns
 * @{
 */

/**
 * @brief Creates a concat view object from a tuple of beginnings and a tuple of endings. The size of the parameter pack must be
 * greater than or equal to 2. The underlying value types must be the same.
 * @details This view object, contains the iterators that 'glues'/'concatenates' two or more containers together.
 * @param iterables A parameter pack of containers/iterables.
 * @return A concatenate view object, which contains the iterator, that can be used to iterate over.
 */
template<LZ_CONCEPT_ITERABLE... Iterables>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 concatenate_iterable<std::tuple<iter_t<Iterables>...>, std::tuple<sentinel_t<Iterables>...>>
concat(Iterables&&... iterables) {
    constexpr auto tuple_size = sizeof...(Iterables);
    static_assert(tuple_size >= 2, "amount of iterators/containers cannot be less than or equal to 1");
    static_assert(detail::is_all_same<val_t<iter_t<Iterables>>...>::value, "all value types must be the same");
    return { std::make_tuple(detail::begin(std::forward<Iterables>(iterables))...),
             std::make_tuple(detail::end(std::forward<Iterables>(iterables))...) };
}

// End of group
/**
 * @}
 */

LZ_MODULE_EXPORT_SCOPE_END

} // namespace lz

#endif // LZ_CONCATENATE_HPP