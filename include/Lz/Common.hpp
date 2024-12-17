#pragma once

#ifndef LZ_COMMON_HPP
#define LZ_COMMON_HPP

#include "Lz/detail/basic_iterable.hpp"
#include "Lz/detail/compiler_checks.hpp"
#include "Lz/detail/iterators/common.hpp"

namespace lz {

LZ_MODULE_EXPORT_SCOPE_BEGIN

template<class Iterator, class S>
class common_iterable : public detail::basic_iterable<detail::common_iterator<Iterator, S>> {
public:
    using iterator = detail::common_iterator<Iterator, S>;
    using const_iterator = iterator;
    using value_type = typename iterator::value_type;

    common_iterable() = default;

    LZ_CONSTEXPR_CXX_20 common_iterable(Iterator begin, S end) :
        detail::basic_iterable<iterator>(iterator(std::move(begin)), iterator(std::move(end))) {
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
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 common_iterable<iter_t<Iterable>, sentinel_t<Iterable>> common(Iterable&& iterable) {
    static_assert(!std::is_same<iter_t<Iterable>, sentinel_t<Iterable>>::value, "Iterator and Sentinel must be different types");
    return { detail::begin(std::forward<Iterable>(iterable)), detail::end(std::forward<Iterable>(iterable)) };
}

// End of group
/**
 * @}
 */

LZ_MODULE_EXPORT_SCOPE_END
} // namespace lz

#endif
