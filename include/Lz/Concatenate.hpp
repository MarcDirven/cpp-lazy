#pragma once

#ifndef LZ_CONCATENATE_HPP
#define LZ_CONCATENATE_HPP

#include "detail/BasicIteratorView.hpp"
#include "detail/iterators/ConcatenateIterator.hpp"

namespace lz {

LZ_MODULE_EXPORT_SCOPE_BEGIN

template<class... Iterators>
class Concatenate final : public detail::BasicIteratorView<detail::ConcatenateIterator<Iterators...>> {
public:
    using iterator = detail::ConcatenateIterator<Iterators...>;
    using const_iterator = iterator;
    using value_type = typename iterator::value_type;

    LZ_CONSTEXPR_CXX_20 Concatenate(std::tuple<Iterators...> begin, std::tuple<Iterators...> end) :
        detail::BasicIteratorView<iterator>(iterator(begin, begin, end), iterator(end, begin, end)) {
    }

    constexpr Concatenate() = default;
};

/**
 * @addtogroup ItFns
 * @{
 */

/**
 * @brief Creates a concat view object from a tuple of beginnings and a tuple of endings. The size of the tuple must be greater
 * than greater than or equal to 2. The underlying value types must be the same.
 * @details This view object, contains the iterators that 'glues'/'concatenates' two or more containers together.
 * @param begin A tuple of iterators pointing to the beginning.
 * @param end A tuple of iterators pointing to the ending.
 * @return A concatenate view object, which contains the iterator, that can be used to iterate over.
 */
template<LZ_CONCEPT_ITERATOR... Iterators>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 Concatenate<Iterators...>
concatRange(std::tuple<Iterators...> begin, std::tuple<Iterators...> end) {
    static_assert(sizeof...(Iterators) >= 2, "amount of iterators/containers cannot be less than or equal to 1");
    static_assert(detail::IsAllSame<detail::ValueType<Iterators>...>::value, "value types of iterators do no match");
    return { std::move(begin), std::move(end) };
}

/**
 * @brief Creates a concat view object from a tuple of beginnings and a tuple of endings. The size of the parameter pack must be
 * greater than or equal to 2. The underlying value types must be the same.
 * @details This view object, contains the iterators that 'glues'/'concatenates' two or more containers together.
 * @param iterables A parameter pack of containers/iterables.
 * @return A concatenate view object, which contains the iterator, that can be used to iterate over.
 */
template<LZ_CONCEPT_ITERABLE... Iterables>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 Concatenate<detail::IterTypeFromIterable<Iterables>...> concat(Iterables&&... iterables) {
    return concatRange(std::make_tuple(detail::begin(std::forward<Iterables>(iterables))...),
                       std::make_tuple(detail::end(std::forward<Iterables>(iterables))...));
}

// End of group
/**
 * @}
 */

LZ_MODULE_EXPORT_SCOPE_END

} // namespace lz

#endif // LZ_CONCATENATE_HPP