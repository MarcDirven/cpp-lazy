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
    LZ_CONSTEXPR_CXX_14 reference dereference(IndexSequence<I...>) const {
        return { *std::get<I>(_iterators)... };
    }

    template<std::size_t... I>
    LZ_CONSTEXPR_CXX_14 void increment(IndexSequence<I...>) {
        const int expand[] = { (++std::get<I>(_iterators), 0)... };
        static_cast<void>(expand);
    }

    template<std::size_t... I>
    LZ_CONSTEXPR_CXX_14 void decrement(IndexSequence<I...>) {
        const int expand[] = { (--std::get<I>(_iterators), 0)... };
        static_cast<void>(expand);
    }

    template<std::size_t... I>
    LZ_CONSTEXPR_CXX_14 void plusIs(IndexSequence<I...>, const difference_type differenceType) {
        const int expand[] = { ((std::get<I>(_iterators) += differenceType), 0)... };
        static_cast<void>(expand);
    }

    template<std::size_t... I>
    LZ_CONSTEXPR_CXX_14 void minIs(IndexSequence<I...>, const difference_type differenceType) {
        plusIs(MakeIndexSequenceForThis(), -differenceType);
    }

    template<std::size_t... I>
    LZ_CONSTEXPR_CXX_17 difference_type iteratorMin(IndexSequence<I...>, const ZipIterator& other) const {
        const difference_type diff[] = { static_cast<difference_type>(std::get<I>(_iterators) -
                                                                      std::get<I>(other._iterators))... };
        return static_cast<difference_type>(*std::min_element(std::begin(diff), std::end(diff)));
    }

    template<std::size_t... I>
    LZ_CONSTEXPR_CXX_20 bool lessThan(IndexSequence<I...>, const ZipIterator& other) const {
        const bool distances[] = { (std::get<I>(_iterators) < std::get<I>(other._iterators))... };
        const auto* const end = std::end(distances);
        return std::find(std::begin(distances), end, true) != end;
    }

    template<std::size_t... I>
    LZ_CONSTEXPR_CXX_20 bool notEqual(IndexSequence<I...>, const ZipIterator& other) const {
        const bool boolValues[] = { (std::get<I>(_iterators) != std::get<I>(other._iterators))... };
        const auto* const end = std::end(boolValues);
        return std::find(std::begin(boolValues), end, false) == end; // Check if false not in boolValues
    }

  public:
    LZ_CONSTEXPR_CXX_14 explicit ZipIterator(std::tuple<Iterators...> iterators) : _iterators(std::move(iterators)) {
    }

    constexpr ZipIterator() = default;

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 reference operator*() const {
        return dereference(MakeIndexSequenceForThis());
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 pointer operator->() const {
        return FakePointerProxy<decltype(**this)>(**this);
    }

    LZ_CONSTEXPR_CXX_14 ZipIterator& operator++() {
        increment(MakeIndexSequenceForThis());
        return *this;
    }

    LZ_CONSTEXPR_CXX_14 ZipIterator operator++(int) {
        ZipIterator tmp(*this);
        ++*this;
        return tmp;
    }

    LZ_CONSTEXPR_CXX_14 ZipIterator& operator--() {
        decrement(MakeIndexSequenceForThis());
        return *this;
    }

    LZ_CONSTEXPR_CXX_14 ZipIterator operator--(int) {
        auto tmp(*this);
        --*this;
        return tmp;
    }

    LZ_CONSTEXPR_CXX_14 ZipIterator& operator+=(const difference_type offset) {
        plusIs(MakeIndexSequenceForThis(), offset);
        return *this;
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 ZipIterator operator+(const difference_type offset) const {
        ZipIterator tmp(*this);
        tmp += offset;
        return tmp;
    }

    LZ_CONSTEXPR_CXX_14 ZipIterator& operator-=(const difference_type offset) {
        minIs(MakeIndexSequenceForThis(), offset);
        return *this;
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 ZipIterator operator-(const difference_type offset) const {
        ZipIterator tmp(*this);
        tmp -= offset;
        return tmp;
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_17 difference_type operator-(const ZipIterator& other) const {
        return iteratorMin(MakeIndexSequenceForThis(), other);
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 reference operator[](const difference_type offset) const {
        return *(*this + offset);
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 friend bool operator==(const ZipIterator& a, const ZipIterator& b) {
        return !(a != b); // NOLINT
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 friend bool operator!=(const ZipIterator& a, const ZipIterator& b) {
        return a.notEqual(MakeIndexSequenceForThis(), b);
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 friend bool operator<(const ZipIterator& a, const ZipIterator& b) {
        return a.lessThan(MakeIndexSequenceForThis(), b);
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