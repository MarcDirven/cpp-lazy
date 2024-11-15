#pragma once

#ifndef LZ_ZIP_ITERATOR_HPP
#define LZ_ZIP_ITERATOR_HPP

#include "Lz/IterBase.hpp"
#include "Lz/detail/FakePointerProxy.hpp"
#include "Lz/detail/Traits.hpp"

#include <algorithm>

namespace lz {
namespace detail {
template<class... Iterators>
class ZipIterator : public IterBase<ZipIterator<Iterators...>, std::tuple<RefType<Iterators>...>,
                                    std::tuple<FakePointerProxy<RefType<Iterators>>...>, CommonType<DiffType<Iterators>...>,
                                    CommonType<IterCat<Iterators>...>> {

public:
    using iterator_category = CommonType<IterCat<Iterators>...>;
    using value_type = std::tuple<ValueType<Iterators>...>;
    using difference_type = CommonType<DiffType<Iterators>...>;
    using reference = std::tuple<RefType<Iterators>...>;
    using pointer = std::tuple<FakePointerProxy<RefType<Iterators>>...>;

private:
    using MakeIndexSequenceForThis = MakeIndexSequence<sizeof...(Iterators)>;
    std::tuple<Iterators...> _iterators{};

    template<std::size_t... I>
    LZ_CONSTEXPR_CXX_20 reference dereference(IndexSequence<I...>) const {
        return reference{ *std::get<I>(_iterators)... };
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
    LZ_CONSTEXPR_CXX_20 bool eq(const ZipIterator& other, IndexSequence<I...>) const {
        const bool expander[] = { (std::get<I>(_iterators) == std::get<I>(other._iterators))... };
        const auto end = std::end(expander);
        return std::find(std::begin(expander), end, true) != end;
    }

public:
    LZ_CONSTEXPR_CXX_20 explicit ZipIterator(std::tuple<Iterators...> iterators) : _iterators(std::move(iterators)) {
    }

    constexpr ZipIterator() = default;

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 reference dereference() const {
        return dereference(MakeIndexSequenceForThis());
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 pointer arrow() const {
        return FakePointerProxy<decltype(**this)>(**this);
    }

    LZ_CONSTEXPR_CXX_20 void increment() {
        increment(MakeIndexSequenceForThis());
    }

    LZ_CONSTEXPR_CXX_20 void decrement() {
        decrement(MakeIndexSequenceForThis());
    }

    LZ_CONSTEXPR_CXX_20 void plusIs(const difference_type offset) {
        plusIs(MakeIndexSequenceForThis(), offset);
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 difference_type difference(const ZipIterator& other) const {
        return minus(other, MakeIndexSequenceForThis());
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 bool eq(const ZipIterator& b) const {
        return eq(b, MakeIndexSequenceForThis());
    }
};
} // namespace detail
} // namespace lz

#endif