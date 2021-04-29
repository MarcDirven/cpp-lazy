#pragma once

#ifndef LZ_ZIP_ITERATOR_HPP
#define LZ_ZIP_ITERATOR_HPP

#include <algorithm>
#include "LzTools.hpp"


namespace lz { namespace internal {
    template<LZ_CONCEPT_ITERATOR... Iterators>
    class ZipIterator {
    public:
        using iterator_category = LowestIterTypeT<IterCat<Iterators>...>;
        using value_type = std::tuple<ValueType<Iterators>...>;
        using difference_type = std::ptrdiff_t;
        using reference = std::tuple<RefType<Iterators>...>;
        using pointer = std::tuple<PointerType<Iterators>...>;

    private:
        using MakeIndexSequenceForThis = MakeIndexSequence<sizeof...(Iterators)>;
        std::tuple<Iterators...> _iterators{};

        template<std::size_t... I>
        reference dereference(IndexSequence<I...>) const {
            return {*std::get<I>(_iterators)...};
        }

        template<std::size_t... I>
        void increment(IndexSequence<I...>) {
            const int expand[] = {(++std::get<I>(_iterators), 0)...};
            static_cast<void>(expand);
        }

        template<std::size_t... I>
        void decrement(IndexSequence<I...>) {
            const int expand[] = {(--std::get<I>(_iterators), 0)...};
            static_cast<void>(expand);
        }

        template<std::size_t... I>
        void plusIs(IndexSequence<I...>, const difference_type differenceType) {
            const int expand[] = { ((std::get<I>(_iterators) += differenceType), 0)... };
            static_cast<void>(expand);
        }

        template<std::size_t... I>
        void minIs(IndexSequence<I...>, const difference_type differenceType) {
            plusIs(MakeIndexSequenceForThis(), -differenceType);
        }

        template<std::size_t... I>
        difference_type iteratorMin(IndexSequence<I...>, const ZipIterator& other) const {
            const std::initializer_list<difference_type> diff =
                { static_cast<difference_type>(std::get<I>(_iterators) - std::get<I>(other._iterators)) ...};
            return static_cast<difference_type>(std::min(diff));
        }

        template<std::size_t... I>
        bool lessThan(IndexSequence<I...>, const ZipIterator& other) const {
            const bool distances[] = { (std::get<I>(_iterators) < std::get<I>(other._iterators)) ...};
			const auto* end = std::end(distances);
            return std::find(std::begin(distances), end, true) != end;
        }

        template<std::size_t... I>
        bool notEqual(IndexSequence<I...>, const ZipIterator& other) const {
            const bool boolValues[] = {(std::get<I>(_iterators) != std::get<I>(other._iterators))...};
            const auto* const end = std::end(boolValues);
            // Check if false not in boolValues
            return std::find(std::begin(boolValues), end, false) == end;
        }

    public:
        explicit ZipIterator(std::tuple<Iterators...> iterators) :
            _iterators(std::move(iterators)) {
        }

        ZipIterator() = default;

        reference operator*() const {
            return dereference(MakeIndexSequenceForThis());
        }

        pointer operator->() const {
            return FakePointerProxy<decltype(**this)>(**this);
        }

        ZipIterator& operator++() {
            increment(MakeIndexSequenceForThis());
            return *this;
        }

        ZipIterator operator++(int) {
            ZipIterator tmp(*this);
            ++*this;
            return tmp;
        }

        ZipIterator& operator--() {
            decrement(MakeIndexSequenceForThis());
            return *this;
        }

        ZipIterator operator--(int) {
            auto tmp(*this);
            --*this;
            return tmp;
        }

        ZipIterator& operator+=(const difference_type offset) {
            plusIs(MakeIndexSequenceForThis(), offset);
            return *this;
        }

        ZipIterator operator+(const difference_type offset) const {
            ZipIterator tmp(*this);
            tmp += offset;
            return tmp;
        }

        ZipIterator& operator-=(const difference_type offset) {
            minIs(MakeIndexSequenceForThis(), offset);
            return *this;
        }

        ZipIterator operator-(const difference_type offset) const {
            ZipIterator tmp(*this);
            tmp -= offset;
            return tmp;
        }

        difference_type operator-(const ZipIterator& other) const {
            return iteratorMin(MakeIndexSequenceForThis(), other);
        }

        reference operator[](const difference_type offset) const {
            return *(*this + offset);
        }

        friend bool operator==(const ZipIterator& a, const ZipIterator& b) {
            return !(a != b); // NOLINT
        }

        friend bool operator!=(const ZipIterator& a, const ZipIterator& b) {
            return a.notEqual(MakeIndexSequenceForThis(), b);
        }

        friend bool operator<(const ZipIterator& a, const ZipIterator& b) {
            return a.lessThan(MakeIndexSequenceForThis(), b);
        }

        friend bool operator>(const ZipIterator& a, const ZipIterator& b) {
            return b < a;
        }

        friend bool operator<=(const ZipIterator& a, const ZipIterator& b) {
            return !(b < a); // NOLINT
        }

        friend bool operator>=(const ZipIterator& a, const ZipIterator& b) {
            return !(a < b); // NOLINT
        }
    };
}}

#endif