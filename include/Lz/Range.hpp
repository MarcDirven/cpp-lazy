#pragma once

#ifndef LZ_RANGE_HPP
#define LZ_RANGE_HPP

#include "detail/BasicIteratorView.hpp"
#include "detail/RangeIterator.hpp"

namespace lz {
template<LZ_CONCEPT_ARITHMETIC Arithmetic>
class Range final : public internal::BasicIteratorView<internal::RangeIterator<Arithmetic>> {
  public:
    using iterator = internal::RangeIterator<Arithmetic>;
    using const_iterator = iterator;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using value_type = typename iterator::value_type;

  private:
    using Base = internal::BasicIteratorView<iterator>;

  public:
    /**
     * @brief Range iterator constructor from [start, end) with step.
     * @param start The start of the counting.
     * @param end The end of the counting.
     * @param step The step that gets added every iteration.
     */
    constexpr Range(const Arithmetic start, const Arithmetic end, const Arithmetic step) noexcept :
        Base(iterator(start, step), iterator(end, step)) {
    }

    constexpr Range() = default;
};

// Start of group
/**
 * @addtogroup ItFns
 * @{
 */

/**
 * @brief Returns a random access range object with specified [start, end) and a step.
 * @details E.g. `lz::range(3, 20, 2)` will return all values between [3, 20) with a step of 2 when iterating over
 * its iterator.
 * @tparam Arithmetic Int is automatically assumed, may be any arithmetic type.
 * @param end Specifies when to stop iterator after `end` count. It is assumed from [start, end) with a step.
 * @param step The incrementing value after each loop.
 * @param start Specifies where to start counting.
 * @return A Range object that can be converted to an arbitrary container or can be iterated over using
 * `for (auto... lz::range(...))`.
 */
template<LZ_CONCEPT_ARITHMETIC Arithmetic = int>
LZ_NODISCARD constexpr Range<Arithmetic> range(const Arithmetic start, const Arithmetic end, const Arithmetic step = 1) noexcept {
#ifndef LZ_HAS_CONCEPTS
    static_assert(std::is_arithmetic<Arithmetic>::value, "type must be of type arithmetic");
#endif
    return Range<Arithmetic>(start, end, step);
}

/**
 * @brief Returns a random access range object.
 * @details E.g. `lz::range(50)` will return all values between [0, 50) when iterating over its iterator.
 * @tparam Arithmetic Int is automatically assumed, may be any arithmetic type.
 * @param end Specifies when to stop iterator after `end` count. It is assumed from [0, end).
 * @return A Range object that can be converted to an arbitrary container or can be iterated over using
 * `for (auto... lz::range(...))`.
 */
template<LZ_CONCEPT_ARITHMETIC Arithmetic = int>
LZ_NODISCARD constexpr Range<Arithmetic> range(const Arithmetic end) noexcept {
    return range<Arithmetic>(0, end, 1);
}

// End of group
/**
 * @}
 */
} // namespace lz

#endif