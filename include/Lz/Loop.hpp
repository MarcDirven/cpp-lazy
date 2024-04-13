#pragma once

#ifndef LZ_LOOP_HPP
#define LZ_LOOP_HPP

#include "detail/BasicIteratorView.hpp"
#include "detail/LoopIterator.hpp"

namespace lz {

LZ_MODULE_EXPORT_SCOPE_BEGIN

template<class Iterator>
class Loop final : public internal::BasicIteratorView<internal::LoopIterator<Iterator>> {
public:
    using iterator = internal::LoopIterator<Iterator>;
    using const_iterator = iterator;
    using value_type = typename iterator::value_type;

    Loop(Iterator begin, Iterator end) :
        internal::BasicIteratorView<iterator>(iterator(begin, begin, end), iterator(end, begin, end)) {
    }

    Loop() = default;
};

/**
 * @addtogroup ItFns
 * @{
 */

/**
 * Loops over [begin, end) indefinitely
 * 
 * @param begin Beginning of the sequence
 * @param end Ending of the sequence
 * @return A loop iterator object.
 */
template<LZ_CONCEPT_ITERATOR Iterator>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 Loop<Iterator> loopRange(Iterator begin, Iterator end) {
    LZ_ASSERT(begin != end, "View cannot be empty in order to loop over elements");
    return { std::move(begin), std::move(end) };
}
/**
 * Loops over iterable indefinitely
 * 
 * @param iterable The sequence to loop over.
 * @return A loop iterator object.
 */
template<LZ_CONCEPT_ITERABLE Iterable>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 Loop<internal::IterTypeFromIterable<Iterable>> loop(Iterable&& iterable) {
    return loopRange(internal::begin(std::forward<Iterable>(iterable)), internal::end(std::forward<Iterable>(iterable)));
}

// End of group
/**
 * @}
 */

LZ_MODULE_EXPORT_SCOPE_END

} // namespace lz

#endif // LZ_LOOP_HPP
