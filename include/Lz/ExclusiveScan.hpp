#pragma once

#ifndef LZ_EXCLUSIVE_SCAN_HPP
#    define LZ_EXCLUSIVE_SCAN_HPP

#    include "detail/BasicIteratorView.hpp"
#    include "detail/ExclusiveScanIterator.hpp"

namespace lz {
LZ_MODULE_EXPORT_SCOPE_BEGIN

template<class Iterator, class T, class BinaryOp>
class ExclusiveScan final : public internal::BasicIteratorView<internal::ExclusiveScanIterator<Iterator, T, BinaryOp>> {
public:
    using iterator = internal::ExclusiveScanIterator<Iterator, T, BinaryOp>;
    using const_iterator = iterator;

    constexpr ExclusiveScan() = default;

    LZ_CONSTEXPR_CXX_14 ExclusiveScan(Iterator first, Iterator last, T init, BinaryOp binaryOp) :
        internal::BasicIteratorView<iterator>(iterator(std::move(first), init, binaryOp),
                                              iterator(std::move(last), init, binaryOp)) {
    }
};

/**
 * @addtogroup ItFns
 * @{
 */

/**
 * @brief Returns an exclusive scan iterator.
 * @details Returns an exclusive scan iterator. This iterator begins by returning @p init. It then uses @p binOp to calculate the
 * next value, which is essentially @p init (the previously calculated value) + the current iterator element being handled.
 * Example:
 * @example
 * ```cpp
 * int array[] = {3, 5, 2, 3, 4, 2, 3};
 * // start the scan from 0
 * auto scan = lz::eScan(array, 0);
 *
 * for (const int& i : scan) {
 *     fmt::print("{} ", i);
 * }
 * // prints 0 3 8 10 13 17 19
 * ```
 *
 * @param first The beginning of the iterator
 * @param last The ending of the iterator
 * @param init The value to start with
 * @param binOp The fold function. Essentially, it is executed as (`init = binOp(std::move(init), *iterator);`)
 * @return An exclusive scan view object.
 */
template<LZ_CONCEPT_ITERATOR Iterator, class T = internal::ValueType<Iterator>,
         class BinaryOp = MAKE_BIN_OP(std::plus, internal::Decay<T>)>
LZ_NODISCARD LZ_CONSTEXPR_CXX_14 ExclusiveScan<Iterator, internal::Decay<T>, internal::Decay<BinaryOp>>
eScan(Iterator first, Iterator last, T&& init = {}, BinaryOp&& binOp = {}) {
    return { std::move(first), std::move(last), std::move(init), std::forward<BinaryOp>(binOp) };
}

/**
 * @brief Returns an exclusive scan iterator.
 * @details Returns an exclusive scan iterator. This iterator begins by returning @p init. It then uses @p binOp to calculate the
 * next value, which is essentially @p init (the previously calculated value) + the current iterator element being handled.
 * Example:
 * @example
 * ```cpp
 * int array[] = {3, 5, 2, 3, 4, 2, 3};
 * // start the scan from 0
 * auto scan = lz::eScan(array, 0);
 *
 * for (const int& i : scan) {
 *     fmt::print("{} ", i);
 * }
 * // prints 0 3 8 10 13 17 19
 * ```
 *
 * @param iterable The iterable to perform the exclusive scan over
 * @param init The value to start with
 * @param binOp The fold function. Essentially, it is executed as (`init = binOp(std::move(init), *iterator);`)
 * @return An exclusive scan view object.
 */
// clang-format off
template<LZ_CONCEPT_ITERABLE Iterable, class T = internal::ValueTypeIterable<Iterable>, class BinaryOp = MAKE_BIN_OP(std::plus, internal::Decay<T>)>
LZ_NODISCARD LZ_CONSTEXPR_CXX_14
ExclusiveScan<internal::IterTypeFromIterable<Iterable>, internal::Decay<T>, internal::Decay<BinaryOp>>
eScan(Iterable&& iterable, T&& init = {}, BinaryOp&& binOp = {}) {
    return eScan(internal::begin(std::forward<Iterable>(iterable)), internal::end(std::forward<Iterable>(iterable)),
                 std::forward<T>(init), std::forward<BinaryOp>(binOp));
}
// clang-format on

// End of group
/**
 * @}
 */

LZ_MODULE_EXPORT_SCOPE_END
} // namespace lz

#endif