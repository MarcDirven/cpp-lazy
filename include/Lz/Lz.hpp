#pragma once

#ifndef LZ_LZ_HPP
#    define LZ_LZ_HPP

#    include "Lz/CartesianProduct.hpp"
#    include "Lz/ChunkIf.hpp"
#    include "Lz/Chunks.hpp"
#    include "Lz/Enumerate.hpp"
#    include "Lz/Except.hpp"
#    include "Lz/Exclude.hpp"
#    include "Lz/Flatten.hpp"
#    include "Lz/FunctionTools.hpp"
#    include "Lz/Generate.hpp"
#    include "Lz/GroupBy.hpp"
#    include "Lz/JoinWhere.hpp"
#    include "Lz/Loop.hpp"
#    include "Lz/Random.hpp"
#    include "Lz/Range.hpp"
#    include "Lz/Repeat.hpp"
#    include "Lz/Rotate.hpp"
#    include "Lz/TakeEvery.hpp"
#    include "Lz/Unique.hpp"
// Function tools includes:
// Concatenate.hpp
// Filter.hpp
// Join.hpp
// Map.hpp
// StringSplitter.hpp
// Take.hpp
// Zip.hpp

namespace lz {
#    ifndef LZ_HAS_CXX_EXECUTION
namespace internal {
template<class Iterator, class T, class BinOp>
T accumulate(Iterator begin, Iterator end, T init, BinOp binOp) {
    while (begin != end) {
        init = binOp(std::move(init), *begin);
        ++begin;
    }
    return init;
}
} // namespace internal
#    endif // LZ_HAS_CXX_EXECUTION

template<LZ_CONCEPT_ITERATOR Iterator>
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
template<LZ_CONCEPT_ITERATOR Iterator>
LZ_CONSTEXPR_CXX_20 IterView<Iterator> toIterRange(Iterator begin, Iterator end) {
    return lz::IterView<Iterator>(std::move(begin), std::move(end));
}

/**
 * Converts an iterable into a IterView, where one can chain iterators using dot operator (.filter().map().select().any())
 * @param iterable The iterable to view over.
 * @return An iterator view object.
 */
template<LZ_CONCEPT_ITERABLE Iterable>
LZ_CONSTEXPR_CXX_20 IterView<internal::IterTypeFromIterable<Iterable>> toIter(Iterable&& iterable) {
    return toIterRange(internal::begin(std::forward<Iterable>(iterable)), internal::end(std::forward<Iterable>(iterable)));
}

// End of group
/**
 * @}
 */

template<LZ_CONCEPT_ITERATOR Iterator>
class IterView final : public internal::BasicIteratorView<Iterator> {
    using Base = internal::BasicIteratorView<Iterator>;
    using Traits = std::iterator_traits<Iterator>;

public:
    using iterator = Iterator;
    using const_iterator = iterator;
    using difference_type = typename Traits::difference_type;

    using value_type = typename Traits::value_type;
    using reference = typename Traits::reference;

    LZ_CONSTEXPR_CXX_20 IterView(Iterator begin, Iterator end) : Base(std::move(begin), std::move(end)) {
    }

    LZ_CONSTEXPR_CXX_20 IterView() = default;

    //! See Concatenate.hpp for documentation.
    template<LZ_CONCEPT_ITERABLE... Iterables>
    LZ_NODISCARD
        LZ_CONSTEXPR_CXX_20 IterView<internal::ConcatenateIterator<Iterator, internal::IterTypeFromIterable<Iterables>...>>
        concat(Iterables&&... iterables) const {
        return toIter(lz::concat(*this, std::forward<Iterables>(iterables)...));
    }

    //! See Enumerate.hpp for documentation.
    template<LZ_CONCEPT_ARITHMETIC Arithmetic = int>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 IterView<internal::EnumerateIterator<Iterator, Arithmetic>>
    enumerate(const Arithmetic begin = 0) const {
        return toIter(lz::enumerate(*this, begin));
    }

    //! See Exclude.hpp for documentation.
    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 IterView<internal::ExcludeIterator<Iterator>>
    exclude(const difference_type from, const difference_type to) const {
        return toIter(lz::exclude(*this, from, to));
    }

    //! See Join.hpp for documentation.
    LZ_NODISCARD IterView<internal::JoinIterator<Iterator>> join(std::string delimiter) const {
        return toIter(lz::join(*this, std::move(delimiter)));
    }

    //! See Map.hpp for documentation
    template<class UnaryFunction>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 IterView<internal::MapIterator<Iterator, UnaryFunction>>
    map(UnaryFunction unaryFunction) const {
        return toIter(lz::map(*this, std::move(unaryFunction)));
    }

    //! See Take.hpp for documentation.
    template<class UnaryPredicate>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 IterView<Iterator> takeWhile(UnaryPredicate predicate) const {
        return toIter(lz::takeWhile(*this, std::move(predicate)));
    }

    //! See Take.hpp for documentation.
    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 IterView<Iterator> take(const difference_type amount) const {
        return toIter(lz::take(*this, amount));
    }

    //! See Take.hpp for documentation.
    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 IterView<Iterator> drop(const difference_type amount) const {
        return toIter(lz::drop(*this, amount));
    }

    //! See Take.hpp for documentation.
    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 IterView<Iterator> slice(const difference_type from, const difference_type to) const {
        return toIter(lz::slice(*this, from, to));
    }

    //! See Take.hpp for documentation.
    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 IterView<internal::TakeEveryIterator<Iterator, internal::IsBidirectional<Iterator>::value>>
    takeEvery(const difference_type offset, const difference_type start = 0) const {
        return toIter(lz::takeEvery(*this, offset, start));
    }

    //! See Chunks.hpp for documentation
    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 IterView<internal::ChunksIterator<Iterator>> chunks(const std::size_t chunkSize) const {
        return toIter(lz::chunks(*this, chunkSize));
    }

    //! See Zip.hpp for documentation.
    template<LZ_CONCEPT_ITERABLE... Iterables>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 IterView<internal::ZipIterator<Iterator, internal::IterTypeFromIterable<Iterables>>...>
    zip(Iterables&&... iterables) const {
        return toIter(lz::zip(*this, std::forward<Iterables>(iterables)...));
    }

    //! See FunctionTools.hpp `zipWith` for documentation
    template<class Fn, class... Iterables>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 auto zipWith(Fn fn, Iterables&&... iterables) const
        -> IterView<decltype(std::begin(lz::zipWith(std::move(fn), *this, std::forward<Iterables>(iterables)...)))> {
        return toIter(lz::zipWith(std::move(fn), *this, std::forward<Iterables>(iterables)...));
    }

    //! See FunctionTools.hpp `as` for documentation.
    template<class T>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 IterView<internal::MapIterator<Iterator, internal::ConvertFn<T>>> as() const {
        return toIter(lz::as<T>(*this));
    }

    //! See FunctionTools.hpp `reverse` for documentation.
    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 IterView<std::reverse_iterator<Iterator>> reverse() const {
        return toIter(lz::reverse(*this));
    }

    //! See FunctionTools.hpp `reverse` for documentation.
    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 IterView<internal::ZipIterator<Iterator, Iterator>> pairwise() const {
        return toIter(lz::pairwise(*this));
    }

    // clang-format off

    //! See CartesianProduct.hpp for documentation
    template<class... Iterables>
    LZ_NODISCARD
    LZ_CONSTEXPR_CXX_20 IterView<internal::CartesianProductIterator<Iterator, internal::IterTypeFromIterable<Iterables>...>>
    cartesian(Iterables&&... iterables) const {
        return toIter(lz::cartesian(*this, std::forward<Iterables>(iterables)...));
    }

    // clang-format on

    //! See Flatten.hpp for documentation
    template<int N = lz::internal::CountDims<std::iterator_traits<Iterator>>::value - 1>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 IterView<internal::FlattenIterator<Iterator, N>> flatten() const {
        return toIter(lz::flatten(*this));
    }

    template<class Iterable>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 IterView<internal::LoopIterator<Iterator>> loop() const {
        return toIter(lz::loop(*this));
    }

    template<class Iterable>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 IterView<internal::RotateIterator<Iterator, internal::IsRandomAccess<Iterator>::value>>
    rotate(const internal::DiffType<iterator> start) const {
        return toIter(lz::rotate(*this, start));
    }

    //! See FunctionTools.hpp `hasOne` for documentation.
    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 bool hasOne() const {
        return lz::hasOne(*this);
    }

    //! See FunctionTools.hpp `hasMany` for documentation.
    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 bool hasMany() const {
        return lz::hasMany(*this);
    }

    //! See FunctionTools.hpp `front` for documentation.
    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 reference front() const {
        return lz::front(*this);
    }

    //! See FunctionTools.hpp `last` for documentation.
    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 reference back() const {
        return lz::back(*this);
    }

    //! See FunctionTools.hpp `frontOr` for documentation.
    template<class T>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 value_type frontOr(const T& defaultValue) const {
        return lz::frontOr(*this, defaultValue);
    }

    //! See FunctionTools.hpp `backOr` for documentation.
    template<class T>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 value_type backOr(const T& defaultValue) const {
        return lz::backOr(*this, defaultValue);
    }

#    ifdef LZ_HAS_EXECUTION
    //! See Filter.hpp for documentation.
    template<class UnaryPredicate, class Execution = std::execution::sequenced_policy>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 IterView<internal::FilterIterator<Iterator, UnaryPredicate, Execution>>
    filter(UnaryPredicate predicate, Execution execution = std::execution::seq) const {
        return toIter(lz::filter(*this, std::move(predicate), execution));
    }

    //! See Except.hpp for documentation.
    template<class IterableToExcept, class Execution = std::execution::sequenced_policy, class Compare = std::less<>>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_20
        IterView<internal::ExceptIterator<Iterator, internal::IterTypeFromIterable<IterableToExcept>, Compare, Execution>>
        except(IterableToExcept&& toExcept, Compare compare = {}, Execution execution = std::execution::seq) const {
        return toIter(lz::except(*this, toExcept, std::move(compare), execution));
    }

    //! See Unique.hpp for documentation.
    template<class Execution = std::execution::sequenced_policy, class Compare = std::less<>>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 IterView<internal::UniqueIterator<Execution, Iterator, Compare>>
    unique(Compare compare = {}, Execution execution = std::execution::seq) const {
        return toIter(lz::unique(*this, std::move(compare), execution));
    }

    //! See ChunkIf.hpp for documentation
    template<class UnaryPredicate, class Execution = std::execution::sequenced_policy>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 IterView<internal::ChunkIfIterator<Iterator, UnaryPredicate, Execution>>
    chunkIf(UnaryPredicate predicate, Execution execution = std::execution::seq) const {
        return toIter(lz::chunkIf(*this, std::move(predicate), execution));
    }

    //! See FunctionTools.hpp `filterMap` for documentation.
    template<class UnaryMapFunc, class UnaryFilterFunc, class Execution = std::execution::sequenced_policy>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_20
        IterView<internal::MapIterator<internal::FilterIterator<Iterator, UnaryFilterFunc, Execution>, UnaryMapFunc>>
        filterMap(UnaryFilterFunc filterFunc, UnaryMapFunc mapFunc, Execution execution = std::execution::seq) const {
        return toIter(lz::filterMap(*this, std::move(filterFunc), std::move(mapFunc), execution));
    }

    //! See FunctionTools.hpp `select` for documentation.
    template<class SelectorIterable, class Execution = std::execution::sequenced_policy>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 auto select(SelectorIterable&& selectors, Execution execution = std::execution::seq) const {
        return toIter(lz::select(*this, std::forward<SelectorIterable>(selectors), execution));
    }

    //! See JoinWhere.hpp for documentation
    template<class IterableB, class SelectorA, class SelectorB, class ResultSelector,
             class Execution = std::execution::sequenced_policy>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 IterView<internal::JoinWhereIterator<Iterator, internal::IterTypeFromIterable<IterableB>,
                                                                          SelectorA, SelectorB, ResultSelector, Execution>>
    joinWhere(IterableB&& iterableB, SelectorA a, SelectorB b, ResultSelector resultSelector,
              Execution execution = std::execution::seq) const {
        return toIter(lz::joinWhere(*this, iterableB, std::move(a), std::move(b), std::move(resultSelector), execution));
    }

    //! See Take.hpp for documentation
    template<class UnaryPredicate, class Execution = std::execution::sequenced_policy>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 IterView<Iterator>
    dropWhile(UnaryPredicate predicate, Execution execution = std::execution::seq) const {
        return toIter(lz::dropWhile(*this, std::move(predicate), execution));
    }

    //! See GroupBy.hpp for documentation
    template<class Comparer = std::equal_to<>, class Execution = std::execution::sequenced_policy>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 IterView<internal::GroupByIterator<Iterator, Comparer, Execution>>
    groupBy(Comparer comparer = {}, Execution execution = std::execution::seq) const {
        return toIter(lz::groupBy(*this, std::move(comparer), execution));
    }

    //! See FunctionTools.hpp `trim` for documentation
    template<class UnaryPredicateFirst, class UnaryPredicateLast, class Execution = std::execution::sequenced_policy>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 auto
    trim(UnaryPredicateFirst first, UnaryPredicateLast last, Execution execution = std::execution::seq) const
        -> decltype(toIter(lz::trim(*this, std::move(first), std::move(last), execution))) {
        return toIter(lz::trim(*this, std::move(first), std::move(last), execution));
    }

    //! See FunctionTools.hpp `findFirstOrDefault` for documentation.
    template<class T, class U, class Execution = std::execution::sequenced_policy>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 value_type findFirstOrDefault(const T& toFind, const U& defaultValue,
                                                                   Execution execution = std::execution::seq) const {
        return lz::findFirstOrDefault(*this, toFind, defaultValue, execution);
    }

    //! See FunctionTools.hpp `findFirstOrDefaultIf` for documentation.
    template<class UnaryPredicate, class U, class Execution = std::execution::sequenced_policy>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 value_type findFirstOrDefaultIf(UnaryPredicate predicate, const U& defaultValue,
                                                                     Execution execution = std::execution::seq) const {
        return lz::findFirstOrDefaultIf(*this, std::move(predicate), defaultValue, execution);
    }

    //! See FunctionTools.hpp `findLastOrDefault` for documentation.
    template<class T, class U, class Execution = std::execution::sequenced_policy>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 value_type findLastOrDefault(const T& toFind, const U& defaultValue,
                                                                  Execution execution = std::execution::seq) const {
        return lz::findLastOrDefault(*this, toFind, defaultValue, execution);
    }

    //! See FunctionTools.hpp `findLastOrDefaultIf` for documentation.
    template<class UnaryPredicate, class U, class Execution = std::execution::sequenced_policy>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 value_type findLastOrDefaultIf(UnaryPredicate predicate, const U& defaultValue,
                                                                    Execution execution = std::execution::seq) const {
        return lz::findLastOrDefaultIf(*this, std::move(predicate), defaultValue, execution);
    }

    //! See FunctionTools.hpp `indexOf` for documentation.
    template<class T, class Execution = std::execution::sequenced_policy>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 difference_type indexOf(const T& value, Execution execution = std::execution::seq) const {
        return lz::indexOf(*this, value, execution);
    }

    //! See FunctionTools.hpp `indexOfIf` for documentation.
    template<class UnaryPredicate, class Execution = std::execution::sequenced_policy>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 difference_type indexOfIf(UnaryPredicate predicate,
                                                               Execution execution = std::execution::seq) const {
        return lz::indexOfIf(*this, std::move(predicate), execution);
    }

    //! See FunctionTools.hpp `contains` for documentation.
    template<class T, class Execution = std::execution::sequenced_policy>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 bool contains(const T& value, Execution execution = std::execution::seq) const {
        return lz::contains(*this, value, execution);
    }

    //! See FunctionTools.hpp `containsIf` for documentation.
    template<class UnaryPredicate, class Execution = std::execution::sequenced_policy>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 bool containsIf(UnaryPredicate predicate, Execution execution = std::execution::seq) const {
        return lz::containsIf(*this, std::move(predicate), execution);
    }

    /**
     * Checks if two views/iterables are equal.
     * @param other The other view/iterable to compare with
     * @param compare The comparer, default is `operator==`
     * @param execution The execution policy. Must be one of `std::execution::*` tags.
     * @return
     */
    template<class Iterable, class BinaryCompare = std::equal_to<>, class Execution = std::execution::sequenced_policy>
    bool equal(const Iterable& other, BinaryCompare compare = {}, Execution execution = std::execution::seq) const {
        return lz::equal(*this, other, std::move(compare), execution);
    }

    template<class Iterable, class BinaryPredicate = std::equal_to<>, class Execution = std::execution::sequenced_policy>
    bool startsWith(const Iterable& iterable, BinaryPredicate compare = {}, Execution execution = std::execution::seq) const {
        return lz::startsWith(*this, iterable, std::move(compare), execution);
    }

    template<class Iterable, class BinaryPredicate = std::equal_to<>, class Execution = std::execution::sequenced_policy>
    bool endsWith(const Iterable& iterable, BinaryPredicate compare = {}, Execution execution = std::execution::seq) const {
        return lz::endsWith(*this, iterable, std::move(compare), execution);
    }

    /**
     * Iterates over the sequence generated so far.
     * @param func A function to apply over each element. Must have the following signature: `void func(value_type)`
     * @param execution The execution policy.
     */
    template<class UnaryFunc, class Execution = std::execution::sequenced_policy>
    LZ_CONSTEXPR_CXX_20 IterView<Iterator>& forEach(UnaryFunc func, Execution execution = std::execution::seq) {
        if constexpr (internal::checkForwardAndPolicies<Execution, Iterator>()) {
            static_cast<void>(execution);
            std::for_each(Base::begin(), Base::end(), std::move(func));
        }
        else {
            std::for_each(execution, Base::begin(), Base::end(), std::move(func));
        }
        return *this;
    }

    /**
     * Performs a left fold with as starting point `init`. Can be used to for e.g. sum all values. For this use:
     * `[](value_type init, value_type next) const { return init + value_type; }`
     * @param init The starting value
     * @param function A binary function with the following signature `value_type func(value_type init, value_type element)`
     */
    template<class T, class BinaryFunction, class Execution = std::execution::sequenced_policy>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 T foldl(T&& init, BinaryFunction function, Execution execution = std::execution::seq) const {
        if constexpr (internal::checkForwardAndPolicies<Execution, Iterator>()) {
            static_cast<void>(execution);
            return std::reduce(Base::begin(), Base::end(), std::forward<T>(init), std::move(function));
        }
        else {
            return std::reduce(execution, Base::begin(), Base::end(), std::forward<T>(init), std::move(function));
        }
    }

    /**
     * Performs a right fold with as starting point `init`. Can be used to for e.g. sum all values. For this use:
     * `[](value_type init, value_type next) const { return init + value_type; }`
     * @param init The starting value
     * @param function A binary function with the following signature `value_type func(value_type init, value_type element)`
     */
    template<class T, class BinaryFunction, class Execution = std::execution::sequenced_policy>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 T foldr(T&& init, BinaryFunction function, Execution execution = std::execution::seq) const {
        auto reverseView = reverse();
        if constexpr (internal::checkForwardAndPolicies<Execution, Iterator>()) {
            static_cast<void>(execution);
            return std::reduce(internal::begin(std::move(reverseView)), internal::end(std::move(reverseView)),
                               std::forward<T>(init), std::move(function));
        }
        else {
            return std::reduce(execution, internal::begin(std::move(reverseView)), internal::end(std::move(reverseView)),
                               std::forward<T>(init), std::move(function));
        }
    }

    /**
     * Sums the sequence generated so far.
     * @param execution The execution policy.
     */
    template<class Execution = std::execution::sequenced_policy>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 value_type sum(Execution execution = std::execution::seq) const {
        return this->foldl(value_type(), std::plus<>(), execution);
    }

    /**
     * Gets the min value of the current iterator view.
     * @param cmp The comparer. operator< is assumed by default.
     * @param execution The execution policy.
     * @return The min element.
     */
    template<class Compare = std::less<>, class Execution = std::execution::sequenced_policy>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 reference max(Compare cmp = {}, Execution execution = std::execution::seq) const {
        LZ_ASSERT(!lz::isEmpty(*this), "sequence cannot be empty in order to get max element");
        if constexpr (internal::checkForwardAndPolicies<Execution, Iterator>()) {
            static_cast<void>(execution);
            return *std::max_element(Base::begin(), Base::end(), std::move(cmp));
        }
        else {
            return *std::max_element(execution, Base::begin(), Base::end(), std::move(cmp));
        }
    }

    /**
     * Gets the min value of the current iterator view.
     * @param cmp The comparer. operator< is assumed by default.
     * @param execution The execution policy.
     * @return The min element.
     */
    template<class Compare = std::less<>, class Execution = std::execution::sequenced_policy>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 reference min(Compare cmp = {}, Execution execution = std::execution::seq) const {
        LZ_ASSERT(!lz::isEmpty(*this), "sequence cannot be empty in order to get min element");
        if constexpr (internal::checkForwardAndPolicies<Execution, Iterator>()) {
            static_cast<void>(execution);
            return *std::min_element(Base::begin(), Base::end(), std::move(cmp));
        }
        else {
            return *std::min_element(execution, Base::begin(), Base::end(), std::move(cmp));
        }
    }

    //! See FunctionTools.hpp for documentation
    template<class BinaryOp = std::plus<>, class Execution = std::execution::sequenced_policy>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 double mean(BinaryOp binOp = {}, Execution execution = std::execution::seq) const {
        return lz::mean(*this, std::move(binOp), execution);
    }

    //! See FunctionTools.hpp for documentation
    template<class Compare = std::less<>, class Execution = std::execution::sequenced_policy>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 double median(Compare compare = {}, Execution execution = std::execution::seq) const {
        return lz::median(*this, std::move(compare), execution);
    }

    /**
     * Checks if all of the elements meet the condition `predicate`. `predicate` must return a bool and take a `value_type` as
     * parameter.
     * @param predicate The function that checks if an element meets a certain condition.
     * @param execution The execution policy.
     */
    template<class UnaryPredicate, class Execution = std::execution::sequenced_policy>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 bool all(UnaryPredicate predicate, Execution execution = std::execution::seq) const {
        if constexpr (internal::checkForwardAndPolicies<Execution, Iterator>()) {
            static_cast<void>(execution);
            return std::all_of(Base::begin(), Base::end(), std::move(predicate));
        }
        else {
            return std::all_of(execution, Base::begin(), Base::end(), std::move(predicate));
        }
    }

    /**
     * Checks if any of the elements meet the condition `predicate`. `predicate` must return a bool and take a `value_type` as
     * parameter.
     * @param predicate The function that checks if an element meets a certain condition.
     * @param execution The execution policy.
     */
    template<class UnaryPredicate, class Execution = std::execution::sequenced_policy>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 bool any(UnaryPredicate predicate, Execution execution = std::execution::seq) const {
        if constexpr (internal::checkForwardAndPolicies<Execution, Iterator>()) {
            static_cast<void>(execution);
            return std::any_of(Base::begin(), Base::end(), std::move(predicate));
        }
        else {
            return std::any_of(execution, Base::begin(), Base::end(), std::move(predicate));
        }
    }

    /**
     * Checks if none of the elements meet the condition `predicate`. `predicate` must return a bool and take a `value_type` as
     * parameter.
     * @param predicate The function that checks if an element meets a certain condition.
     * @param execution The execution policy.
     */
    template<class UnaryPredicate, class Execution = std::execution::sequenced_policy>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 bool none(UnaryPredicate predicate, Execution execution = std::execution::seq) {
        if constexpr (internal::checkForwardAndPolicies<Execution, Iterator>()) {
            static_cast<void>(execution);
            return std::none_of(Base::begin(), Base::end(), std::move(predicate));
        }
        else {
            return std::none_of(execution, Base::begin(), Base::end(), std::move(predicate));
        }
    }

    /**
     * Counts how many occurrences of `value` are in this.
     * @param value The value to count
     * @return The amount of counted elements equal to `value`.
     */
    template<class T, class Execution = std::execution::sequenced_policy>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 difference_type count(const T& value, Execution execution = std::execution::seq) const {
        if constexpr (internal::checkForwardAndPolicies<Execution, Iterator>()) {
            static_cast<void>(execution);
            return std::count(Base::begin(), Base::end(), value);
        }
        else {
            return std::count(execution, Base::begin(), Base::end(), value);
        }
    }

    /**
     * Counts how many occurrences times the unary predicate returns true.
     * @param predicate The function predicate that must return a bool.
     * @return The amount of counted elements.
     */
    template<class UnaryPredicate, class Execution = std::execution::sequenced_policy>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 difference_type countIf(UnaryPredicate predicate,
                                                             Execution execution = std::execution::seq) const {
        if constexpr (internal::checkForwardAndPolicies<Execution, Iterator>()) {
            static_cast<void>(execution);
            return std::count_if(Base::begin(), Base::end(), std::move(predicate));
        }
        else {
            return std::count_if(execution, Base::begin(), Base::end(), std::move(predicate));
        }
    }

    /**
     * Sorts the sequence with the default (operator<) comparer.
     * @param execution The execution policy.
     * @return A reference to this.
     */
    template<class BinaryPredicate = std::less<>, class Execution = std::execution::sequenced_policy>
    LZ_CONSTEXPR_CXX_20 IterView<Iterator>& sort(BinaryPredicate predicate = {}, Execution execution = std::execution::seq) {
        if constexpr (internal::checkForwardAndPolicies<Execution, IterView>()) {
            static_cast<void>(execution);
            std::sort(Base::begin(), Base::end(), std::move(predicate));
        }
        else {
            std::sort(execution, Base::begin(), Base::end(), std::move(predicate));
        }
        return *this;
    }

    /**
     * Checks whether the sequence is sorted, using the standard (operator<) compare.
     * @param execution The execution policy.
     * @return True if the sequence is sorted given by the `predicate` false otherwise.
     */
    template<class BinaryPredicate = std::less<>, class Execution = std::execution::sequenced_policy>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 bool
    isSorted(BinaryPredicate predicate = {}, Execution execution = std::execution::seq) const {
        if constexpr (internal::checkForwardAndPolicies<Execution, Iterator>()) {
            static_cast<void>(execution);
            return std::is_sorted(Base::begin(), Base::end(), std::move(predicate));
        }
        else {
            return std::is_sorted(execution, Base::begin(), Base::end(), std::move(predicate));
        }
    }
#    else // ^^^ lz has execution vvv ! lz has execution

    //! See Filter.hpp for documentation
    template<class UnaryPredicate>
    IterView<internal::FilterIterator<Iterator, UnaryPredicate>> filter(UnaryPredicate predicate) const {
        return toIter(lz::filter(*this, std::move(predicate)));
    }

    //! See Except.hpp for documentation
    template<class IterableToExcept, class Compare = std::less<value_type>>
    IterView<internal::ExceptIterator<Iterator, internal::IterTypeFromIterable<IterableToExcept>, Compare>>
    except(IterableToExcept&& toExcept, Compare compare = {}) const {
        return toIter(lz::except(*this, toExcept, std::move(compare)));
    }

    //! See Unique.hpp for documentation
    template<class Compare = std::less<value_type>>
    IterView<internal::UniqueIterator<Iterator, Compare>> unique(Compare compare = {}) const {
        return toIter(lz::unique(*this, std::move(compare)));
    }

    //! See ChunkIf.hpp for documentation
    template<class UnaryPredicate>
    IterView<internal::ChunkIfIterator<Iterator, UnaryPredicate>> chunkIf(UnaryPredicate predicate) const {
        return toIter(lz::chunkIf(*this, std::move(predicate)));
    }

    //! See FunctionTools.hpp `filterMap` for documentation
    template<class UnaryMapFunc, class UnaryFilterFunc>
    IterView<internal::MapIterator<internal::FilterIterator<Iterator, UnaryFilterFunc>, UnaryMapFunc>>
    filterMap(UnaryFilterFunc filterFunc, UnaryMapFunc mapFunc) const {
        return toIter(lz::filterMap(*this, std::move(filterFunc), std::move(mapFunc)));
    }

    //! See FunctionTools.hpp `select` for documentation
    template<class SelectorIterable>
    auto select(SelectorIterable&& selectors) const
        -> decltype(toIter(lz::select(*this, std::forward<SelectorIterable>(selectors)))) {
        return toIter(lz::select(*this, std::forward<SelectorIterable>(selectors)));
    }

    //! See JoinWhere.hpp for documentation
    template<class IterableB, class SelectorA, class SelectorB, class ResultSelector>
    LZ_CONSTEXPR_CXX_20 IterView<
        internal::JoinWhereIterator<Iterator, internal::IterTypeFromIterable<IterableB>, SelectorA, SelectorB, ResultSelector>>
    joinWhere(IterableB&& iterableB, SelectorA a, SelectorB b, ResultSelector resultSelector) const {
        return toIter(lz::joinWhere(*this, iterableB, std::move(a), std::move(b), std::move(resultSelector)));
    }

    //! See Take.hpp for documentation
    template<class UnaryPredicate>
    IterView<Iterator> dropWhile(UnaryPredicate predicate) const {
        return toIter(lz::dropWhile(*this, std::move(predicate)));
    }

    //! See GroupBy.hpp for documentation
    template<class Comparer = std::equal_to<value_type>>
    IterView<internal::GroupByIterator<Iterator, Comparer>> groupBy(Comparer comparer = {}) const {
        return toIter(lz::groupBy(*this, std::move(comparer)));
    }

    //! See FunctionTools.hpp `trim` for documentation
    template<class UnaryPredicateFirst, class UnaryPredicateLast>
    auto trim(UnaryPredicateFirst first, UnaryPredicateLast last) const
        -> decltype(toIter(lz::trim(*this, std::move(first), std::move(last)))) {
        return toIter(lz::trim(*this, std::move(first), std::move(last)));
    }

    //! See FunctionTools.hpp `findFirstOrDefault` for documentation
    template<class T, class U>
    value_type findFirstOrDefault(T&& toFind, U&& defaultValue) const {
        return lz::findFirstOrDefault(*this, toFind, defaultValue);
    }

    //! See FunctionTools.hpp `findFirstOrDefaultIf` for documentation
    template<class UnaryPredicate, class U>
    value_type findFirstOrDefaultIf(UnaryPredicate predicate, U&& defaultValue) const {
        return lz::findFirstOrDefaultIf(*this, std::move(predicate), defaultValue);
    }

    //! See FunctionTools.hpp `findLastOrDefault` for documentation
    template<class T, class U>
    value_type findLastOrDefault(T&& toFind, U&& defaultValue) const {
        return lz::findLastOrDefault(*this, toFind, defaultValue);
    }

    //! See FunctionTools.hpp `findLastOrDefaultIf` for documentation
    template<class UnaryPredicate, class U>
    value_type findLastOrDefaultIf(UnaryPredicate predicate, U&& defaultValue) const {
        return lz::findLastOrDefaultIf(*this, std::move(predicate), defaultValue);
    }

    //! See FunctionTools.hpp `indexOf` for documentation
    template<class T>
    difference_type indexOf(const T& value) const {
        return lz::indexOf(*this, value);
    }

    //! See FunctionTools.hpp `indexOfIf` for documentation
    template<class UnaryPredicate>
    difference_type indexOfIf(UnaryPredicate predicate) const {
        return lz::indexOfIf(*this, std::move(predicate));
    }

    //! See FunctionTools.hpp `contains` for documentation
    template<class T>
    bool contains(const T& value) const {
        return lz::contains(*this, value);
    }

    //! See FunctionTools.hpp `containsIf` for documentation
    template<class UnaryPredicate>
    bool containsIf(UnaryPredicate predicate) const {
        return lz::containsIf(*this, std::move(predicate));
    }

    /**
     * Checks if two views/iterables are equal.
     * @param other The other view/iterable to compare with
     * @param compare The comparer, default is `operator==`
     * @return
     */
#        ifdef LZ_HAS_CXX_11
    template<class Iterable, class Compare = std::equal_to<value_Type>>
#        else
    template<class Iterable, class BinaryPredicate = std::equal_to<>>
#        endif
    bool equal(const Iterable& other, BinaryPredicate compare = {}) const {
        return lz::equal(*this, other, std::move(compare));
    }

#        ifdef LZ_HAS_CXX_11
    template<class Iterable, class Compare = std::equal_to<value_Type>>
#        else
    template<class Iterable, class BinaryPredicate = std::equal_to<>>
#        endif
    bool startsWith(const Iterable& iterable, BinaryPredicate compare = {}) const {
        return lz::startsWith(*this, iterable, std::move(compare));
    }

#        ifdef LZ_HAS_CXX_11
    template<class Iterable, class Compare = std::equal_to<value_Type>>
#        else
    template<class Iterable, class BinaryPredicate = std::equal_to<>>
#        endif
    bool endsWith(const Iterable& iterable, BinaryPredicate compare = {}) const {
        return lz::endsWith(*this, iterable, std::move(compare));
    }

    /**
     * Iterates over the sequence generated so far.
     * @param func A function to apply over each element. Must have the following signature: `void func(value_type)`
     */
    template<class UnaryFunc>
    IterView<Iterator>& forEach(UnaryFunc func) {
        std::for_each(Base::begin(), Base::end(), std::move(func));
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
        return internal::accumulate(Base::begin(), Base::end(), std::forward<T>(init), std::move(function));
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
        return internal::accumulate(internal::begin(std::move(reverseView)), internal::end(std::move(reverseView)),
                                    std::forward<T>(init), std::move(function));
    }

    /**
     * Sums the sequence generated so far.
     */
    value_type sum() const {
#        ifdef LZ_HAS_CXX_11
        return this->foldl(value_type(), [](value_type init, const value_type& val) { return std::move(init) + val; });
#        else
        return this->foldl(value_type(), std::plus<>());
#        endif // LZ_HAS_CXX_11
    }

    /**
     * Gets the max value of the current iterator view.
     * @param cmp The comparer. operator< is assumed by default.
     * @return The max element.
     */
#        ifdef LZ_HAS_CXX_11
    template<class Compare = std::less<value_type>>
#        else
    template<class Compare = std::less<>>
#        endif // LZ_HAS_CXX_11
    reference max(Compare cmp = {}) const {
        LZ_ASSERT(!lz::empty(*this), "sequence cannot be empty in order to get max element");
        return *std::max_element(Base::begin(), Base::end(), std::move(cmp));
    }

    /**
     * Gets the min value of the current iterator view.
     * @param cmp The comparer. operator< is assumed by default.
     * @return The min element.
     */
#        ifdef LZ_HAS_CXX_11
    template<class Compare = std::less<value_type>>
#        else
    template<class Compare = std::less<>>
#        endif // LZ_HAS_CXX_11
    reference min(Compare cmp = {}) const {
        LZ_ASSERT(!lz::empty(*this), "sequence cannot be empty in order to get min element");
        return *std::min_element(Base::begin(), Base::end(), std::move(cmp));
    }

    //! See FunctionTools.hpp for documentation
#        ifdef LZ_HAS_CXX_11
    template<class BinaryOp = std::plus<value_type>>
#        else
    template<class BinaryOp = std::plus<>>
#        endif // LZ_HAS_CXX_11
    double mean(BinaryOp binOp = {}) const {
        return lz::mean(*this, std::move(binOp));
    }

    //! See FunctionTools.hpp for documentation
#        ifdef LZ_HAS_CXX_11
    template<class Compare = std::less<value_type>>
#        else
    template<class Compare = std::less<>>
#        endif // LZ_HAS_CXX_11
    double median(Compare compare = {}) const {
        return lz::median(*this, std::move(compare));
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
#        ifdef LZ_HAS_CXX_11
    template<class Comparer = std::less<value_type>>
#        else
    template<class Comparer = std::less<>>
#        endif // LZ_HAS_CXX_11
    IterView<Iterator>& sort(Comparer comparer = {}) {
        std::sort(Base::begin(), Base::end(), std::move(comparer));
        return *this;
    }

    /**
     * Checks whether the sequence is sorted, using the standard (operator<) compare.
     * @return True if the sequence is sorted given by the `predicate` false otherwise.
     */
#        ifdef LZ_HAS_CXX_11
    template<class Comparer = std::less<value_type>>
#        else
    template<class Comparer = std::less<>>
#        endif // LZ_HAS_CXX_11
    bool isSorted(Comparer comparer = {}) const {
        return std::is_sorted(Base::begin(), Base::end(), std::move(comparer));
    }

#    endif // LZ_HAS_EXECUTION
};
} // namespace lz

#endif // LZ_LZ_HPP