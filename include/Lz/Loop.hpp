#pragma once

#ifndef LZ_LOOP_HPP
#define LZ_LOOP_HPP

#include "detail/BasicIterable.hpp"
#include "detail/iterators/LoopIterator.hpp"

namespace lz {

LZ_MODULE_EXPORT_SCOPE_BEGIN

template<class Iterator, class S>
    class Loop final : public detail::basic_iterable < detail::loop_iterator<Iterator, S>,
    typename detail::loop_iterator<Iterator, S::sentinel> {

    Loop(Iterator begin, S end, std::forward_iterator_tag) :
        detail::basic_iterable<iterator, default_sentinel>(iterator(std::move(begin), std::move(begin), std::move(end))) {
    }

    Loop(Iterator begin, S end, std::bidirectional_iterator_tag) :
        detail::basic_iterable<iterator>(iterator(begin, begin, end), iterator(end, begin, end)) {
    }

public:
    using iterator = detail::loop_iterator<Iterator, S>;
    using const_iterator = iterator;
    using value_type = typename iterator::value_type;

    Loop(Iterator begin, S end) : Loop(std::move(begin), std::move(end), iter_cat<Iterator>{}) {
    }

    Loop() = default;
};

/**
 * @addtogroup ItFns
 * @{
 */

/**
 * Loops over iterable indefinitely
 * 
 * @param iterable The sequence to loop over.
 * @return A loop iterator object.
 */
template<LZ_CONCEPT_ITERABLE Iterable>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 Loop<iter<Iterable>, sentinel < Iterable >> loop(Iterable&& iterable) {
    return { detail::begin(std::forward<Iterable>(iterable)), detail::end(std::forward<Iterable>(iterable)) };
}

// End of group
/**
 * @}
 */

LZ_MODULE_EXPORT_SCOPE_END

} // namespace lz

#endif // LZ_LOOP_HPP
