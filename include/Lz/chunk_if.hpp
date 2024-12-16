#pragma once

#ifndef LZ_CHUNK_IF_HPP
#define LZ_CHUNK_IF_HPP

#include "detail/iterators/chunk_if.hpp"

namespace lz {

LZ_MODULE_EXPORT_SCOPE_BEGIN

template<class Iterator, class S, class UnaryPredicate>
class chunk_if_iterable final
    : public detail::basic_iterable<detail::chunk_if_iterator<Iterator, S, UnaryPredicate>, default_sentinel> {
public:
    using iterator = detail::chunk_if_iterator<Iterator, S, UnaryPredicate>;
    using const_iterator = iterator;
    using value_type = typename iterator::value_type;

    constexpr chunk_if_iterable() = default;

    chunk_if_iterable(Iterator begin, S end, UnaryPredicate predicate) :
        detail::basic_iterable<iterator, default_sentinel>(iterator(std::move(begin), std::move(end), std::move(predicate))) {
    }
};

/**
 * @addtogroup ItFns
 * @{
 */

/**
 * Chops the sequence into pieces of iterators, when `unary_predicate` function returns true.
 * @param iterable The iterable to chop up.
 * @param unary_predicate The predicate that must return a boolean, to indicate whether or not to make a new chunk.
 * @return A chunk if iterator view object.
 */
template<LZ_CONCEPT_ITERABLE Iterable, class UnaryPredicate>
chunk_if_iterable<iter<Iterable>, sentinel<Iterable>, UnaryPredicate>
chunk_if(Iterable&& iterable, UnaryPredicate unary_predicate) {
    return { detail::begin(std::forward<Iterable>(iterable)), detail::end(std::forward<Iterable>(iterable)),
             std::move(unary_predicate) };
}

// End of group
/**
 * @}
 */

LZ_MODULE_EXPORT_SCOPE_END

} // namespace lz

#endif // LZ_CHUNK_IF_HPP
