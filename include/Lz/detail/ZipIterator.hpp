#pragma once

#ifndef LZ_ZIP_ITERATOR_HPP
#define LZ_ZIP_ITERATOR_HPP

#include "LzTools.hpp"

#include <algorithm>

namespace lz {
namespace internal {
template<LZ_CONCEPT_ITERATOR... Iterators>
class ZipIterator {
public:
    using iterator_category = typename std::common_type<IterCat<Iterators>...>::type;
    using value_type = std::tuple<ValueType<Iterators>...>;
    using difference_type = typename std::common_type<DiffType<Iterators>...>::type;
    using reference = std::tuple<RefType<Iterators>...>;
    using pointer = std::tuple<PointerType<Iterators>...>;

private:
    using MakeIndexSequenceForThis = MakeIndexSequence<sizeof...(Iterators)>;
    std::tuple<Iterators...> _iterators{};

    template<std::size_t... I>
    LZ_CONSTEXPR_CXX_20 reference dereference(IndexSequence<I...>) const {
        return { *std::get<I>(_iterators)... };
    }

    template<std::size_t... I>
    LZ_CONSTEXPR_CXX_20 void increment(IndexSequence<I...>) {
        const int expand[] = { (++std::get<I>(_iterators), 0)... };
        static_cast<void>(expand);
    }

    template<std::size_t... I>
    LZ_CONSTEXPR_CXX_20 void decrement(IndexSequence<I...>) {
        const int expand[] = { (--std::get<I>(_iterators), 0)... };
        static_cast<void>(expand);
    }

    template<std::size_t... I>
    LZ_CONSTEXPR_CXX_20 void plusIs(IndexSequence<I...>, const difference_type differenceType) {
        const int expand[] = { ((std::get<I>(_iterators) += differenceType), 0)... };
        static_cast<void>(expand);
    }

    template<std::size_t... I>
    LZ_CONSTEXPR_CXX_20 void minIs(const IndexSequence<I...> is, const difference_type differenceType) {
        plusIs(is, -differenceType);
    }

public:
    LZ_CONSTEXPR_CXX_20 explicit ZipIterator(std::tuple<Iterators...> iterators) : _iterators(std::move(iterators)) {
    }

    constexpr ZipIterator() = default;

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 reference operator*() const {
        return dereference(MakeIndexSequenceForThis());
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 pointer operator->() const {
        return FakePointerProxy<decltype(**this)>(**this);
    }

    LZ_CONSTEXPR_CXX_20 ZipIterator& operator++() {
        increment(MakeIndexSequenceForThis());
        return *this;
    }

    LZ_CONSTEXPR_CXX_20 ZipIterator operator++(int) {
        ZipIterator tmp(*this);
        ++*this;
        return tmp;
    }

    LZ_CONSTEXPR_CXX_20 ZipIterator& operator--() {
        decrement(MakeIndexSequenceForThis());
        return *this;
    }

    LZ_CONSTEXPR_CXX_20 ZipIterator operator--(int) {
        auto tmp(*this);
        --*this;
        return tmp;
    }

    LZ_CONSTEXPR_CXX_20 ZipIterator& operator+=(const difference_type offset) {
        plusIs(MakeIndexSequenceForThis(), offset);
        return *this;
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 ZipIterator operator+(const difference_type offset) const {
        ZipIterator tmp(*this);
        tmp += offset;
        return tmp;
    }

    LZ_CONSTEXPR_CXX_20 ZipIterator& operator-=(const difference_type offset) {
        minIs(MakeIndexSequenceForThis(), offset);
        return *this;
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 ZipIterator operator-(const difference_type offset) const {
        ZipIterator tmp(*this);
        tmp -= offset;
        return tmp;
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 difference_type operator-(const ZipIterator& other) const {
        return std::get<0>(_iterators) - std::get<0>(other._iterators);
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 reference operator[](const difference_type offset) const {
        return *(*this + offset);
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 friend bool operator==(const ZipIterator& a, const ZipIterator& b) {
        return !(a != b); // NOLINT
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 friend bool operator!=(const ZipIterator& a, const ZipIterator& b) {
        return std::get<0>(a._iterators) != std::get<0>(b._iterators);
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 friend bool operator<(const ZipIterator& a, const ZipIterator& b) {
        return std::get<0>(a._iterators) < std::get<0>(b._iterators);
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 friend bool operator>(const ZipIterator& a, const ZipIterator& b) {
        return b < a;
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 friend bool operator<=(const ZipIterator& a, const ZipIterator& b) {
        return !(b < a); // NOLINT
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 friend bool operator>=(const ZipIterator& a, const ZipIterator& b) {
        return !(a < b); // NOLINT
    }
};
} // namespace internal
} // namespace lz

#endif