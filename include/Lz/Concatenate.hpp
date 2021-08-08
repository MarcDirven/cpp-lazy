#pragma once

#ifndef LZ_CONCATENATE_HPP
#define LZ_CONCATENATE_HPP

#include "detail/BasicIteratorView.hpp"
#include "detail/ConcatenateIterator.hpp"

namespace lz {
template<class... Iterators>
class Concatenate final : public internal::BasicIteratorView<internal::ConcatenateIterator<Iterators...>> {
public:
    using iterator = internal::ConcatenateIterator<Iterators...>;
    using const_iterator = iterator;
    using value_type = typename iterator::value_type;

    LZ_CONSTEXPR_CXX_20 Concatenate(std::tuple<Iterators...> begin, std::tuple<Iterators...> end) :
        internal::BasicIteratorView<iterator>(iterator(begin, begin, end), iterator(end, begin, end)) {
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
 * @return A concatenate view object, which contains the random access iterator, that can be used to iterate over.
 */
template<LZ_CONCEPT_ITERATOR... Iterators>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 Concatenate<Iterators...>
concatRange(std::tuple<Iterators...> begin, std::tuple<Iterators...> end) {
    static_assert(sizeof...(Iterators) >= 2, "amount of iterators/containers cannot be less than or equal to 1");
    static_assert(internal::IsAllSame<internal::ValueType<Iterators>...>::value, "value types of iterators do no match");
    return { std::move(begin), std::move(end) };
}

/**
 * @brief Creates a concat view object from a tuple of beginnings and a tuple of endings. The size of the parameter pack must be
 * greater than or equal to 2. The underlying value types must be the same.
 * @details This view object, contains the iterators that 'glues'/'concatenates' two or more containers together.
 * @param iterables A parameter pack of containers/iterables.
 * @return A concatenate view object, which contains the random access iterator, that can be used to iterate over.
 */
template<LZ_CONCEPT_ITERABLE... Iterables>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 Concatenate<internal::IterTypeFromIterable<Iterables>...> concat(Iterables&&... iterables) {
    return concatRange(std::make_tuple(internal::begin(std::forward<Iterables>(iterables))...),
                       std::make_tuple(internal::end(std::forward<Iterables>(iterables))...));
}

// End of group
/**
 * @}
 */
} // namespace lz

#endif // LZ_CONCATENATE_HPP