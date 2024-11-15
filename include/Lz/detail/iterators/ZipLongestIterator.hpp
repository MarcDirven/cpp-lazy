#pragma once

#ifndef LZ_ZIP_LONGEST_ITERATOR_HPP
#define LZ_ZIP_LONGEST_ITERATOR_HPP

#include "Lz/IterBase.hpp"
#include "Lz/detail/FakePointerProxy.hpp"
#include "Lz/detail/Optional.hpp"
#include "Lz/detail/Traits.hpp"

namespace lz {
namespace detail {
template<bool, class...>
class ZipLongestIterator;

template<class... Iterators>
class ZipLongestIterator<false /*is random access*/, Iterators...>
    : public IterBase<ZipLongestIterator<false, Iterators...>, std::tuple<Optional<ValueType<Iterators>>...>,
                      FakePointerProxy<std::tuple<Optional<ValueType<Iterators>>...>>, CommonType<DiffType<Iterators>...>,
                      std::forward_iterator_tag> {
public:
    using iterator_category = std::forward_iterator_tag;
    using value_type = std::tuple<Optional<ValueType<Iterators>>...>;
    using difference_type = CommonType<DiffType<Iterators>...>;
    using reference = value_type;
    using pointer = FakePointerProxy<value_type>;

private:
    using MakeIndexSequenceForThis = MakeIndexSequence<sizeof...(Iterators)>;
    std::tuple<Iterators...> _iterators{};
    std::tuple<Iterators...> _end{};

    template<class I>
    LZ_CONSTEXPR_CXX_20 Optional<ValueType<I>> dereferenceOne(const I& iterator, const I& end) const {
        return iterator == end ? Optional<ValueType<I>>{} : Optional<ValueType<I>>(*iterator);
    }

    template<class I>
    LZ_CONSTEXPR_CXX_20 void incrementOne(I& iterator, const I& end) const {
        if (iterator == end) {
            return;
        }
        ++iterator;
    }

    template<class I>
    LZ_CONSTEXPR_CXX_20 void decrementOne(I& iterator, const I& begin) const {
        if (iterator == begin) {
            return;
        }
        --iterator;
    }

    template<std::size_t... I>
    LZ_CONSTEXPR_CXX_20 value_type dereference(IndexSequence<I...>) const {
        return value_type{ dereferenceOne(std::get<I>(_iterators), std::get<I>(_end))... };
    }

    template<std::size_t... I>
    LZ_CONSTEXPR_CXX_20 void increment(IndexSequence<I...>) {
        decompose((incrementOne(std::get<I>(_iterators), std::get<I>(_end)), 0)...);
    }

    template<std::size_t... I>
    LZ_CONSTEXPR_CXX_20 bool eq(const ZipLongestIterator& other, IndexSequence<I...>) const {
        const bool expander[] = { (std::get<I>(_iterators) == std::get<I>(other._iterators))... };
        const auto end = std::end(expander);
        return std::find(std::begin(expander), end, false) == end;
    }

public:
    LZ_CONSTEXPR_CXX_20 explicit ZipLongestIterator(std::tuple<Iterators...> iterators, std::tuple<Iterators...> end) :
        _iterators(std::move(iterators)),
        _end(std::move(end)) {
    }

    constexpr ZipLongestIterator() = default;

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 value_type dereference() const {
        return dereference(MakeIndexSequenceForThis());
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 pointer arrow() const {
        return FakePointerProxy<decltype(**this)>(**this);
    }

    LZ_CONSTEXPR_CXX_20 void increment() {
        increment(MakeIndexSequenceForThis());
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 bool eq(const ZipLongestIterator& b) const {
        return eq(b, MakeIndexSequenceForThis());
    }
};

template<class... Iterators>
class ZipLongestIterator<true /*is random access*/, Iterators...>
    : public IterBase<ZipLongestIterator<true, Iterators...>, std::tuple<Optional<ValueType<Iterators>>...>,
                      FakePointerProxy<std::tuple<Optional<ValueType<Iterators>>...>>, CommonType<DiffType<Iterators>...>,
                      std::random_access_iterator_tag> {
public:
    using iterator_category = CommonType<IterCat<Iterators>...>;
    using value_type = std::tuple<Optional<ValueType<Iterators>>...>;
    using difference_type = CommonType<DiffType<Iterators>...>;
    using reference = value_type;
    using pointer = FakePointerProxy<value_type>;

private:
    using MakeIndexSequenceForThis = MakeIndexSequence<sizeof...(Iterators)>;
    std::tuple<Iterators...> _begin{};
    std::tuple<Iterators...> _iterators{};
    std::tuple<Iterators...> _end{};

    template<class I>
    LZ_CONSTEXPR_CXX_20 Optional<ValueType<I>> dereferenceOne(const I& iterator, const I& end) const {
        return iterator == end ? Optional<ValueType<I>>{} : Optional<ValueType<I>>(*iterator);
    }

    template<class I>
    LZ_CONSTEXPR_CXX_20 void incrementOne(I& iterator, const I& end) const {
        if (iterator == end) {
            return;
        }
        ++iterator;
    }

    template<class I>
    LZ_CONSTEXPR_CXX_20 void decrementOne(I& iterator, const I& begin) const {
        if (iterator == begin) {
            return;
        }
        --iterator;
    }

    template<std::size_t... I>
    LZ_CONSTEXPR_CXX_20 value_type dereference(IndexSequence<I...>) const {
        return value_type{ dereferenceOne(std::get<I>(_iterators), std::get<I>(_end))... };
    }

    template<std::size_t... I>
    LZ_CONSTEXPR_CXX_20 void increment(IndexSequence<I...>) {
        decompose((incrementOne(std::get<I>(_iterators), std::get<I>(_end)), 0)...);
    }

    template<class I>
    LZ_CONSTEXPR_CXX_20 void decrementOne(I& iterator, const difference_type longest, const difference_type thisIteratorLength) {
        if (thisIteratorLength == longest) {
            --iterator;
        }
    }

    template<std::size_t... I>
    LZ_CONSTEXPR_CXX_20 void decrement(IndexSequence<I...>) {
        const difference_type allSizes[] = { static_cast<difference_type>(std::get<I>(_iterators) - std::get<I>(_begin))... };
        const auto longest = *std::max_element(std::begin(allSizes), std::end(allSizes));
        decompose((decrementOne(std::get<I>(_iterators), longest, allSizes[I]), 0)...);
    }

    template<class I>
    LZ_CONSTEXPR_CXX_20 void plusIsOne(I& iterator, const I& end, const difference_type offset) {
        const auto difference = end - iterator;
        if (offset > difference) {
            iterator = end;
        }
        else {
            iterator += offset;
        }
    }

    template<std::size_t... I>
    LZ_CONSTEXPR_CXX_20 void plusIs(IndexSequence<I...>, const difference_type offset) {
        decompose((plusIsOne(std::get<I>(_iterators), std::get<I>(_end), offset), 0)...);
    }

    template<std::size_t... I>
    LZ_CONSTEXPR_CXX_20 difference_type minus(const ZipLongestIterator& other, IndexSequence<I...>) const {
        const difference_type allSizes[] = { static_cast<difference_type>(std::get<I>(_iterators) -
                                                                          std::get<I>(other._iterators))... };
        return *std::max_element(std::begin(allSizes), std::end(allSizes));
    }

    template<class I>
    LZ_CONSTEXPR_CXX_20 void
    minIsOne(I& iterator, const difference_type thisIteratorLength, const difference_type longest, const difference_type offset) {
        if (thisIteratorLength == longest) {
            iterator -= offset;
            return;
        }
        const auto surplus = thisIteratorLength + offset;
        if (surplus > longest) {
            iterator -= (surplus - longest);
        }
    }

    template<std::size_t... I>
    LZ_CONSTEXPR_CXX_20 void minIs(IndexSequence<I...>, const difference_type offset) {
        const difference_type allSizes[] = { static_cast<difference_type>(std::get<I>(_iterators) - std::get<I>(_begin))... };
        const auto longest = *std::max_element(std::begin(allSizes), std::end(allSizes));
        decompose((minIsOne(std::get<I>(_iterators), allSizes[I], longest, offset), 0)...);
    }

    template<std::size_t... I>
    LZ_CONSTEXPR_CXX_20 bool eq(const ZipLongestIterator& other, IndexSequence<I...>) const {
        const bool expander[] = { (std::get<I>(_iterators) == std::get<I>(other._iterators))... };
        const auto end = std::end(expander);
        return std::find(std::begin(expander), end, false) == end;
    }

    template<std::size_t... I>
    LZ_CONSTEXPR_CXX_20 bool lt(const ZipLongestIterator& other, IndexSequence<I...>) const {
        const difference_type allSizes[] = { static_cast<difference_type>(std::get<I>(_iterators) -
                                                                          std::get<I>(other._iterators))... };
        return *std::min_element(std::begin(allSizes), std::end(allSizes)) < 0;
    }

public:
    LZ_CONSTEXPR_CXX_20 explicit ZipLongestIterator(std::tuple<Iterators...> begin, std::tuple<Iterators...> iterators,
                                                    std::tuple<Iterators...> end) :
        _begin(std::move(begin)),
        _iterators(std::move(iterators)),
        _end(std::move(end)) {
    }

    constexpr ZipLongestIterator() = default;

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 value_type dereference() const {
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
        if (offset < 0) {
            minIs(MakeIndexSequenceForThis(), -offset);
        }
        else {
            plusIs(MakeIndexSequenceForThis(), offset);
        }
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 difference_type difference(const ZipLongestIterator& other) const {
        return minus(other, MakeIndexSequenceForThis());
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 value_type operator[](const difference_type offset) const {
        return *(*this + offset);
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 bool eq(const ZipLongestIterator& b) const {
        return eq(b, MakeIndexSequenceForThis());
    }
};
} // namespace detail
} // namespace lz
#endif // LZ_ZIP_LONGEST_ITERATOR_HPP