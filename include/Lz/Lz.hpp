#pragma once

#ifndef LZ_LZ_HPP
#define LZ_LZ_HPP

#include "Lz/Algorithm.hpp"
#include "Lz/AnyView.hpp"
#include "Lz/CString.hpp"
#include "Lz/CartesianProduct.hpp"
#include "Lz/ChunkIf.hpp"
#include "Lz/Chunks.hpp"
#include "Lz/Concatenate.hpp"
#include "Lz/Enumerate.hpp"
#include "Lz/Except.hpp"
#include "Lz/Exclude.hpp"
#include "Lz/ExclusiveScan.hpp"
#include "Lz/Filter.hpp"
#include "Lz/Flatten.hpp"
#include "Lz/Generate.hpp"
#include "Lz/GenerateWhile.hpp"
#include "Lz/GroupBy.hpp"
#include "Lz/InclusiveScan.hpp"
#include "Lz/IterTools.hpp"
#include "Lz/Join.hpp"
#include "Lz/JoinWhere.hpp"
#include "Lz/Loop.hpp"
#include "Lz/Map.hpp"
#include "Lz/Random.hpp"
#include "Lz/Range.hpp"
#include "Lz/RegexSplit.hpp"
#include "Lz/Repeat.hpp"
#include "Lz/Rotate.hpp"
#include "Lz/Splitter.hpp"
#include "Lz/Take.hpp"
#include "Lz/TakeEvery.hpp"
#include "Lz/Unique.hpp"
#include "Lz/Zip.hpp"
#include "Lz/ZipLongest.hpp"

namespace lz {
#ifndef LZ_HAS_CXX_EXECUTION
namespace detail {
template<class Iterator, class T, class BinOp>
T accumulate(Iterator begin, Iterator end, T init, BinOp binOp) {
    while (begin != end) {
        init = binOp(std::move(init), *begin);
        ++begin;
    }
    return init;
}
} // namespace detail
#endif // LZ_HAS_CXX_EXECUTION

LZ_MODULE_EXPORT_SCOPE_BEGIN

template<class Iterator, class S>
class IterView;

// Start of group
/**
 * @addtogroup ItFns
 * @{
 */

/**
 * Converts an iterable into a IterView, where one can chain iterators using dot operator (.filter().map().select().any())
 * @param iterable The iterable to view over.
 * @return An iterator view object.
 */
template<LZ_CONCEPT_ITERABLE Iterable>
LZ_CONSTEXPR_CXX_20 IterView<IterT<Iterable>, SentinelT<Iterable>> chain(Iterable&& iterable) {
    return { detail::begin(std::forward<Iterable>(iterable)), detail::end(std::forward<Iterable>(iterable)) };
}

// End of group
/**
 * @}
 */

template<class Iterator, class S = Iterator>
class IterView final : public detail::BasicIterable<Iterator, S> {
    using Base = detail::BasicIterable<Iterator, S>;
    using Traits = std::iterator_traits<Iterator>;

public:
    using detail::BasicIterable<Iterator, S>::begin;
    using detail::BasicIterable<Iterator, S>::end;

    using iterator = Iterator;
    using const_iterator = iterator;
    using difference_type = typename Traits::difference_type;

    using value_type = typename Traits::value_type;
    using reference = typename Traits::reference;

    LZ_CONSTEXPR_CXX_20 IterView(Iterator begin, S end) : Base(std::move(begin), std::move(end)) {
    }

    LZ_CONSTEXPR_CXX_20 IterView() = default;

    template<class BinaryPredicate>
    void forEachWhile(BinaryPredicate&& predicate) {
        lz::forEachWhile(*this, std::forward<BinaryPredicate>(predicate));
    }

    //! See Concatenate.hpp for documentation.
    template<LZ_CONCEPT_ITERABLE... Iterables>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 IterView<detail::ConcatenateIterator<Iterator, IterT<Iterables>...>>
    concat(Iterables&&... iterables) const {
        return chain(lz::concat(*this, std::forward<Iterables>(iterables)...));
    }

    //! See Enumerate.hpp for documentation.
    template<LZ_CONCEPT_ARITHMETIC Arithmetic = int>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 IterView<detail::EnumerateIterator<Iterator, S, Arithmetic>>
    enumerate(const Arithmetic begin = 0) const {
        return chain(lz::enumerate(*this, begin));
    }

    //! See Join.hpp for documentation.
    LZ_NODISCARD IterView<detail::JoinIterator<Iterator>> join(std::string delimiter) const {
        return chain(lz::join(*this, std::move(delimiter)));
    }

    //! See Map.hpp for documentation
    template<class UnaryFunction>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 IterView<detail::MapIterator<Iterator, UnaryFunction>>
    map(UnaryFunction unaryFunction) const {
        return chain(lz::map(*this, std::move(unaryFunction)));
    }

    //! See TakeWhile.hpp for documentation.
    template<class UnaryPredicate>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 auto takeWhile(UnaryPredicate predicate) const
        -> decltype(chain(lz::takeWhile(*this, std::move(predicate)))) {
        return chain(lz::takeWhile(*this, std::move(predicate)));
    }

    //! See Take.hpp for documentation. Internally uses lz::take to take the amounts
    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 auto take(const difference_type amount) const
        -> IterView<detail::TakeNIterator<Iterator, S>, typename detail::TakeNIterator<Iterator, S>::Sentinel> {
        return chain(lz::take(*this, amount));
    }

    //! Drops the first amount elements from this iterator. Internally uses std::next to add an amount
    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 auto drop(const difference_type amount) const -> IterView<Iterator, S> {
        return chain(lz::drop(*this, amount));
    }

    //! Slices the iterator [from, to). Internally uses std::next and lz::take to create a slice.
    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 auto slice(const difference_type from, const difference_type to) const
        -> IterView<detail::TakeNIterator<Iterator, S>, typename detail::TakeNIterator<Iterator, S>::Sentinel> {
        return chain(lz::slice(*this, from, to));
    }

    //! See Take.hpp for documentation.
    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 IterView<detail::TakeEveryIterator<Iterator, detail::IsBidirectional<Iterator>::value>>
    takeEvery(const difference_type offset, const difference_type start = 0) const {
        return chain(lz::takeEvery(*this, offset, start));
    }

    //! See Chunks.hpp for documentation
    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 IterView<detail::ChunksIterator<Iterator, S>> chunks(const std::size_t chunkSize) const {
        return chain(lz::chunks(*this, chunkSize));
    }

    //! See Zip.hpp for documentation.
    template<LZ_CONCEPT_ITERABLE... Iterables>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 IterView<detail::ZipIterator<Iterator, IterT<Iterables>>...>
    zip(Iterables&&... iterables) const {
        return chain(lz::zip(*this, std::forward<Iterables>(iterables)...));
    }

    //! See IterTools.hpp `zipWith` for documentation
    template<class Fn, class... Iterables>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 auto zipWith(Fn fn, Iterables&&... iterables) const
        -> IterView<decltype(std::begin(lz::zipWith(std::move(fn), *this, std::forward<Iterables>(iterables)...)))> {
        return chain(lz::zipWith(std::move(fn), *this, std::forward<Iterables>(iterables)...));
    }

    //! See IterTools.hpp `as` for documentation.
    template<class T>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 IterView<detail::MapIterator<Iterator, detail::ConvertFn<T>>> as() const {
        return chain(lz::as<T>(*this));
    }

    //! See IterTools.hpp `reverse` for documentation.
    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 IterView<std::reverse_iterator<Iterator>> reverse() const {
        return chain(lz::reverse(*this));
    }

    //! See IterTools.hpp `reverse` for documentation.
    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 IterView<detail::ZipIterator<Iterator, Iterator>> pairwise() const {
        return chain(lz::pairwise(*this));
    }

    // clang-format off

    //! See CartesianProduct.hpp for documentation
    template<class... Iterables>
    LZ_NODISCARD
    LZ_CONSTEXPR_CXX_20 IterView<detail::CartesianProductIterator<
        std::tuple<Iterator, IterT<Iterables>...>,
        std::tuple<S, SentinelT<Iterables>...>>
    >
    cartesian(Iterables&&... iterables) const {
        return chain(lz::cartesian(*this, std::forward<Iterables>(iterables)...));
    }

    // clang-format on

    //! See Flatten.hpp for documentation
    template<int N = lz::detail::CountDims<std::iterator_traits<Iterator>>::value - 1>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 IterView<detail::FlattenIterator<Iterator, S, N>> flatten() const {
        return chain(lz::flatten(*this));
    }

    //! See Loop.hpp for documentation
    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 IterView<detail::LoopIterator<Iterator>> loop() const {
        return chain(lz::loop(*this));
    }

    //! See Exclude.hpp for documentation.
    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 auto exclude(const difference_type from, const difference_type to) const
        -> decltype(chain(std::declval<Exclude<Iterator, S>>())) {
        return chain(lz::exclude(*this, from, to));
    }

    // clang-format off
    //! See InclusiveScan.hpp for documentation.
    template<class T = value_type, class BinaryOp = MAKE_BIN_OP(std::plus, ValueType<iterator>)>
    LZ_NODISCARD
    LZ_CONSTEXPR_CXX_20 IterView<detail::InclusiveScanIterator<Iterator, detail::Decay<T>, detail::Decay<BinaryOp>>>
    iScan(T&& init = {}, BinaryOp&& binaryOp = {}) const {
        return chain(lz::iScan(*this, std::forward<T>(init), std::forward<BinaryOp>(binaryOp)));
    }

    //! See ExclusiveScan.hpp for documentation.
    template<class T = value_type, class BinaryOp = MAKE_BIN_OP(std::plus, ValueType<iterator>)>
    LZ_NODISCARD
    LZ_CONSTEXPR_CXX_20 IterView<detail::ExclusiveScanIterator<Iterator, S, detail::Decay<T>, detail::Decay<BinaryOp>>>
    eScan(T&& init = {}, BinaryOp&& binaryOp = {}) const {
        return chain(lz::eScan(*this, std::forward<T>(init), std::forward<BinaryOp>(binaryOp)));
    }
    // clang-format on

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 IterView<detail::RotateIterator<Iterator>> rotate(iterator start) const {
        return chain(lz::rotate(std::move(start), this->begin(), this->end()));
    }

    //! See Algorithm.hpp `hasOne` for documentation.
    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 bool hasOne() const {
        return lz::hasOne(*this);
    }

    //! See Algorithm.hpp `hasMany` for documentation.
    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 bool hasMany() const {
        return lz::hasMany(*this);
    }

    //! See Algorithm.hpp `frontOr` for documentation.
    template<class T>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 value_type frontOr(const T& defaultValue) const {
        return lz::frontOr(*this, defaultValue);
    }

    //! See Algorithm.hpp `backOr` for documentation.
    template<class T>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 value_type backOr(const T& defaultValue) const {
        return lz::backOr(*this, defaultValue);
    }

    //! See Filter.hpp for documentation
    template<class UnaryPredicate>
    IterView<detail::FilterIterator<Iterator, S, UnaryPredicate>> filter(UnaryPredicate predicate) const {
        return chain(lz::filter(*this, std::move(predicate)));
    }

    //! See Except.hpp for documentation
    template<class IterableToExcept, class Compare = std::less<value_type>>
    IterView<detail::ExceptIterator<Iterator, S, IterT<IterableToExcept>, SentinelT<IterableToExcept>, Compare>>
    except(IterableToExcept&& toExcept, Compare compare = {}) const {
        return chain(lz::except(*this, toExcept, std::move(compare)));
    }

    //! See Unique.hpp for documentation
    template<class Compare = std::less<value_type>>
    IterView<detail::UniqueIterator<Iterator, Compare>> unique(Compare compare = {}) const {
        return chain(lz::unique(*this, std::move(compare)));
    }

    //! See ChunkIf.hpp for documentation
    template<class UnaryPredicate>
    IterView<detail::ChunkIfIterator<Iterator, S, UnaryPredicate>> chunkIf(UnaryPredicate predicate) const {
        return chain(lz::chunkIf(*this, std::move(predicate)));
    }

    //! See IterTools.hpp `filterMap` for documentation
    template<class UnaryMapFunc, class UnaryFilterFunc>
    IterView<detail::MapIterator<detail::FilterIterator<Iterator, S, UnaryFilterFunc>, UnaryMapFunc>>
    filterMap(UnaryFilterFunc filterFunc, UnaryMapFunc mapFunc) const {
        return chain(lz::filterMap(*this, std::move(filterFunc), std::move(mapFunc)));
    }

    //! See IterTools.hpp `select` for documentation
    template<class SelectorIterable>
    auto select(SelectorIterable&& selectors) const
        -> decltype(chain(lz::select(*this, std::forward<SelectorIterable>(selectors)))) {
        return chain(lz::select(*this, std::forward<SelectorIterable>(selectors)));
    }

    //! See JoinWhere.hpp for documentation
    template<class IterableB, class SelectorA, class SelectorB, class ResultSelector>
    LZ_CONSTEXPR_CXX_20 IterView<detail::JoinWhereIterator<Iterator, IterT<IterableB>, SelectorA, SelectorB, ResultSelector>>
    joinWhere(IterableB&& iterableB, SelectorA a, SelectorB b, ResultSelector resultSelector) const {
        return chain(lz::joinWhere(*this, iterableB, std::move(a), std::move(b), std::move(resultSelector)));
    }

    //! See Take.hpp for documentation
    template<class UnaryPredicate>
    IterView<Iterator> dropWhile(UnaryPredicate predicate) const {
        return chain(lz::dropWhile(*this, std::move(predicate)));
    }

    //! See GroupBy.hpp for documentation
    template<class Comparer = std::equal_to<value_type>>
    IterView<detail::GroupByIterator<Iterator, Comparer>> groupBy(Comparer comparer = {}) const {
        return chain(lz::groupBy(*this, std::move(comparer)));
    }

    //! See IterTools.hpp `trim` for documentation
    template<class UnaryPredicateFirst, class UnaryPredicateLast>
    auto trim(UnaryPredicateFirst first, UnaryPredicateLast last) const
        -> decltype(chain(lz::trim(*this, std::move(first), std::move(last)))) {
        return chain(lz::trim(*this, std::move(first), std::move(last)));
    }

    //! See Algorithm.hpp `findFirstOrDefault` for documentation
    template<class T, class U>
    value_type findFirstOrDefault(T&& toFind, U&& defaultValue) const {
        return lz::findFirstOrDefault(*this, toFind, defaultValue);
    }

    //! See Algorithm.hpp `findFirstOrDefaultIf` for documentation
    template<class UnaryPredicate, class U>
    value_type findFirstOrDefaultIf(UnaryPredicate predicate, U&& defaultValue) const {
        return lz::findFirstOrDefaultIf(*this, std::move(predicate), defaultValue);
    }

    // //! See Algorithm.hpp `findLastOrDefault` for documentation
    // template<class T, class U>
    // value_type findLastOrDefault(T&& toFind, U&& defaultValue) const {
    //     return lz::findLastOrDefault(*this, toFind, defaultValue);
    // }

    // //! See Algorithm.hpp `findLastOrDefaultIf` for documentation
    // template<class UnaryPredicate, class U>
    // value_type findLastOrDefaultIf(UnaryPredicate predicate, U&& defaultValue) const {
    //     return lz::findLastOrDefaultIf(*this, std::move(predicate), defaultValue);
    // }

    // //! See Algorithm.hpp `indexOf` for documentation
    // template<class T>
    // std::size_t indexOf(const T& value) const {
    //     return lz::indexOf(*this, value);
    // }

    // //! See Algorithm.hpp `indexOfIf` for documentation
    // template<class UnaryPredicate>
    // std::size_t indexOfIf(UnaryPredicate predicate) const {
    //     return lz::indexOfIf(*this, std::move(predicate));
    // }

    // //! See Algorithm.hpp `contains` for documentation
    // template<class T>
    // bool contains(const T& value) const {
    //     return lz::contains(*this, value);
    // }

    // //! See Algorithm.hpp `containsIf` for documentation
    // template<class UnaryPredicate>
    // bool containsIf(UnaryPredicate predicate) const {
    //     return lz::containsIf(*this, std::move(predicate));
    // }

    /**
     * Checks if two views/iterables are equal.
     * @param other The other view/iterable to compare with
     * @param compare The comparer, default is `operator==`
     * @return
     */
    template<class Iterable, class BinaryPredicate = MAKE_BIN_OP(std::equal_to, value_type)>
    bool equal(const Iterable& other, BinaryPredicate compare = {}) const {
        return lz::equal(*this, other, std::move(compare));
    }

    // /**
    //  * Checks if this starts with an other iterable.
    //  * @param iterable The other iterable to compare with
    //  * @param compare The comparer (operator== is default)
    //  * @param execution The execution policy.
    //  * @return True if this starts with `iterable`, false otherwise.
    //  */
    // template<class Iterable, class BinaryPredicate = MAKE_BIN_OP(std::equal_to, value_type)>
    // bool startsWith(const Iterable& iterable, BinaryPredicate compare = {}) const {
    //     return lz::startsWith(*this, iterable, std::move(compare));
    // }

    // /**
    //  * Checks if this ends with an other iterable.
    //  * @param iterable The other iterable to compare with
    //  * @param compare The comparer (operator== is default)
    //  * @param execution The execution policy.
    //  * @return True if this ends with `iterable`, false otherwise.
    //  */
    // template<class Iterable, class BinaryPredicate = MAKE_BIN_OP(std::equal_to, value_type)>
    // bool endsWith(const Iterable& iterable, BinaryPredicate compare = {}) const {
    //     return lz::endsWith(*this, iterable, std::move(compare));
    // }

    /**
     * Iterates over the sequence generated so far.
     * @param func A function to apply over each element. Must have the following signature: `void func(value_type)`
     */
    template<class UnaryFunc>
    IterView<Iterator>& forEach(UnaryFunc func) {
        lz::forEach(Base::begin(), Base::end(), std::move(func));
        return *this;
    }

    /**
     * Performs a left fold with as starting point `init`. Can be used to for e.g. sum all values. For this use:
     * `[](value_type init, value_type next) const { return std::move(init) + value_type; }`
     * @param init The starting value
     * @param function A binary function with the following signature `value_type func(value_type init, value_type element)`
     */
    template<class T, class BinaryFunction>
    T foldl(T&& init, BinaryFunction function) const {
        return detail::accumulate(Base::begin(), Base::end(), std::forward<T>(init), std::move(function));
    }

    /**
     * Performs a right fold with as starting point `init`. Can be used to for e.g. sum all values. For this use:
     * `[](value_type init, value_type next) const { return std::move(init) + value_type; }`
     * @param init The starting value
     * @param function A binary function with the following signature `value_type func(value_type init, value_type element)`
     */
    template<class T, class BinaryFunction>
    T foldr(T&& init, BinaryFunction function) const {
        auto reverseView = reverse();
        return detail::accumulate(detail::begin(std::move(reverseView)), detail::end(std::move(reverseView)),
                                  std::forward<T>(init), std::move(function));
    }

    /**
     * Sums the sequence generated so far.
     */
    value_type sum() const {
#ifdef LZ_HAS_CXX_11
        return this->foldl(value_type(), [](value_type init, const value_type& val) { return std::move(init) + val; });
#else
        return this->foldl(value_type(), std::plus<>());
#endif // LZ_HAS_CXX_11
    }

    /**
     * Gets the max value of the current iterator view.
     * @param cmp The comparer. operator< is assumed by default.
     * @return The max element.
     */
    template<class Compare = MAKE_BIN_OP(std::less, value_type)>
    reference max(Compare cmp = {}) const {
        LZ_ASSERT(!lz::empty(*this), "sequence cannot be empty in order to get max element");
        return *lz::maxElement(Base::begin(), Base::end(), std::move(cmp));
    }

    /**
     * Gets the min value of the current iterator view.
     * @param cmp The comparer. operator< is assumed by default.
     * @return The min element.
     */
    template<class Compare = MAKE_BIN_OP(std::less, value_type)>
    reference min(Compare cmp = {}) const {
        LZ_ASSERT(!lz::empty(*this), "sequence cannot be empty in order to get min element");
        return *lz::minElement(Base::begin(), Base::end(), std::move(cmp));
    }

    //! See Algorithm.hpp for documentation
    template<class BinaryOp = MAKE_BIN_OP(std::plus, value_type)>
    double mean(BinaryOp binOp = {}) const {
        return lz::mean(*this, std::move(binOp));
    }

    /**
     * Checks if all of the elements meet the condition `predicate`. `predicate` must return a bool and take a `value_type` as
     * parameter.
     * @param predicate The function that checks if an element meets a certain condition.
     */
    template<class UnaryPredicate>
    bool all(UnaryPredicate predicate) const {
        return std::all_of(Base::begin(), Base::end(), std::move(predicate));
    }

    /**
     * Checks if any of the elements meet the condition `predicate`. `predicate` must return a bool and take a `value_type` as
     * parameter.
     * @param predicate The function that checks if an element meets a certain condition.
     */
    template<class UnaryPredicate>
    bool any(UnaryPredicate predicate) const {
        return std::any_of(Base::begin(), Base::end(), std::move(predicate));
    }

    /**
     * Checks if none of the elements meet the condition `predicate`. `predicate` must return a bool and take a `value_type` as
     * parameter.
     * @param predicate The function that checks if an element meets a certain condition.
     */
    template<class UnaryPredicate>
    bool none(UnaryPredicate predicate) const {
        return std::none_of(Base::begin(), Base::end(), std::move(predicate));
    }

    /**
     * Counts how many occurrences of `value` are in this.
     * @param value The value to count
     * @return The amount of counted elements equal to `value`.
     */
    template<class T>
    difference_type count(const T& value) const {
        return std::count(Base::begin(), Base::end(), value);
    }

    /**
     * Counts how many occurrences times the unary predicate returns true.
     * @param predicate The function predicate that must return a bool.
     * @return The amount of counted elements.
     */
    template<class UnaryPredicate>
    difference_type countIf(UnaryPredicate predicate) const {
        return std::count_if(Base::begin(), Base::end(), std::move(predicate));
    }

    /**
     * Sorts the sequence with the default (operator<) comparer.
     * @return A reference to this.
     */
#ifdef LZ_HAS_CXX_11
    template<class Comparer = std::less<value_type>>
#else
    template<class Comparer = std::less<>>
#endif // LZ_HAS_CXX_11
    IterView<Iterator>& sort(Comparer comparer = {}) {
        std::sort(Base::begin(), Base::end(), std::move(comparer));
        return *this;
    }

    /**
     * Checks whether the sequence is sorted, using the standard (operator<) compare.
     * @return True if the sequence is sorted given by the `predicate` false otherwise.
     */
#ifdef LZ_HAS_CXX_11
    template<class Comparer = std::less<value_type>>
#else
    template<class Comparer = std::less<>>
#endif // LZ_HAS_CXX_11
    bool isSorted(Comparer comparer = {}) const {
        return std::is_sorted(Base::begin(), Base::end(), std::move(comparer));
    }
};
} // namespace lz

LZ_MODULE_EXPORT_SCOPE_END

#endif // LZ_LZ_HPP