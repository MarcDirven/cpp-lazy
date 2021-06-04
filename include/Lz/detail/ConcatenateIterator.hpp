#pragma once

#ifndef LZ_CONCATENATE_ITERATOR_HPP
#define LZ_CONCATENATE_ITERATOR_HPP

#include "LzTools.hpp"

#include <numeric>

namespace lz {
namespace internal {
template<class Tuple, std::size_t I, class = void>
struct PlusPlus {
    LZ_CONSTEXPR_CXX_17 void operator()(Tuple& iterators, const Tuple& end) const {
        if (std::get<I>(iterators) != std::get<I>(end)) {
            ++std::get<I>(iterators);
        }
        else {
            PlusPlus<Tuple, I + 1>()(iterators, end);
        }
    }
};

template<class Tuple, std::size_t I>
struct PlusPlus<Tuple, I, EnableIf<I == std::tuple_size<Decay<Tuple>>::value>> {
    LZ_CONSTEXPR_CXX_17 void operator()(const Tuple& /*iterators*/, const Tuple& /*end*/) const {
    }
};

template<class Tuple, std::size_t I, class = void>
struct NotEqual {
    LZ_CONSTEXPR_CXX_17 bool operator()(const Tuple& iterators, const Tuple& end) const {
        const bool iterHasValue = std::get<I>(iterators) != std::get<I>(end);
        return iterHasValue ? iterHasValue : NotEqual<Tuple, I + 1>()(iterators, end);
    }
};

template<class Tuple, std::size_t I>
struct NotEqual<Tuple, I, EnableIf<I == std::tuple_size<Decay<Tuple>>::value - 1>> {
    LZ_CONSTEXPR_CXX_17 bool operator()(const Tuple& iterators, const Tuple& end) const {
        return std::get<I>(iterators) != std::get<I>(end);
    }
};

template<class Tuple, std::size_t I, class = void>
struct Deref {
    LZ_CONSTEXPR_CXX_17 auto operator()(const Tuple& iterators, const Tuple& end) const -> decltype(*std::get<I>(iterators)) {
        return std::get<I>(iterators) != std::get<I>(end) ? *std::get<I>(iterators) : Deref<Tuple, I + 1>()(iterators, end);
    }
};

template<class Tuple, std::size_t I>
struct Deref<Tuple, I, EnableIf<I == std::tuple_size<Decay<Tuple>>::value - 1>> {
    LZ_CONSTEXPR_CXX_17 auto operator()(const Tuple& iterators, const Tuple&) const -> decltype(*std::get<I>(iterators)) {
        return *std::get<I>(iterators);
    }
};

template<class Tuple, std::size_t I>
struct MinusMinus {
    LZ_CONSTEXPR_CXX_17 void operator()(Tuple& iterators, const Tuple& begin, const Tuple& end) const {
        using lz::distance;
        using std::distance;
        if (distance(std::get<I>(begin), std::get<I>(iterators)) > 0) {
            --std::get<I>(iterators);
        }
        else {
            MinusMinus<Tuple, I - 1>()(iterators, begin, end);
        }
    }
};

template<class Tuple>
struct MinusMinus<Tuple, 0> {
    LZ_CONSTEXPR_CXX_17 void operator()(Tuple& iterators, const Tuple&, const Tuple&) const {
        --std::get<0>(iterators);
    }
};

template<class Tuple, std::size_t I>
struct MinIs {
    template<class DifferenceType>
    LZ_CONSTEXPR_CXX_17 void
    operator()(Tuple& iterators, const Tuple& begin, const Tuple& end, const DifferenceType offset) const {
        using lz::distance;
        using std::distance;
        using TupElem = TupleElement<I, Tuple>;
        const TupElem current = std::get<I>(iterators);
        const TupElem currentBegin = std::get<I>(begin);
        // Current is begin, move on to next iterator
        if (current == currentBegin) {
            MinIs<Tuple, I - 1>()(iterators, begin, end, offset);
        }
        else {
            const auto dist = static_cast<DifferenceType>(distance(std::get<I>(begin), std::get<I>(end)));
            if (dist <= offset) {
                std::get<I>(iterators) = std::get<I>(begin);
                MinIs<Tuple, I - 1>()(iterators, begin, end, dist == 0 ? DifferenceType{ 1 } : offset - dist);
            }
            else {
                std::get<I>(iterators) -= offset;
            }
        }
    }
};

template<class Tuple>
struct MinIs<Tuple, 0> {
    template<class DifferenceType>
    LZ_CONSTEXPR_CXX_17 void
    operator()(Tuple& iterators, const Tuple& begin, const Tuple& /*end*/, const DifferenceType offset) const {
        using TupElem = TupleElement<0, Tuple>;
        TupElem& current = std::get<0>(iterators);
        const TupElem currentBegin = std::get<0>(begin);
        static_cast<void>(currentBegin);
        current -= offset;
    }
};

template<class Tuple, std::size_t I, class = void>
struct PlusIs {
    template<class DifferenceType>
    LZ_CONSTEXPR_CXX_17 void operator()(Tuple& iterators, const Tuple& end, const DifferenceType offset) const {
        using lz::distance;
        using std::distance;
        using TupElem = TupleElement<I, Tuple>;
        TupElem& currentIterator = std::get<I>(iterators);
        const TupElem currentEnd = std::get<I>(end);
        const auto dist = static_cast<DifferenceType>(distance(currentIterator, currentEnd));
        if (dist > offset) {
            currentIterator += offset;
        }
        else {
            // Moves to end
            const auto toEndDistance = distance(currentIterator, currentEnd);
            currentIterator += toEndDistance;
            PlusIs<Tuple, I + 1>()(iterators, end, offset - dist);
        }
    }
};

template<class Tuple, std::size_t I>
struct PlusIs<Tuple, I, EnableIf<I == std::tuple_size<Decay<Tuple>>::value - 1>> {
    template<class DifferenceType>
    LZ_CONSTEXPR_CXX_17 void operator()(Tuple& /*iterators*/, const Tuple& /*end*/, const DifferenceType /*offset*/) const {
    }
};

template<LZ_CONCEPT_ITERATOR... Iterators>
class ConcatenateIterator {
    using IterTuple = std::tuple<Iterators...>;
    IterTuple _iterators{};
    IterTuple _begin{};
    IterTuple _end{};

    using FirstTupleIterator = std::iterator_traits<TupleElement<0, IterTuple>>;

  public:
    using value_type = typename FirstTupleIterator::value_type;
    using difference_type = typename std::common_type<DiffType<Iterators>...>::type;
    using reference = typename FirstTupleIterator::reference;
    using pointer = FakePointerProxy<reference>;
    using iterator_category = typename std::common_type<IterCat<Iterators>...>::type;

  private:
    template<std::size_t... I>
    LZ_CONSTEXPR_CXX_20 difference_type minus(IndexSequence<I...>, const ConcatenateIterator& other) const {
        const difference_type totals[] = { static_cast<difference_type>(std::get<I>(_iterators) -
                                                                        std::get<I>(other._iterators))... };
        return std::accumulate(std::begin(totals), std::end(totals), difference_type{ 0 });
    }

  public:
    LZ_CONSTEXPR_CXX_14 ConcatenateIterator(IterTuple iterators, IterTuple begin, IterTuple end) :
        _iterators(std::move(iterators)),
        _begin(std::move(begin)),
        _end(std::move(end)) {
    }

    constexpr ConcatenateIterator() = default;

    LZ_NODISCARD LZ_CONSTEXPR_CXX_17 reference operator*() const {
        return Deref<IterTuple, 0>()(_iterators, _end);
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_17 pointer operator->() const {
        return FakePointerProxy<decltype(**this)>(**this);
    }

    LZ_CONSTEXPR_CXX_17 ConcatenateIterator& operator++() {
        PlusPlus<IterTuple, 0>()(_iterators, _end);
        return *this;
    }

    LZ_CONSTEXPR_CXX_17 ConcatenateIterator operator++(int) {
        ConcatenateIterator tmp(*this);
        ++*this;
        return tmp;
    }

    LZ_CONSTEXPR_CXX_17 ConcatenateIterator& operator--() {
        MinusMinus<IterTuple, sizeof...(Iterators) - 1>()(_iterators, _begin, _end);
        return *this;
    }

    LZ_CONSTEXPR_CXX_17 ConcatenateIterator operator--(int) {
        ConcatenateIterator tmp(*this);
        ++*this;
        return tmp;
    }

    LZ_CONSTEXPR_CXX_17 ConcatenateIterator& operator+=(const difference_type offset) {
        PlusIs<IterTuple, 0>()(_iterators, _end, offset);
        return *this;
    }

    LZ_CONSTEXPR_CXX_17 ConcatenateIterator& operator-=(const difference_type offset) {
        MinIs<IterTuple, sizeof...(Iterators) - 1>()(_iterators, _begin, _end, offset);
        return *this;
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_17 ConcatenateIterator operator+(const difference_type offset) const {
        ConcatenateIterator tmp(*this);
        tmp += offset;
        return tmp;
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_17 ConcatenateIterator operator-(const difference_type offset) const {
        ConcatenateIterator tmp(*this);
        tmp -= offset;
        return tmp;
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 difference_type operator-(const ConcatenateIterator& other) const {
        return minus(MakeIndexSequence<sizeof...(Iterators)>(), other);
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_17 friend bool operator!=(const ConcatenateIterator& a, const ConcatenateIterator& b) {
        return NotEqual<IterTuple, 0>()(a._iterators, b._iterators);
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_17 friend bool operator==(const ConcatenateIterator& a, const ConcatenateIterator& b) {
        return !(a != b); // NOLINT
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_17 reference operator[](const difference_type offset) const {
        return *(*this + offset);
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 friend bool operator<(const ConcatenateIterator& a, const ConcatenateIterator& b) {
        return b - a > 0;
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 friend bool operator>(const ConcatenateIterator& a, const ConcatenateIterator& b) {
        return b < a;
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 friend bool operator<=(const ConcatenateIterator& a, const ConcatenateIterator& b) {
        return !(b < a); // NOLINT
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 friend bool operator>=(const ConcatenateIterator& a, const ConcatenateIterator& b) {
        return !(a < b); // NOLINT
    }
};

} // namespace internal
} // namespace lz

#endif // LZ_CONCATENATE_ITERATOR_HPP