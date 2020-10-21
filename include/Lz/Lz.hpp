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
            auto filterMapView = lz::filterMap(*this, selectors);
            return lz::toIter(std::move(filterMapView));
        }
#endif // end lz has execution

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

        template<class T, class BinaryFunction>
        T foldl(T&& init, BinaryFunction function) const {
            return std::accumulate(Base::begin(), Base::end(), std::forward<T>(init), std::move(function));
        }

        template<class T, class BinaryFunction>
        T foldr(T&& init, BinaryFunction function) const {
            IterView<std::reverse_iterator<Iterator>> reverseView = this->reverse();
            return std::accumulate(reverseView.begin(), reverseView.end(), std::forward<T>(init), std::move(function));
        }
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