#pragma once

#ifndef LZ_COMMON_HPP
#define LZ_COMMON_HPP

#include "Lz/detail/BasicIteratorView.hpp"
#include "Lz/detail/CompilerChecks.hpp"
#include "Lz/detail/iterators/CommonIterator.hpp"

namespace lz {

LZ_MODULE_EXPORT_SCOPE_BEGIN

template<class Iterator, class S>
class CommonView : public detail::BasicIteratorView<detail::CommonIterator<Iterator, S>> {
    using Base = detail::BasicIteratorView<detail::CommonIterator<Iterator, S>>;

public:
    using iterator = detail::CommonIterator<Iterator, S>;
    using const_iterator = iterator;
    using value_type = typename iterator::value_type;
    using reference = typename iterator::reference;
    using pointer = typename iterator::pointer;
    using difference_type = typename iterator::difference_type;

    CommonView() = default;

    LZ_CONSTEXPR_CXX_20 CommonView(Iterator begin, S end) : Base(iterator(std::move(begin)), iterator(std::move(end))) {
    }
};

/**
 * @addtogroup ItFns
 * @{
 */

/**
 * @brief Creates a common view from an iterator and a sentinel. The iterable must have a sentinel type (i.e. its begin() function
 * must return a different type than its end() function), otherwise a static assertion will fail.
 *
 * @param iterable The iterable to create a common view from.
 * @return A common view object. Can be used in <algorithm> functions.
 */
template<LZ_CONCEPT_ITERABLE Iterable>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 CommonView<IterT<Iterable>, SentinelT<Iterable>> common(Iterable&& iterable) {
    static_assert(detail::HasActualSentinel<Iterable>::value, "Iterable must have a sentinel type");
    return { detail::begin(std::forward<Iterable>(iterable)), detail::end(std::forward<Iterable>(iterable)) };
}

// End of group
/**
 * @}
 */

LZ_MODULE_EXPORT_SCOPE_END
} // namespace lz

#endif
