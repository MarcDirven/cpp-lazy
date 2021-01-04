#pragma once

#ifndef LZ_LZ_HPP
#define LZ_LZ_HPP

#include "Lz/CartesianProduct.hpp"
#include "Lz/ChunkIf.hpp"
#include "Lz/Chunks.hpp"
#include "Lz/Enumerate.hpp"
#include "Lz/Except.hpp"
#include "Lz/Flatten.hpp"
#include "Lz/Generate.hpp"
#include "Lz/GroupBy.hpp"
#include "Lz/JoinWhere.hpp"
#include "Lz/Random.hpp"
#include "Lz/Range.hpp"
#include "Lz/Repeat.hpp"
#include "Lz/TakeEvery.hpp"
#include "Lz/Unique.hpp"

#include "Lz/FunctionTools.hpp"
// Function tools includes:
// StringSplitter.hpp
// Join.hpp
// Zip.hpp
// Map.hpp
// Filter.hpp
// Take.hpp
// Concatenate.hpp

namespace lz {
    namespace {
        template<class>
        struct IsCartesianProductIterator : std::false_type {
        };

        template<class T, class... Ts>
        struct IsCartesianProductIterator<internal::CartesianProductIterator<T, Ts...>> : std::true_type {
        };
    }

    template<LZ_CONCEPT_ITERATOR>
    class IterView;

    template<LZ_CONCEPT_ITERABLE Iterable>
    IterView<internal::IterTypeFromIterable<Iterable>> toIter(Iterable&& iterable);

    template<LZ_CONCEPT_ITERATOR Iterator>
    IterView<Iterator> toIterRange(Iterator begin, Iterator end);

    template<LZ_CONCEPT_ITERATOR Iterator>
    class IterView final : public internal::BasicIteratorView<Iterator> {
        using Base = internal::BasicIteratorView<Iterator>;

    public:
        using difference_type = internal::DiffType<Iterator>;
        using value_type = internal::ValueType<Iterator>;
        using reference = internal::RefType<Iterator>;

    private:
        difference_type lengthImpl(std::true_type /*IsCartesianProductIterator*/) const {
            return Base::begin().length();
        }

        difference_type lengthImpl(std::false_type /*IsCartesianProductIterator*/) const {
            return lz::length(*this);
        }

    public:
        IterView(Iterator begin, Iterator end):
            Base(std::move(begin), std::move(end))
        {}

        IterView() = default;

        //! See Concatenate.hpp for documentation.
        template<LZ_CONCEPT_ITERABLE... Iterables>
        IterView<internal::ConcatenateIterator<Iterator, internal::IterTypeFromIterable<Iterables>...>>
        concat(Iterables&&... iterables) const {
            return lz::toIter(lz::concat(*this, std::forward<Iterables>(iterables)...));
        }

        //! See Enumerate.hpp for documentation.
        template<LZ_CONCEPT_ARITHMETIC Arithmetic = int>
        IterView<internal::EnumerateIterator<Iterator, Arithmetic>> enumerate(const Arithmetic begin = 0) const {
            return lz::toIter(lz::enumerate(*this, begin));
        }

        //! See Join.hpp for documentation.
        IterView<internal::JoinIterator<Iterator>> join(std::string delimiter) const {
            return lz::toIter(lz::join(*this, std::move(delimiter)));
        }

        //! See Map.hpp for documentation
        template<class UnaryFunction>
        IterView<internal::MapIterator<Iterator, UnaryFunction>> map(UnaryFunction unaryFunction) const {
            return lz::toIter(lz::map(*this, std::move(unaryFunction)));
        }

        //! See Take.hpp for documentation.
        template<class UnaryPredicate>
        IterView<Iterator> takeWhile(UnaryPredicate predicate) const {
            return lz::toIter(lz::takeWhile(*this, std::move(predicate)));
        }

        //! See Take.hpp for documentation.
        IterView<Iterator> take(const difference_type amount) const {
            return lz::toIter(lz::take(*this, amount));
        }

        //! See Take.hpp for documentation.
        IterView<Iterator> drop(const difference_type amount) const {
            return lz::toIter(lz::drop(*this, amount));
        }

        //! See Take.hpp for documentation.
        IterView<Iterator> slice(const difference_type from, const difference_type to) const {
            return lz::toIter(lz::slice(*this, from, to));
        }

        //! See Take.hpp for documentation.
        IterView<internal::TakeEveryIterator<Iterator>> takeEvery(const difference_type offset, const difference_type start = 0) const {
            return lz::toIter(lz::takeEvery(*this, offset, start));
        }

        //! See Chunks.hpp for documentation
        IterView<internal::ChunksIterator<Iterator>> chunks(const std::size_t chunkSize) const {
        	return lz::toIter(lz::chunks(*this, chunkSize));
		}

        //! See Zip.hpp for documentation.
        template<LZ_CONCEPT_ITERABLE... Iterables>
        IterView<internal::ZipIterator<Iterator, internal::IterTypeFromIterable<Iterables>>...>
		zip(Iterables&&... iterables) const {
            return lz::toIter(lz::zip(*this, std::forward<Iterables>(iterables)...));
        }

        //! See FunctionTools.hpp `zipWith` for documentation
        template<class Fn, class... Iterables>
        auto zipWith(Fn fn, Iterables&&... iterables) const ->
        IterView<decltype(lz::zipWith(std::move(fn), std::forward<Iterables>(iterables)...))> {
            return lz::toIter(lz::zipWith(std::move(fn), std::forward<Iterables>(iterables)...));
        }

        //! See FunctionTools.hpp `as` for documentation.
        template<class T>
        IterView<internal::MapIterator<Iterator, internal::ConvertFn<T>>> as() const {
            return lz::toIter(lz::as<T>(*this));
        }

        //! See FunctionTools.hpp `reverse` for documentation.
        IterView<std::reverse_iterator<Iterator>> reverse() const {
            return lz::toIter(lz::reverse(*this));
        }

        //! See FunctionTools.hpp `reverse` for documentation.
        IterView<internal::ZipIterator<Iterator, Iterator>> pairwise() const {
            return lz::toIter(lz::pairwise(*this));
        }

        //! See FunctionTools.hpp `trim` for documentation
        template<class UnaryPredicateFirst, class UnaryPredicateLast>
        IterView<typename lz::Take<std::reverse_iterator<std::reverse_iterator<Iterator>>>::iterator>
        trim(UnaryPredicateFirst first, UnaryPredicateLast last) const {
            return lz::toIter(lz::trim(*this, std::move(first), std::move(last)));
        }

        //! See CartesianProduct.hpp for documentation
        template<class... Iterables>
        IterView<internal::CartesianProductIterator<Iterator, internal::IterTypeFromIterable<Iterables>...>>
        cartesian(Iterables&&... iterables) const {
            return lz::toIter(lz::cartesian(*this, std::forward<Iterables>(iterables)...));
        }

        /**
         * Gets the nth element from this sequence.
         * @param n The offset.
         * @return The element referred to by `begin() + n`
         */
        reference nth(const difference_type n) const {
            return *std::next(Base::begin(), n);
        }

        //! See FunctionTools.hpp `length` for documentation.
        difference_type length() const {
            return lengthImpl(IsCartesianProductIterator<Iterator>());
        }

        //! See FunctionTools.hpp `isEmpty` for documentation.
        bool isEmpty() const {
            return lz::isEmpty(*this);
        }

        //! See FunctionTools.hpp `hasOne` for documentation.
        bool hasOne() const {
            return lz::hasOne(*this);
        }

        //! See FunctionTools.hpp `hasMany` for documentation.
        bool hasMany() const {
            return lz::hasMany(*this);
        }

        //! See FunctionTools.hpp `first` for documentation.
        reference first() const {
            return lz::first(*this);
        }

        //! See FunctionTools.hpp `last` for documentation.
        reference last() const {
            return lz::last(*this);
        }

        //! See FunctionTools.hpp `firstOr` for documentation.
        template<class T>
        value_type firstOr(const T& defaultValue) const {
            return lz::firstOr(*this, defaultValue);
        }

        //! See FunctionTools.hpp `lastOr` for documentation.
        template<class T>
        value_type lastOr(const T& defaultValue) const {
            return lz::lastOr(*this, defaultValue);
        }

#ifdef LZ_HAS_EXECUTION
        //! See Filter.hpp for documentation.
        template<class UnaryPredicate, class Execution = std::execution::sequenced_policy>
        IterView<internal::FilterIterator<Execution, Iterator, UnaryPredicate>> filter(UnaryPredicate predicate,
                                                                                       const Execution exec = std::execution::seq) const {
            return lz::toIter(lz::filter(*this, std::move(predicate), exec));
        }

        //! See Except.hpp for documentation.
        template<class IterableToExcept, class Execution = std::execution::sequenced_policy>
        IterView<internal::ExceptIterator<Iterator, internal::IterTypeFromIterable<IterableToExcept>, Execution>>
        except(IterableToExcept&& toExcept, const Execution exec = std::execution::seq) const {
            return lz::toIter(lz::except(*this, toExcept, exec));
        }

        //! See Unique.hpp for documentation.
        template<class Execution = std::execution::sequenced_policy>
        IterView<internal::UniqueIterator<Iterator, Execution>> unique(const Execution exec = std::execution::seq) const {
            return lz::toIter(lz::unique(*this, exec));
        }

        //! See ChunkIf.hpp for documentation
		template<class UnaryPredicate, class Execution = std::execution::sequenced_policy>
		IterView<internal::ChunkIfIterator<Iterator, UnaryPredicate, Execution>>
		chunkIf(UnaryPredicate predicate, Execution execution = std::execution::seq) const {
			return lz::toIter(lz::chunkIf(*this, std::move(predicate), execution));
		}

        //! See FunctionTools.hpp `filterMap` for documentation.
        template<class UnaryMapFunc, class UnaryFilterFunc, class Execution = std::execution::sequenced_policy>
        IterView<internal::MapIterator<internal::FilterIterator<Iterator, UnaryFilterFunc, Execution>, UnaryMapFunc>>
        filterMap(UnaryMapFunc mapFunc, UnaryFilterFunc filterFunc, const Execution exec = std::execution::seq) const {
            return lz::toIter(lz::filterMap(*this, std::move(filterFunc), std::move(mapFunc), exec));
        }

        //! See FunctionTools.hpp `select` for documentation.
        template<class SelectorIterable, class Execution = std::execution::sequenced_policy>
        auto select(SelectorIterable&& selectors, const Execution exec = std::execution::seq) const {
            return lz::toIter(lz::select(*this, std::forward<SelectorIterable>(selectors), exec));
        }

        //! See Take.hpp for documentation
        template<class UnaryPredicate, class Execution = std::execution::sequenced_policy>
        IterView<Iterator> dropWhile(UnaryPredicate predicate, const Execution exec = std::execution::seq) const {
            return lz::toIter(lz::dropWhile(*this, std::move(predicate), exec));
        }

        //! See JoinWhere.hpp for documentation
        template<class IterableA, class IterableB, class SelectorA, class SelectorB, class ResultSelector,
            class Execution = std::execution::sequenced_policy>
        IterView<JoinWhere<internal::IterTypeFromIterable<IterableA>, internal::IterTypeFromIterable<IterableB>,
            SelectorA, SelectorB, ResultSelector, Execution>>
        joinWhere(IterableB&& iterableB, SelectorA a, SelectorB b, ResultSelector resultSelector,
                 Execution execution = std::execution::seq) const {
            return lz::toIter(lz::joinWhere(*this, iterableB, std::move(a), std::move(b), std::move(resultSelector), execution));
        }

		//! See GroupBy.hpp for documentation
		template<class KeySelector, class Execution = std::execution::sequenced_policy>
		IterView<internal::GroupByIterator<Iterator, KeySelector>> groupBy(KeySelector selector,
																	 	   Execution execution = std::execution::seq,
																	 	   const bool sort = true) const {
			return lz::toIter(lz::groupBy(*this, std::move(selector), execution, sort));
		}

        //! See FunctionTools.hpp `firstOrDefault` for documentation.
        template<class T, class U, class Execution = std::execution::sequenced_policy>
        value_type firstOrDefault(T&& toFind, U&& defaultValue, const Execution exec = std::execution::seq) const {
            return lz::firstOrDefault(*this, toFind, defaultValue, exec);
        }

        //! See FunctionTools.hpp `firstOrDefaultIf` for documentation.
        template<class UnaryPredicate, class U, class Execution = std::execution::sequenced_policy>
        value_type firstOrDefaultIf(UnaryPredicate predicate, U&& defaultValue, const Execution exec = std::execution::seq) const {
            return lz::firstOrDefault(*this, std::move(predicate), defaultValue, exec);
        }

        //! See FunctionTools.hpp `lastOrDefault` for documentation.
        template<class T, class U, class Execution = std::execution::sequenced_policy>
        value_type lastOrDefault(T&& toFind, U&& defaultValue, const Execution exec = std::execution::seq) const {
            return lz::lastOrDefault(*this, toFind, defaultValue, exec);
        }

        //! See FunctionTools.hpp `lastOrDefaultIf` for documentation.
        template<class UnaryPredicate, class U, class Execution = std::execution::sequenced_policy>
        value_type lastOrDefaultIf(UnaryPredicate predicate, U&& defaultValue, const Execution exec = std::execution::seq) const {
            return lz::lastOrDefault(*this, std::move(predicate), defaultValue, exec);
        }

        //! See FunctionTools.hpp `indexOf` for documentation.
        template<class T, class Execution = std::execution::sequenced_policy>
        difference_type indexOf(const T& value, const Execution exec = std::execution::seq) const {
            return lz::indexOf(*this, value, exec);
        }

        //! See FunctionTools.hpp `indexOfIf` for documentation.
        template<class UnaryPredicate, class Execution = std::execution::sequenced_policy>
        difference_type indexOf(UnaryPredicate predicate, const Execution exec = std::execution::seq) const {
            return lz::indexOfIf(*this, std::move(predicate), exec);
        }

        //! See FunctionTools.hpp `contains` for documentation.
        template<class T, class Execution = std::execution::sequenced_policy>
        bool contains(const T& value, const Execution exec = std::execution::seq) const {
            return lz::contains(*this, value, exec);
        }

        //! See FunctionTools.hpp `containsIf` for documentation.
        template<class UnaryPredicate, class Execution = std::execution::sequenced_policy>
        bool containsIf(UnaryPredicate predicate, const Execution exec = std::execution::seq) const {
            return lz::containsIf(*this, std::move(predicate), exec);
        }

        /**
         * Iterates over the sequence generated so far.
         * @param func A function to apply over each element. Must have the following signature: `void func(value_type)`
         * @param exec The execution policy.
         */
        template<class UnaryFunc, class Execution = std::execution::sequenced_policy>
        const IterView<Iterator>& forEach(UnaryFunc func, const Execution exec = std::execution::seq) const {
            std::for_each(exec, Base::begin(), Base::end(), std::move(func));
            return *this;
        }

        /**
         * Performs a left fold with as starting point `init`. Can be used to for e.g. sum all values. For this use:
         * `[](value_type init, value_type next) { return init + value_type; }`
         * @param init The starting value
         * @param function A binary function with the following signature `value_type func(value_type init, value_type element)`
         */
        template<class T, class BinaryFunction, class Execution = std::execution::sequenced_policy>
        T foldl(T&& init, BinaryFunction function, const Execution exec = std::execution::seq) const {
            if constexpr (internal::checkForwardAndPolicies<Execution, Iterator>()) {
                static_cast<void>(exec);
                return std::reduce(Base::begin(), Base::end(), std::forward<T>(init), std::move(function));
            }
            else {
                return std::reduce(exec, Base::begin(), Base::end(), std::forward<T>(init), std::move(function));
            }
        }

        /**
         * Performs a right fold with as starting point `init`. Can be used to for e.g. sum all values. For this use:
         * `[](value_type init, value_type next) { return init + value_type; }`
         * @param init The starting value
         * @param function A binary function with the following signature `value_type func(value_type init, value_type element)`
         */
        template<class T, class BinaryFunction, class Execution = std::execution::sequenced_policy>
        T foldr(T&& init, BinaryFunction function, const Execution exec = std::execution::seq) const {
            IterView<std::reverse_iterator<Iterator>> reverseView = this->reverse();
            if constexpr (internal::checkForwardAndPolicies<Execution, Iterator>()) {
                static_cast<void>(exec);
                return std::reduce(std::move(reverseView).begin(), std::move(reverseView).end(), std::forward<T>(init),
                                   std::move(function));
            }
            else {
                return std::reduce(exec, std::move(reverseView).begin(), std::move(reverseView).end(), std::forward<T>(init),
                                   std::move(function));
            }
        }

        /**
         * Sums the sequence generated so far.
         * @param exec The exectuion policy.
         */
        template<class Execution = std::execution::sequenced_policy>
        value_type sum(const Execution exec = std::execution::seq) const {
            return this->foldl(value_type(), std::plus<>(), exec);
        }

        /**
         * Gets the min value of the current iterator view.
         * @param cmp The comparer. operator< is assumed by default.
         * @param exec The execution policy.
         * @return The min element.
         */
        template<class Compare = std::less<>, class Execution = std::execution::sequenced_policy>
        value_type max(Compare cmp = std::less<>(), const Execution exec = std::execution::seq) const {
            if constexpr (internal::checkForwardAndPolicies<Execution, Iterator>()) {
                static_cast<void>(exec);
                return std::max_element(exec, Base::begin(), Base::end(), cmp);
            }
            else {
                return std::max_element(Base::begin(), Base::end(), cmp);
            }
        }

        /**
         * Gets the min value of the current iterator view.
         * @param cmp The comparer. operator< is assumed by default.
         * @param exec The execution policy.
         * @return The min element.
         */
        template<class Compare = std::less<>, class Execution = std::execution::sequenced_policy>
        value_type min(Compare cmp = std::less<>(), const Execution exec = std::execution::seq) const {
            if constexpr (internal::checkForwardAndPolicies<Execution, Iterator>()) {
                static_cast<void>(exec);
                return std::min_element(exec, Base::begin(), Base::end(), cmp);
            }
            else {
                return std::min_element(Base::begin(), Base::end(), cmp);
            }
        }

        /**
         * Checks if all of the elements meet the condition `predicate`. `predicate` must return a bool and take a `value_type` as parameter.
         * @param predicate The function that checks if an element meets a certain condition.
         * @param exec The execution policy.
         */
        template<class UnaryPredicate, class Execution = std::execution::sequenced_policy>
        bool all(UnaryPredicate predicate, const Execution exec = std::execution::seq) const {
            if constexpr (internal::checkForwardAndPolicies<Execution, Iterator>()) {
                static_cast<void>(exec);
                return std::all_of(Base::begin(), Base::end(), std::move(predicate));
            }
            else {
                return std::all_of(exec, Base::begin(), Base::end(), std::move(predicate));
            }
        }

        /**
         * Checks if any of the elements meet the condition `predicate`. `predicate` must return a bool and take a `value_type` as parameter.
         * @param predicate The function that checks if an element meets a certain condition.
         * @param exec The execution policy.
         */
        template<class UnaryPredicate, class Execution = std::execution::sequenced_policy>
        bool any(UnaryPredicate predicate, const Execution exec = std::execution::seq) const {
            if constexpr (internal::checkForwardAndPolicies<Execution, Iterator>()) {
                static_cast<void>(exec);
                return std::any_of(Base::begin(), Base::end(), std::move(predicate));
            }
            else {
                return std::any_of(exec, Base::begin(), Base::end(), std::move(predicate));
            }
        }

        /**
         * Checks if none of the elements meet the condition `predicate`. `predicate` must return a bool and take a `value_type` as parameter.
         * @param predicate The function that checks if an element meets a certain condition.
         * @param exec The execution policy.
         */
        template<class UnaryPredicate, class Execution = std::execution::sequenced_policy>
        bool none(UnaryPredicate predicate, const Execution exec = std::execution::seq) const {
            return !this->all(std::move(predicate), exec);
        }

        /**
         * Counts how many occurrences of `value` are in this.
         * @param value The value to count
         * @return The amount of counted elements equal to `value`.
         */
        template<class T, class Execution = std::execution::sequenced_policy>
        difference_type count(const T& value, Execution execution = std::execution::seq) const {
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
        difference_type countIf(UnaryPredicate predicate, Execution execution = std::execution::seq) const {
        	if constexpr (internal::checkForwardAndPolicies<Execution, Iterator>()) {
        		static_cast<void>(execution);
        		return std::count_if(Base::begin(), Base::end(), std::move(predicate));
        	}
        	else {
        		return std::count_if(execution, Base::begin(), Base::end(), std::move(predicate));
        	}
        }

        /**
         * Sorts the sequence given by a predicate.
         * @param predicate A callable object that takes two values types as its parameter and returns a bool.
         * @param execution The execution policy.
         * @return A reference to this.
         */
        template<class BinaryPredicate, class Execution = std::execution::sequenced_policy>
        const IterView<Iterator>& sortBy(BinaryPredicate predicate, Execution execution = std::execution::seq) const {
            if constexpr (internal::checkForwardAndPolicies<Execution, Iterator>()) {
                std::sort(Base::begin(), Base::end(), predicate);
            }
            else {
                std::sort(execution, Base::begin(), Base::end(), predicate);
            }
            return *this;
        }

        /**
         * Sorts the sequence with the default (operator<) comparer.
         * @param execution The execution policy.
         * @return A reference to this.
         */
        template<class Execution = std::execution::sequenced_policy>
        const IterView<Iterator>& sort(Execution execution = std::execution::seq) const {
            return this->sortBy(std::less<>(), execution);
        }

        /**
         * Checks whether the sequence is sorted, given by a predicate. The predicate must take the underlying type of the iterator as its
         * argument and return a bool.
         * @param predicate The predicate -- which specifies how to sort -- that takes the type of the underlying iterator and returns a
         * bool.
         * @param exec The execution policy.
         * @return True if the sequence is sorted given by the `predicate` false otherwise.
         */
        template<class BinaryPredicate, class Execution = std::execution::sequenced_policy>
        bool isSortedBy(BinaryPredicate predicate, const Execution exec = std::execution::seq) const {
            if constexpr (internal::checkForwardAndPolicies<Execution, Iterator>()) {
                return std::is_sorted(Base::begin(), Base::end(), std::move(predicate));
            }
            else {
                return std::is_sorted(exec, Base::begin(), Base::end(), std::move(predicate));
            }
        }

        /**
         * Checks whether the sequence is sorted, using the standard (operator<) compare.
         * @param exec The execution policy.
         * @return True if the sequence is sorted given by the `predicate` false otherwise.
         */
        template<class Execution = std::execution::sequenced_policy>
        bool isSorted(const Execution exec = std::execution::seq) const {
            return this->isSortedBy(std::less<>(), exec);
        }

#else // ^^^ lz has execution vvv ! lz has execution
        //! See Filter.hpp for documentation
        template<class UnaryPredicate>
        IterView<internal::FilterIterator<Iterator, UnaryPredicate>> filter(UnaryPredicate predicate) const {
            return lz::toIter(lz::filter(*this, std::move(predicate)));
        }

        //! See Except.hpp for documentation
        template<class IterableToExcept>
        IterView<internal::ExceptIterator<Iterator, internal::IterTypeFromIterable<IterableToExcept>>>
        except(IterableToExcept&& toExcept) const {
            return lz::toIter(lz::except(*this, toExcept));
        }

        //! See Unique.hpp for documentation
        IterView<internal::UniqueIterator<Iterator>> unique() const {
            return lz::toIter(lz::unique(*this));
        }

        //! See ChunkIf.hpp for documentation
		template<class UnaryPredicate>
		IterView<internal::ChunkIfIterator<Iterator, UnaryPredicate>> chunkIf(UnaryPredicate predicate) const {
			return lz::toIter(lz::chunkIf(*this, std::move(predicate)));
		}

        //! See FunctionTools.hpp `filterMap` for documentation
        template<class UnaryMapFunc, class UnaryFilterFunc>
        IterView<internal::MapIterator<internal::FilterIterator<Iterator, UnaryFilterFunc>, UnaryMapFunc>>
        filterMap(UnaryMapFunc mapFunc, UnaryFilterFunc filterFunc) const {
            return lz::toIter(lz::filterMap(*this, std::move(filterFunc), std::move(mapFunc)));
        }

        //! See FunctionTools.hpp `select` for documentation
        template<class SelectorIterable
#ifdef LZ_HAS_CXX_11
            , class It = internal::IterTypeFromIterable<SelectorIterable>,
            class SelectorIterator = internal::IterTypeFromIterable<SelectorIterable>,
            class ZipIter = typename lz::Zip<Iterator, SelectorIterator>::iterator,
            class RefTuple = internal::RefType<ZipIter>
#endif // end lz has cxx11
        >
        auto select(SelectorIterable&& selectors)
#ifdef LZ_HAS_CXX_11
        -> IterView<internal::MapIterator<internal::FilterIterator<internal::ZipIterator<It, SelectorIterator>,
                                                                   std::function<bool(RefTuple)>>,
                                          std::function<internal::RefType<It>(RefTuple)>>>
#endif // end lz has cxx11
        {
            return lz::toIter(lz::select(*this, std::forward<SelectorIterable>(selectors)));
        }

        //! See Take.hpp for documentation
        template<class UnaryPredicate>
        IterView<Iterator> dropWhile(UnaryPredicate predicate) const {
            return lz::toIter(lz::dropWhile(*this, std::move(predicate)));
        }

        //! See JoinWhere.hpp for documentation
        template<class IterableA, class IterableB, class SelectorA, class SelectorB, class ResultSelector>
        IterView<JoinWhere<internal::IterTypeFromIterable<IterableA>, internal::IterTypeFromIterable<IterableB>,
            SelectorA, SelectorB, ResultSelector>>
        joinWhere(IterableB&& iterableB, SelectorA a, SelectorB b, ResultSelector resultSelector) const {
            return lz::toIter(lz::joinWhere(*this, iterableB, std::move(a), std::move(b), std::move(resultSelector)));
        }

		//! See GroupBy.hpp for documentation
		template<class KeySelector>
		IterView<internal::GroupByIterator<Iterator, KeySelector>> groupBy(KeySelector selector, const bool sort = true) const {
			return lz::toIter(lz::groupBy(*this, std::move(selector), sort));
		}

        //! See FunctionTools.hpp `firstOrDefault` for documentation
        template<class T, class U>
        value_type firstOrDefault(T&& toFind, U&& defaultValue) const {
            return lz::firstOrDefault(*this, toFind, defaultValue);
        }

        //! See FunctionTools.hpp `firstOrDefaultIf` for documentation
        template<class UnaryPredicate, class U>
        value_type firstOrDefaultIf(UnaryPredicate predicate, U&& defaultValue) const {
            return lz::firstOrDefault(*this, std::move(predicate), defaultValue);
        }

        //! See FunctionTools.hpp `lastOrDefault` for documentation
        template<class T, class U>
        value_type lastOrDefault(T&& toFind, U&& defaultValue) const {
            return lz::lastOrDefault(*this, toFind, defaultValue);
        }

        //! See FunctionTools.hpp `lastOrDefaultIf` for documentation
        template<class UnaryPredicate, class U>
        value_type lastOrDefaultIf(UnaryPredicate predicate, U&& defaultValue) const {
            return lz::lastOrDefault(*this, std::move(predicate), defaultValue);
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
         * Iterates over the sequence generated so far.
         * @param func A function to apply over each element. Must have the following signature: `void func(value_type)`
         */
        template<class UnaryFunc>
        const IterView<Iterator>& forEach(UnaryFunc func) const {
            std::for_each(Base::begin(), Base::end(), std::move(func));
            return *this;
        }

        /**
         * Performs a left fold with as starting point `init`. Can be used to for e.g. sum all values. For this use:
         * `[](value_type init, value_type next) { return init + value_type; }`
         * @param init The starting value
         * @param function A binary function with the following signature `value_type func(value_type init, value_type element)`
         */
        template<class T, class BinaryFunction>
        T foldl(T&& init, BinaryFunction function) const {
            return std::accumulate(Base::begin(), Base::end(), std::forward<T>(init), std::move(function));
        }

        /**
         * Performs a right fold with as starting point `init`. Can be used to for e.g. sum all values. For this use:
         * `[](value_type init, value_type next) { return init + value_type; }`
         * @param init The starting value
         * @param function A binary function with the following signature `value_type func(value_type init, value_type element)`
         */
        template<class T, class BinaryFunction>
        T foldr(T&& init, BinaryFunction function) const {
            IterView<std::reverse_iterator<Iterator>> reverseView = this->reverse();
            return std::accumulate(std::move(reverseView).begin(), std::move(reverseView).end(), std::forward<T>(init),
                                   std::move(function));
        }

        /**
         * Sums the sequence generated so far.
         */
        value_type sum() const {
            return this->foldl(value_type(), std::plus<value_type>());
        }

        /**
         * Gets the max value of the current iterator view.
         * @param cmp The comparer. operator< is assumed by default.
         * @return The max element.
         */
        template<class Compare = std::less<value_type>>
        value_type max(Compare cmp = std::less<value_type>()) const {
            return std::max_element(Base::begin(), Base::end(), std::move(cmp));
        }

        /**
         * Gets the min value of the current iterator view.
         * @param cmp The comparer. operator< is assumed by default.
         * @return The min element.
         */
        template<class Compare = std::less<value_type>>
        value_type min(Compare cmp = std::less<value_type>()) const {
            return std::min_element(Base::begin(), Base::end(), std::move(cmp));
        }

        /**
         * Checks if all of the elements meet the condition `predicate`. `predicate` must return a bool and take a `value_type` as parameter.
         * @param predicate The function that checks if an element meets a certain condition.
         */
        template<class UnaryPredicate>
        bool all(UnaryPredicate predicate) const {
            return std::all_of(Base::begin(), Base::end(), std::move(predicate));
        }

        /**
         * Checks if any of the elements meet the condition `predicate`. `predicate` must return a bool and take a `value_type` as parameter.
         * @param predicate The function that checks if an element meets a certain condition.
         */
        template<class UnaryPredicate>
        bool any(UnaryPredicate predicate) const {
            return std::any_of(Base::begin(), Base::end(), std::move(predicate));
        }

        /**
         * Checks if none of the elements meet the condition `predicate`. `predicate` must return a bool and take a `value_type` as parameter.
         * @param predicate The function that checks if an element meets a certain condition.
         */
        template<class UnaryPredicate>
        bool none(UnaryPredicate predicate) const {
            return !this->all(std::move(predicate));
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
         * Sorts the sequence given by a predicate.
         * @param predicate A callable object that takes two values types as its parameter and returns a bool.
         * @return A reference to this.
         */
        template<class BinaryPredicate>
        const IterView<Iterator>& sortBy(BinaryPredicate predicate) const {
            std::sort(Base::begin(), Base::end(), predicate);
            return *this;
        }

        /**
         * Sorts the sequence with the default (operator<) comparer.
         * @return A reference to this.
         */
        const IterView<Iterator>& sort() const {
            return this->sortBy(std::less<value_type>());
        }

        /**
         * Checks whether the sequence is sorted, given by a predicate. The predicate must take the underlying type of the iterator as its
         * argument and return a bool.
         * @param predicate The predicate -- which specifies how to sort -- that takes the type of the underlying iterator and returns a
         * bool.
         * @return True if the sequence is sorted given by the `predicate` false otherwise.
         */
        template<class BinaryPredicate>
        bool isSortedBy(BinaryPredicate predicate) const {
            return std::is_sorted(Base::begin(), Base::end(), predicate);
        }

        /**
         * Checks whether the sequence is sorted, using the standard (operator<) compare.
         * @return True if the sequence is sorted given by the `predicate` false otherwise.
         */
        bool isSorted() const {
            return this->isSortedBy(std::less<value_type>());
        }
#endif // end lz has execution
    };

    /**
     * Converts an iterable into a IterView, where one can chain iterators using dot operator (.filter().map().select().any())
     * @param iterable The iterable to view over.
     * @return An iterator view object.
     */
    template<LZ_CONCEPT_ITERABLE Iterable>
    IterView<internal::IterTypeFromIterable<Iterable>> toIter(Iterable&& iterable) {
        return lz::toIterRange(internal::begin(std::forward<Iterable>(iterable)), internal::end(std::forward<Iterable>(iterable)));
    }

    /**
     * Converts an iterable into a IterView, where one can chain iterators using dot operator (.filter().map().select().any())
     * @param iterable The iterable to view over.
     * @return An iterator view object.
     */
    template<LZ_CONCEPT_ITERATOR Iterator>
    IterView<Iterator> toIterRange(Iterator begin, Iterator end) {
        return lz::IterView<Iterator>(std::move(begin), std::move(end));
    }
}

#endif // LZ_LZ_HPP