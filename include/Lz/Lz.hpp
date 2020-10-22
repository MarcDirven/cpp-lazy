#pragma once

#ifndef LZ_LZ_HPP
#define LZ_LZ_HPP

#include "Lz/Affirm.hpp"
#include "Lz/Enumerate.hpp"
#include "Lz/Except.hpp"
#include "Lz/Generate.hpp"
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
    template<LZ_CONCEPT_ITERATOR>
    class IterView;

    template<LZ_CONCEPT_ITERABLE Iterable>
    IterView<internal::IterTypeFromIterable<Iterable>> toIter(Iterable&& iterable);

    template<LZ_CONCEPT_ITERATOR Iterator>
    IterView<Iterator> toIter(Iterator begin, Iterator end);

    template<LZ_CONCEPT_ITERATOR Iterator>
    class IterView final : public internal::BasicIteratorView<Iterator> {
        using Base = internal::BasicIteratorView<Iterator>;

    public:
        using difference_type = internal::DiffType<Iterator>;
        using value_type = internal::ValueType<Iterator>;
        using reference = internal::RefType<Iterator>;

        IterView(Iterator begin, Iterator end):
            Base(std::move(begin), std::move(end))
        {}

        template<class Exception, class UnaryPredicate>
        IterView<internal::AffirmIterator<Exception, Iterator, UnaryPredicate>>
        affirm(Exception&& exception, UnaryPredicate predicate) const {
            using AffirmView = lz::Affirm<Exception, Iterator, UnaryPredicate>;
            AffirmView view = lz::affirm(*this, std::forward<Exception>(exception), std::move(predicate));
            return lz::toIter(std::move(view));
        }

        template<LZ_CONCEPT_ITERABLE... Iterables>
        IterView<internal::ConcatenateIterator<Iterator, internal::IterTypeFromIterable<Iterables>...>>
        concat(Iterables&&... iterables) const {
            auto view = lz::concat(*this, iterables...);
            return lz::toIter(std::move(view));
        }

        template<LZ_CONCEPT_ARITHMETIC Arithmetic = int>
        IterView<internal::EnumerateIterator<Iterator, Arithmetic>> enumerate(const Arithmetic begin = 0) const {
            using EnumView = lz::Enumerate<Iterator, Arithmetic>;
            EnumView view = lz::enumerate(*this, begin);
            return lz::toIter(std::move(view));
        }

        IterView<internal::JoinIterator<Iterator>> join(std::string delimiter) const {
            lz::Join<Iterator> joiner = lz::join(*this, std::move(delimiter));
            return lz::toIter(std::move(joiner));
        }

        template<class UnaryFunction>
        IterView<internal::MapIterator<Iterator, UnaryFunction>> map(UnaryFunction unaryFunction) const {
            using MapView = lz::Map<Iterator, UnaryFunction>;
            MapView view = lz::map(*this, std::move(unaryFunction));
            return lz::toIter(std::move(view));
        }

        template<class UnaryPredicate>
        IterView<Iterator> takeWhile(UnaryPredicate predicate) const {
            lz::Take<Iterator> view = lz::takeWhile(*this, std::move(predicate));
            return lz::toIter(std::move(view));
        }

        IterView<Iterator> take(const difference_type amount) const {
            lz::Take<Iterator> view = lz::take(*this, amount);
            return lz::toIter(std::move(view));
        }

        IterView<Iterator> slice(const difference_type from, const difference_type to) const {
            lz::Take<Iterator> view = lz::slice(*this, from, to);
            return lz::toIter(std::move(view));
        }

        IterView<internal::TakeEveryIterator<Iterator>> takeEvery(const difference_type offset, const difference_type start = 0) const {
            lz::TakeEvery<Iterator> view = lz::takeEvery(*this, offset, start);
            return lz::toIter(std::move(view));
        }

        template<LZ_CONCEPT_ITERABLE... Iterables>
        IterView<internal::ZipIterator<Iterator, internal::IterTypeFromIterable<Iterables>>...> zip(Iterables&&... iterables) const {
            auto view = lz::zip(*this, iterables...);
            return lz::toIter(std::move(view));
        }

        template<class T>
        IterView<internal::MapIterator<Iterator, internal::ConvertFn<T>>> as() const {
            using MapAsView = lz::Map<Iterator, internal::ConvertFn<T>>;
            MapAsView view = lz::as<T>(*this);
            return lz::toIter(std::move(view));
        }

        IterView<std::reverse_iterator<Iterator>> reverse() const {
            lz::Take<std::reverse_iterator<Iterator>> view = lz::reverse(*this);
            return lz::toIter(std::move(view));
        }

        IterView<internal::ZipIterator<Iterator, Iterator>> pairwise() const {
            Zip<Iterator, Iterator> view = lz::pairwise(*this);
            return lz::toIter(std::move(view));
        }

        difference_type length() const {
            return lz::length(*this);
        }

        bool isEmpty() const {
            return lz::isEmpty(*this);
        }

        bool hasOne() const {
            return lz::hasOne(*this);
        }

        bool hasMany() const {
            return lz::hasMany(*this);
        }

        reference first() const {
            return lz::first(*this);
        }

        reference last() const {
            return lz::last(*this);
        }

        template<class T>
        value_type firstOr(const T& defaultValue) const {
            return lz::firstOr(*this, defaultValue);
        }

        template<class T>
        value_type lastOr(const T& defaultValue) const {
            return lz::lastOr(*this, defaultValue);
        }

#ifdef LZ_HAS_EXECUTION
        template<class UnaryPredicate, class Execution = std::execution::sequenced_policy>
        IterView<internal::FilterIterator<Iterator, UnaryPredicate, Execution>> filter(UnaryPredicate predicate,
                                                                                       const Execution exec = std::execution::seq) const {
            using FilterView = lz::Filter<Iterator, UnaryPredicate, Execution>;
            FilterView view = lz::filter(*this, std::move(predicate), exec);
            return lz::toIter(std::move(view));
        }

        template<class IterableToExcept, class Execution = std::execution::sequenced_policy>
        IterView<internal::ExceptIterator<Iterator, internal::IterTypeFromIterable<IterableToExcept>, Execution>>
        except(IterableToExcept&& toExcept, const Execution exec = std::execution::seq) const {
            using ExceptView = lz::Except<Iterator, internal::IterTypeFromIterable<IterableToExcept>, Execution>;
            ExceptView view = lz::except(*this, toExcept, exec);
            return lz::toIter(std::move(view));
        }
        template<class Execution = std::execution::sequenced_policy>
        IterView<internal::UniqueIterator<Iterator, Execution>> unique(const Execution exec = std::execution::seq) const {
            lz::Unique<Iterator, Execution> view = lz::unique(*this, exec);
            return lz::toIter(std::move(view));
        }

        template<class UnaryMapFunc, class UnaryFilterFunc, class Execution = std::execution::sequenced_policy>
        IterView<internal::MapIterator<internal::FilterIterator<Iterator, UnaryFilterFunc, Execution>, UnaryMapFunc>>
        filterMap(UnaryMapFunc mapFunc, UnaryFilterFunc filterFunc, const Execution exec = std::execution::seq) const {
            using MapFilterView = lz::Map<internal::FilterIterator<Iterator, UnaryFilterFunc, Execution>, UnaryMapFunc>;
            MapFilterView view = lz::filterMap(*this, std::move(filterFunc), std::move(mapFunc), exec);
            return lz::toIter(std::move(view));
        }

        template<class SelectorIterable, class Execution = std::execution::sequenced_policy>
        auto select(SelectorIterable&& selectors, const Execution exec = std::execution::seq) const {
            auto selected = lz::select(*this, selectors, exec);
            return lz::toIter(std::move(selected));
        }

        template<class UnaryPredicate, class Execution = std::execution::sequenced_policy>
        IterView<Iterator> dropWhile(UnaryPredicate predicate, const Execution exec = std::execution::seq) const {
            lz::Take<Iterator> view = lz::dropWhile(*this, std::move(predicate), exec);
            return lz::toIter(std::move(view));
        }

        template<class T, class U, class Execution = std::execution::sequenced_policy>
        value_type firstOrDefault(T&& toFind, U&& defaultValue, const Execution exec = std::execution::seq) const {
            return lz::firstOrDefault(*this, toFind, defaultValue, exec);
        }

        template<class UnaryPredicate, class U, class Execution = std::execution::sequenced_policy>
        value_type firstOrDefaultIf(UnaryPredicate predicate, U&& defaultValue, const Execution exec = std::execution::seq) const {
            return lz::firstOrDefault(*this, std::move(predicate), defaultValue, exec);
        }

        template<class T, class U, class Execution = std::execution::sequenced_policy>
        value_type lastOrDefault(T&& toFind, U&& defaultValue, const Execution exec = std::execution::seq) const {
            return lz::lastOrDefault(*this, toFind, defaultValue, exec);
        }

        template<class UnaryPredicate, class U, class Execution = std::execution::sequenced_policy>
        value_type lastOrDefaultIf(UnaryPredicate predicate, U&& defaultValue, const Execution exec = std::execution::seq) const {
            return lz::lastOrDefault(*this, std::move(predicate), defaultValue, exec);
        }

        template<class T, class Execution = std::execution::sequenced_policy>
        difference_type indexOf(const T& value, const Execution exec = std::execution::seq) const {
            return lz::indexOf(*this, value, exec);
        }

        template<class UnaryPredicate, class Execution = std::execution::sequenced_policy>
        difference_type indexOf(UnaryPredicate predicate, const Execution exec = std::execution::seq) const {
            return lz::indexOfIf(*this, std::move(predicate), exec);
        }

        template<class T, class Execution = std::execution::sequenced_policy>
        bool contains(const T& value, const Execution exec = std::execution::seq) const {
            return lz::contains(*this, value, exec);
        }

        template<class UnaryPredicate, class Execution = std::execution::sequenced_policy>
        bool containsIf(UnaryPredicate predicate, const Execution exec = std::execution::seq) const {
            return lz::containsIf(*this, std::move(predicate), exec);
        }

        template<class UnaryFunc, class Execution = std::execution::sequenced_policy>
        const IterView<Iterator>& forEach(UnaryFunc func, const Execution exec = std::execution::seq) const {
            std::for_each(exec, Base::begin(), Base::end(), std::move(func));
            return *this;
        }

        template<class T, class BinaryFunction, class Execution = std::execution::sequenced_policy>
        T foldl(T&& init, BinaryFunction function, const Execution exec = std::execution::seq) const {
            constexpr bool isSequencedPolicy = internal::checkForwardAndPolicies<Execution, Iterator>();
            if constexpr (isSequencedPolicy) {
                static_cast<void>(exec);
                return std::reduce(Base::begin(), Base::end(), std::forward<T>(init), std::move(function));
            }
            else {
                return std::reduce(exec, Base::begin(), Base::end(), std::forward<T>(init), std::move(function));
            }
        }

        template<class T, class BinaryFunction, class Execution = std::execution::sequenced_policy>
        T foldr(T&& init, BinaryFunction function, const Execution exec = std::execution::seq) const {
            constexpr bool isSequencedPolicy = internal::checkForwardAndPolicies<Execution, Iterator>();
            IterView<std::reverse_iterator<Iterator>> reverseView = this->reverse();
            if constexpr (isSequencedPolicy) {
                static_cast<void>(exec);
                return std::reduce(reverseView.begin(), reverseView.end(), std::forward<T>(init), std::move(function));
            }
            else {
                return std::reduce(exec, reverseView.begin(), reverseView.end(), std::forward<T>(init), std::move(function));
            }
        }

        template<class Execution = std::execution::sequenced_policy>
        value_type sum(const Execution exec = std::execution::seq) const {
            return this->foldl(value_type(), std::plus<value_type>(), exec);
        }

        template<class UnaryPredicate, class Execution = std::execution::sequenced_policy>
        bool all(UnaryPredicate predicate, const Execution exec = std::execution::seq) {
            constexpr bool isSequencedPolicy = internal::checkForwardAndPolicies<Execution, Iterator>();
            if constexpr (isSequencedPolicy) {
                static_cast<void>(exec);
                return std::all_of(Base::begin(), Base::end(), std::move(predicate));
            }
            else {
                return std::all_of(exec, Base::begin(), Base::end(), std::move(predicate));
            }
        }

        template<class UnaryPredicate, class Execution = std::execution::sequenced_policy>
        bool any(UnaryPredicate predicate, const Execution exec = std::execution::seq) {
            constexpr bool isSequencedPolicy = internal::checkForwardAndPolicies<Execution, Iterator>();
            if constexpr (isSequencedPolicy) {
                static_cast<void>(exec);
                return std::any_of(Base::begin(), Base::end(), std::move(predicate));
            }
            else {
                return std::any_of(exec, Base::begin(), Base::end(), std::move(predicate));
            }
        }

        template<class UnaryPredicate, class Execution = std::execution::sequenced_policy>
        bool none(UnaryPredicate predicate, const Execution exec = std::execution::seq) {
            return !this->all(std::move(predicate), exec);
        }

#else // ^^^ lz has execution vvv ! lz has execution
        template<class UnaryPredicate>
        IterView<internal::FilterIterator<Iterator, UnaryPredicate>> filter(UnaryPredicate predicate) const {
            using FilterView = lz::Filter<Iterator, UnaryPredicate>;
            FilterView view = lz::filter(*this, std::move(predicate));
            return lz::toIter(std::move(view));
        }

        template<class IterableToExcept>
        IterView<internal::ExceptIterator<Iterator, internal::IterTypeFromIterable<IterableToExcept>>>
        except(IterableToExcept&& toExcept) const {
            using ExceptView = lz::Except<Iterator, internal::IterTypeFromIterable<IterableToExcept>>;
            ExceptView view = lz::except(*this, toExcept);
            return lz::toIter(std::move(view));
        }

        IterView<internal::UniqueIterator<Iterator>> unique() const {
            lz::Unique<Iterator> view = lz::unique(*this);
            return lz::toIter(std::move(view));
        }

        template<class UnaryMapFunc, class UnaryFilterFunc>
        IterView<internal::MapIterator<internal::FilterIterator<Iterator, UnaryFilterFunc>, UnaryMapFunc>>
        filterMap(UnaryMapFunc mapFunc, UnaryFilterFunc filterFunc) const {
            using MapFilterView = lz::Map<internal::FilterIterator<Iterator, UnaryFilterFunc>, UnaryMapFunc>;
            MapFilterView view = lz::filterMap(*this, std::move(filterFunc), std::move(mapFunc));
            return lz::toIter(std::move(view));
        }

        template<class SelectorIterable
#ifdef LZ_HAS_CXX11
            , class Iterator = internal::IterTypeFromIterable<Iterable>,
            class SelectorIterator = internal::IterTypeFromIterable<SelectorIterable>,
            class ZipIter = typename lz::Zip<Iterator, SelectorIterator>::iterator,
            class RefTuple = internal::RefType<ZipIter>
#endif // end lz has cxx11
        >
        auto select(SelectorIterable&& selectors)
#ifdef LZ_HAS_CXX11
        -> IterView<internal::MapIterator<internal::FilterIterator<internal::ZipIterator<Iterator, SelectorIterator>,
                                                                   std::function<bool(RefTuple)>>,
                                          std::function<internal::RefType<Iterator>(RefTuple)>>>
#endif // end lz has cxx11
        {
            auto select = lz::select(*this, selectors);
            return lz::toIter(std::move(select));
        }

        template<class UnaryPredicate>
        IterView<Iterator> dropWhile(UnaryPredicate predicate) const {
            lz::Take<Iterator> view = lz::dropWhile(*this, std::move(predicate));
            return lz::toIter(std::move(view));
        }

        template<class T, class U>
        value_type firstOrDefault(T&& toFind, U&& defaultValue) const {
            return lz::firstOrDefault(*this, toFind, defaultValue);
        }

        template<class UnaryPredicate, class U>
        value_type firstOrDefaultIf(UnaryPredicate predicate, U&& defaultValue) const {
            return lz::firstOrDefault(*this, std::move(predicate), defaultValue);
        }

        template<class T, class U>
        value_type lastOrDefault(T&& toFind, U&& defaultValue) const {
            return lz::lastOrDefault(*this, toFind, defaultValue);
        }

        template<class UnaryPredicate, class U>
        value_type lastOrDefaultIf(UnaryPredicate predicate, U&& defaultValue) const {
            return lz::lastOrDefault(*this, std::move(predicate), defaultValue);
        }

        template<class T>
        difference_type indexOf(const T& value) const {
            return lz::indexOf(*this, value);
        }

        template<class UnaryPredicate>
        difference_type indexOf(UnaryPredicate predicate) const {
            return lz::indexOfIf(*this, std::move(predicate));
        }

        template<class T>
        bool contains(const T& value) const {
            return lz::contains(*this, value);
        }

        template<class UnaryPredicate>
        bool containsIf(UnaryPredicate predicate) const {
            return lz::containsIf(*this, std::move(predicate));
        }

        template<class UnaryFunc>
        const IterView<Iterator>& forEach(UnaryFunc func) const {
            std::for_each(Base::begin(), Base::end(), std::move(func));
            return *this;
        }

        template<class T, class BinaryFunction>
        T foldl(T&& init, BinaryFunction function) const {
            return std::accumulate(Base::begin(), Base::end(), std::forward<T>(init), std::move(function));
        }

        template<class T, class BinaryFunction>
        T foldr(T&& init, BinaryFunction function) const {
            IterView<std::reverse_iterator<Iterator>> reverseView = this->reverse();
            return std::accumulate(reverseView.begin(), reverseView.end(), std::forward<T>(init), std::move(function));
        }

        value_type sum() const {
            return this->foldl(value_type(), std::plus<value_type>());
        }

        template<class UnaryPredicate>
        bool all(UnaryPredicate predicate) {
            return std::all_of(Base::begin(), Base::end(), std::move(predicate));
        }

        template<class UnaryPredicate>
        bool any(UnaryPredicate predicate) {
            return std::any_of(Base::begin(), Base::end(), std::move(predicate));
        }

        template<class UnaryPredicate>
        bool none(UnaryPredicate predicate) {
            return !this->all(std::move(predicate));
        }
#endif // end lz has execution
    };

    template<LZ_CONCEPT_ITERABLE Iterable>
    IterView<internal::IterTypeFromIterable<Iterable>> toIter(Iterable&& iterable) {
        return lz::toIter(std::begin(iterable), std::end(iterable));
    }

    template<LZ_CONCEPT_ITERATOR Iterator>
    IterView<Iterator> toIter(Iterator begin, Iterator end) {
        return lz::IterView<Iterator>(std::move(begin), std::move(end));
    }
}

#endif // LZ_LZ_HPP