#pragma once

#ifndef LZ_INCLUSIVE_SCAN_HPP
#    define LZ_INCLUSIVE_SCAN_HPP

#    include "detail/BasicIteratorView.hpp"
#    include "detail/InclusiveScanIterator.hpp"

namespace lz {
LZ_MODULE_EXPORT_SCOPE_BEGIN

template<class Iterator, class T, class BinaryOp>
class InclusiveScan final : public internal::BasicIteratorView<internal::InclusiveScanIterator<Iterator, T, BinaryOp>> {
public:
    using iterator = internal::InclusiveScanIterator<Iterator, T, BinaryOp>;
    using const_iterator = iterator;

    constexpr InclusiveScan() = default;

    LZ_CONSTEXPR_CXX_14 InclusiveScan(Iterator first, Iterator last, T init, BinaryOp binaryOp) :
        internal::BasicIteratorView<iterator>(iterator(std::move(first), init, binaryOp),
                                              iterator(std::move(last), init, binaryOp)) {
    }
};

/**
 * @addtogroup ItFns
 * @{
 */

template<LZ_CONCEPT_ITERATOR Iterator, class T, class BinaryOp = MAKE_BIN_OP(std::plus, internal::Decay<T>)>
LZ_NODISCARD LZ_CONSTEXPR_CXX_14 InclusiveScan<Iterator, internal::Decay<T>, internal::Decay<BinaryOp>>
iScanFrom(Iterator first, Iterator last, T&& init, BinaryOp&& binOp = {}) {
    auto tmp = binOp(std::forward<T>(init), *first);
    return { std::move(first), std::move(last), std::move(tmp), std::forward<BinaryOp>(binOp) };
}

// clang-format off
template<LZ_CONCEPT_ITERABLE Iterable, class T, class BinaryOp = MAKE_BIN_OP(std::plus, internal::Decay<T>)>
LZ_NODISCARD LZ_CONSTEXPR_CXX_14
InclusiveScan<internal::IterTypeFromIterable<Iterable>, internal::Decay<T>, internal::Decay<BinaryOp>>
iScanFrom(Iterable&& iterable, T&& init, BinaryOp&& binOp = {}) {
    return iScanFrom(internal::begin(std::forward<Iterable>(iterable)), internal::end(std::forward<Iterable>(iterable)),
                     std::forward<T>(init), std::forward<BinaryOp>(binOp));
}
// clang-format on

template<LZ_CONCEPT_ITERATOR Iterator, class BinaryOp = MAKE_BIN_OP(std::plus, internal::ValueType<Iterator>)>
LZ_NODISCARD LZ_CONSTEXPR_CXX_14 InclusiveScan<Iterator, internal::ValueType<Iterator>, internal::Decay<BinaryOp>>
iScan(Iterator first, Iterator last, BinaryOp&& binOp = {}) {
    auto tmp = *first;
    return { std::move(first), std::move(last), std::move(tmp), std::forward<BinaryOp>(binOp) };
}

// clang-format off
template<LZ_CONCEPT_ITERABLE Iterable, class BinaryOp = MAKE_BIN_OP(std::plus, internal::ValueTypeIterable<Iterable>)>
LZ_NODISCARD LZ_CONSTEXPR_CXX_14
InclusiveScan<internal::IterTypeFromIterable<Iterable>, internal::ValueTypeIterable<Iterable>, internal::Decay<BinaryOp>>
iScan(Iterable&& iterable, BinaryOp&& binOp = {}) {
    return iScan(internal::begin(std::forward<Iterable>(iterable)), internal::end(std::forward<Iterable>(iterable)), std::forward<BinaryOp>(binOp));
}
// clang-format on

// End of group
/**
 * @}
 */

LZ_MODULE_EXPORT_SCOPE_END
} // namespace lz

#endif