#pragma once

#ifndef LZ_REPEAT_HPP
#define LZ_REPEAT_HPP

#include "detail/BasicIterable.hpp"
#include "detail/iterators/RepeatIterator.hpp"

namespace lz {

LZ_MODULE_EXPORT_SCOPE_BEGIN

template<class T>
class Repeat final : public detail::basic_iterable<detail::repeat_iterator<T>, default_sentinel> {
public:
    using iterator = detail::repeat_iterator<T>;
    using const_iterator = iterator;
    using value_type = T;

    constexpr Repeat(T toRepeat, const std::size_t amount) :
        detail::basic_iterable<iterator, default_sentinel>(iterator(std::move(toRepeat), amount)) {
    }

    constexpr Repeat() = default;
};

// Start of group
/**
 * @addtogroup ItFns
 * @{
 */

/**
 * @brief Returns `toRepeat`, `amount` of times. If amount is left empty, it never stops and is interpreted as a
 * `while-true` loop.
 * @param toRepeat The value to repeat `amount` times.
 * @param amount The amount of times to repeat the loop, returning `toRepeat`.
 * @return A repeat object, containing the random access iterator.
 */
template<class T>
LZ_NODISCARD constexpr Repeat<detail::decay<T>>
repeat(T&& toRepeat, const std::size_t amount = (std::numeric_limits<std::size_t>::max)()) {
    return { std::forward<T>(toRepeat), amount };
}

// End of group
/**
 * @}
 */

LZ_MODULE_EXPORT_SCOPE_END

} // namespace lz

#endif