#pragma once

#ifndef LZ_INCLUSIVE_SCAN_HPP
#define LZ_INCLUSIVE_SCAN_HPP

#include "detail/BasicIteratorView.hpp"
#include "detail/iterators/InclusiveScanIterator.hpp"

namespace lz {
LZ_MODULE_EXPORT_SCOPE_BEGIN

template<class Iterator, class T, class BinaryOp>
class InclusiveScan final : public detail::BasicIteratorView<detail::InclusiveScanIterator<Iterator, T, BinaryOp>> {
public:
    using iterator = detail::InclusiveScanIterator<Iterator, T, BinaryOp>;
    using const_iterator = iterator;

    constexpr InclusiveScan() = default;

    LZ_CONSTEXPR_CXX_14 InclusiveScan(Iterator first, Iterator last, T init, BinaryOp binaryOp) :
        detail::BasicIteratorView<iterator>(iterator(first, last, init, binaryOp), iterator(last, last, init, binaryOp)) {
    }
};

/**
 * @addtogroup ItFns
 * @{
 */

/**
 * @brief Returns an inclusive scan iterator.
 * @details Returns an inclusive scan iterator. This iterator begins by returning `binOp(init, *first)`. It then
 * proceeds to the next one, which is essentially the previously calculated value (calculated by `binOp(std::move(current), *it)`
 * + the current iterator element being handled. Example:
 * @example
 * ```cpp
 * int array[] = {3, 5, 2, 3, 4, 2, 3};
 * // start the scan from 3
 * auto scan = lz::iScan(array);
 *
 * for (const int& i : scan) {
 *     fmt::print("{} ", i);
 * }
 * // prints 3 8 10 13 17 19 22
 * ```
 *
 * @param first The iterator denoting the beginning
 * @param last The iterator denoting the end
 * @param init The value to start with
 * @param binOp The fold function. Essentially, it is executed as (`init = binOp(std::move(init), *iterator);`)
 * @return An inclusive scan view object.
 */
template<LZ_CONCEPT_ITERATOR Iterator, class T = detail::ValueType<Iterator>,
         class BinaryOp = MAKE_BIN_OP(std::plus, detail::ValueType<Iterator>)>
LZ_NODISCARD LZ_CONSTEXPR_CXX_14 InclusiveScan<Iterator, detail::ValueType<Iterator>, detail::Decay<BinaryOp>>
iScan(Iterator first, Iterator last, T&& init = {}, BinaryOp&& binOp = {}) {
    auto tmp = binOp(std::forward<T>(init), *first);
    return { std::move(first), std::move(last), std::move(tmp), std::forward<BinaryOp>(binOp) };
}

/**
 * @brief Returns an inclusive scan iterator.
 * @details Returns an inclusive scan iterator. This iterator begins by returning `*std::begin(iterable)`. It then
 * proceeds to the next one, which is essentially the previously calculated value (calculated by `binOp(std::move(current), *it)`
 * + the current iterator element being handled. Example:
 * @example
 * ```cpp
 * int array[] = {3, 5, 2, 3, 4, 2, 3};
 * // start the scan from 3
 * auto scan = lz::iScan(array);
 *
 * for (const int& i : scan) {
 *     fmt::print("{} ", i);
 * }
 * // prints 3 8 10 13 17 19 22
 * ```
 *
 * @param iterable The iterable to perform the inclusive scan over
 * @param init The value to start with
 * @param binOp The fold function. Essentially, it is executed as (`init = binOp(std::move(init), *iterator);`)
 * @return An inclusive scan view object.
 */
// clang-format off
template<LZ_CONCEPT_ITERABLE Iterable, class T = detail::ValueTypeIterable<Iterable>, 
         class BinaryOp = MAKE_BIN_OP(std::plus, detail::ValueTypeIterable<Iterable>)>
LZ_NODISCARD LZ_CONSTEXPR_CXX_14
InclusiveScan<detail::IterTypeFromIterable<Iterable>, detail::ValueTypeIterable<Iterable>, detail::Decay<BinaryOp>>
iScan(Iterable&& iterable, T&& init = {}, BinaryOp&& binOp = {}) {
    return iScan(detail::begin(std::forward<Iterable>(iterable)), detail::end(std::forward<Iterable>(iterable)), 
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