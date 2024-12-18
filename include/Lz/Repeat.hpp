#pragma once

#ifndef LZ_REPEAT_HPP
#define LZ_REPEAT_HPP

#include "Lz/detail/basic_iterable.hpp"
#include "Lz/detail/iterators/repeat.hpp"

namespace lz {

LZ_MODULE_EXPORT_SCOPE_BEGIN

template<class T>
class repeat_iterable final : public detail::basic_iterable<detail::repeat_iterator<T>, default_sentinel> {
public:
    using iterator = detail::repeat_iterator<T>;
    using const_iterator = iterator;
    using value_type = T;

    constexpr repeat_iterable(T to_repeat, const std::size_t amount) :
        detail::basic_iterable<iterator, default_sentinel>(iterator(std::move(to_repeat), amount)) {
    }

    constexpr repeat_iterable() = default;
};

// Start of group
/**
 * @addtogroup ItFns
 * @{
 */

/**
 * @brief Returns @p `to_repeat`, @p `amount` of times. If amount is left empty, it never stops and is interpreted as a
 * `while-true` loop.
 * @param to_repeat The value to repeat `amount` times.
 * @param amount The amount of times to repeat the loop, returning `to_repeat`.
 * @return A repeat object, containing the random access iterator.
 */
template<class T>
LZ_NODISCARD constexpr repeat_iterable<detail::decay<T>>
repeat(T&& to_repeat, const std::size_t amount = (std::numeric_limits<std::size_t>::max)()) {
    return { std::forward<T>(to_repeat), amount };
}

// End of group
/**
 * @}
 */

LZ_MODULE_EXPORT_SCOPE_END

} // namespace lz

#endif