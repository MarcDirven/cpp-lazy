#pragma once

#ifndef LZ_RANGE_HPP
#define LZ_RANGE_HPP

#include "detail/BasicIteratorView.hpp"
#include "detail/iterators/RangeIterator.hpp"

namespace lz {

LZ_MODULE_EXPORT_SCOPE_BEGIN

template<LZ_CONCEPT_ARITHMETIC Arithmetic>
class Range final : public detail::BasicIteratorView<detail::RangeIterator<Arithmetic>> {
public:
    using iterator = detail::RangeIterator<Arithmetic>;
    using const_iterator = iterator;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using value_type = typename iterator::value_type;

private:
    using Base = detail::BasicIteratorView<iterator>;

public:
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
template<class Arithmetic = int>
LZ_NODISCARD constexpr Range<Arithmetic> range(const Arithmetic start, const Arithmetic end, const Arithmetic step = 1) noexcept {
    return { start, end, step };
}

/**
 * @brief Returns a random access range object.
 * @details E.g. `lz::range(50)` will return all values between [0, 50) when iterating over its iterator.
 * @tparam Arithmetic Int is automatically assumed, may be any arithmetic type.
 * @param end Specifies when to stop iterator after `end` count. It is assumed from [0, end).
 * @return A Range object that can be converted to an arbitrary container or can be iterated over using
 * `for (auto... lz::range(...))`.
 */
template<class Arithmetic = int>
LZ_NODISCARD constexpr Range<Arithmetic> range(const Arithmetic end) noexcept {
    return range<Arithmetic>(0, end, 1);
}

// End of group
/**
 * @}
 */

LZ_MODULE_EXPORT_SCOPE_END

} // namespace lz

#endif