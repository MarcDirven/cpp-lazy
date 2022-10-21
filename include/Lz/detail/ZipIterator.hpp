#pragma once

#ifndef LZ_ZIP_ITERATOR_HPP
#    define LZ_ZIP_ITERATOR_HPP

#    include "LzTools.hpp"

#    include <algorithm>

namespace lz {
namespace internal {
template<class... Iterators>
class ZipIterator {
    using CurrentCat = typename std::common_type<IterCat<Iterators>...>::type;

public:
    using iterator_category = Conditional<IsBidirectionalTag<CurrentCat>::value, std::forward_iterator_tag, CurrentCat>;
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
        decompose((++std::get<I>(_iterators), 0)...);
    }

    template<std::size_t... I>
    LZ_CONSTEXPR_CXX_20 void decrement(IndexSequence<I...>) {
        decompose((--std::get<I>(_iterators), 0)...);
    }

    template<std::size_t... I>
    LZ_CONSTEXPR_CXX_20 void plusIs(IndexSequence<I...>, const difference_type offset) {
        decompose(((std::get<I>(_iterators) += offset), 0)...);
    }

    template<std::size_t... I>
    LZ_CONSTEXPR_CXX_20 difference_type minus(const ZipIterator& other, IndexSequence<I...>) const {
        const difference_type expand[] = { ((std::get<I>(_iterators) - std::get<I>(other._iterators)))... };
        return *std::min_element(std::begin(expand), std::end(expand));
    }

    template<std::size_t... I>
    LZ_CONSTEXPR_CXX_20 void minIs(const IndexSequence<I...> is, const difference_type offset) {
        plusIs(is, -offset);
    }

    template<std::size_t... I>
    LZ_CONSTEXPR_CXX_20 bool eq(const ZipIterator& other, IndexSequence<I...>) const {
        const bool expander[] = { (std::get<I>(_iterators) == std::get<I>(other._iterators))... };
        const auto end = std::end(expander);
        return std::find(std::begin(expander), end, true) != end;
    }

    template<std::size_t... I>
    LZ_CONSTEXPR_CXX_20 bool lt(const ZipIterator& other, IndexSequence<I...>) const {
        const bool expander[] = { (std::get<I>(_iterators) < std::get<I>(other._iterators))... };
        const auto end = std::end(expander);
        return std::find(std::begin(expander), end, true) != end;
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
        return minus(other, MakeIndexSequenceForThis());
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 reference operator[](const difference_type offset) const {
        return *(*this + offset);
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 friend bool operator==(const ZipIterator& a, const ZipIterator& b) {
        return a.eq(b, MakeIndexSequenceForThis());
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 friend bool operator!=(const ZipIterator& a, const ZipIterator& b) {
        return !(a == b);
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 friend bool operator<(const ZipIterator& a, const ZipIterator& b) {
        return a.lt(b, MakeIndexSequenceForThis());
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