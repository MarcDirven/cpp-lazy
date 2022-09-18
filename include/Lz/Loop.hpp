#pragma once

#ifndef LZ_LOOP_HPP
#    define LZ_LOOP_HPP

#    include "detail/BasicIteratorView.hpp"
#    include "detail/LoopIterator.hpp"

namespace lz {
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

template<LZ_CONCEPT_ITERATOR Iterator>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 Loop<Iterator> loopRange(Iterator begin, Iterator end) {
    LZ_ASSERT(begin != end, "View cannot be empty in order to loop over elements");
    return { std::move(begin), std::move(end) };
}

template<LZ_CONCEPT_ITERABLE Iterable>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 Loop<internal::IterTypeFromIterable<Iterable>> loop(Iterable&& iterable) {
    return loopRange(internal::begin(std::forward<Iterable>(iterable)), internal::end(std::forward<Iterable>(iterable)));
}

} // namespace lz

#endif // LZ_LOOP_HPP
