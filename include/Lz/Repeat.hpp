#pragma once

#ifndef LZ_REPEAT_HPP
#define LZ_REPEAT_HPP

#include "detail/BasicIteratorView.hpp"
#include "detail/RepeatIterator.hpp"

namespace lz {
template<class T>
class Repeat final : public internal::BasicIteratorView<internal::RepeatIterator<T>> {
  public:
    using iterator = internal::RepeatIterator<T>;
    using value_type = T;

    constexpr Repeat(T toRepeat, const std::size_t amount)
        : internal::BasicIteratorView<iterator>(iterator(toRepeat, 0, amount), iterator(toRepeat, amount, amount)) {
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
LZ_NODISCARD constexpr Repeat<T> repeat(T toRepeat, const std::size_t amount = (std::numeric_limits<std::size_t>::max)()) {
    return Repeat<T>(std::move(toRepeat), amount);
}

// End of group
/**
 * @}
 */
} // namespace lz

#endif