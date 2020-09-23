#pragma once

#ifndef LZ_ZIP_ITERATOR_HPP
#define LZ_ZIP_ITERATOR_HPP

#include <iterator>
#include <algorithm>


namespace lz { namespace detail {
    template<class... Iterators>
    class ZipIterator {
    public:
        using iterator_category = std::random_access_iterator_tag;
        using value_type = std::tuple<typename std::iterator_traits<Iterators>::value_type...>;
        using difference_type = std::ptrdiff_t;
        using reference = std::tuple<typename std::iterator_traits<Iterators>::reference...>;
        using pointer = FakePointerProxy<reference>;

    private:
        using MakeIndexSequence = std::index_sequence_for<Iterators...>;
        std::tuple<Iterators...> _iterators{};

        template<size_t... I>
        reference dereference(std::index_sequence<I...> /*is*/) const {
            return reference{*std::get<I>(_iterators)...};
        }

        template<size_t... I>
        void increment(std::index_sequence<I...> /*is*/) {
            const std::initializer_list<int> expand = {(++std::get<I>(_iterators), 0)...};
            static_cast<void>(expand);
        }

        template<size_t... I>
        void decrement(std::index_sequence<I...> /*is*/) {
            const std::initializer_list<int> expand = {(--std::get<I>(_iterators), 0)...};
            static_cast<void>(expand);
        }

        template<size_t... I>
        void plusIs(std::index_sequence<I...> /*is*/, const difference_type differenceType) {
            const std::initializer_list<int> expand = {(std::get<I>(_iterators) = std::next(std::get<I>(_iterators), differenceType), 0)...};
            static_cast<void>(expand);
        }

        template<size_t... I>
        void minIs(std::index_sequence<I...> /*is*/, const difference_type differenceType) {
            const  std::initializer_list<int> expand = {(std::get<I>(_iterators) = std::prev(std::get<I>(_iterators), differenceType), 0)...};
            static_cast<void>(expand);
        }

        template<size_t... I>
        difference_type iteratorMin(std::index_sequence<I...> /*is*/, const ZipIterator& other) const {
            const std::initializer_list<difference_type> diff =
                {static_cast<difference_type>((std::distance(std::get<I>(other._iterators), std::get<I>(_iterators))))...};
            return static_cast<difference_type>(std::min(diff));
        }

        template<size_t... I>
        bool lessThan(std::index_sequence<I...> /*is*/, const ZipIterator& other) const {
            const std::initializer_list<difference_type> distances = {(std::distance(std::get<I>(_iterators), std::get<I>(other._iterators)))...};
            return std::find_if(distances.begin(), distances.end(), [](const difference_type diff) {
                return diff > 0;
            }) != distances.end();
        }

        template<size_t... I>
        bool notEqual(std::index_sequence<I...> /*is*/, const ZipIterator& other) const {
            const std::initializer_list<bool> boolValues = {(std::get<I>(_iterators) != std::get<I>(other._iterators))...};
            const auto* const end = boolValues.end();
            // Check if false not in boolValues
            return std::find(boolValues.begin(), end, false) == end;
        }

    public:
        explicit ZipIterator(const std::tuple<Iterators...>& iterators) :  // NOLINT(modernize-pass-by-value)
            _iterators(iterators) {
        }

        ZipIterator() = default;

        reference operator*() const {
            return dereference(MakeIndexSequence{});
        }

        pointer operator->() const {
            return FakePointerProxy<decltype(**this)>(**this);
        }

        ZipIterator& operator++() {
            increment(MakeIndexSequence{});
            return *this;
        }

        ZipIterator operator++(int) {
            ZipIterator tmp(*this);
            ++*this;
            return tmp;
        }

        ZipIterator& operator--() {
            decrement(MakeIndexSequence{});
            return *this;
        }

        ZipIterator operator--(int) {
            auto tmp(*this);
            --*this;
            return tmp;
        }

        ZipIterator& operator+=(const difference_type offset) {
            plusIs(MakeIndexSequence{}, offset);
            return *this;
        }

        ZipIterator operator+(const difference_type offset) const {
            ZipIterator tmp(*this);
            tmp += offset;
            return tmp;
        }

        ZipIterator& operator-=(const difference_type offset) {
            minIs(MakeIndexSequence{}, offset);
            return *this;
        }

        ZipIterator operator-(const difference_type offset) const {
            ZipIterator tmp(*this);
            tmp -= offset;
            return tmp;
        }

        difference_type operator-(const ZipIterator& other) const {
            return iteratorMin(MakeIndexSequence{}, other);
        }

        reference operator[](const difference_type offset) const {
            return *(*this + offset);
        }

        bool operator==(const ZipIterator& other) const {
            return !(*this != other);
        }

        bool operator!=(const ZipIterator& other) const {
            return notEqual(MakeIndexSequence{}, other);
        }

        bool operator<(const ZipIterator& other) const {
            return lessThan(MakeIndexSequence{}, other);
        }

        bool operator>(const ZipIterator& other) const {
            return other < *this;
        }

        bool operator<=(const ZipIterator& other) const {
            return !(other < *this);
        }

        bool operator>=(const ZipIterator& other) const {
            return !(*this < other);
        }
    };
}}

#endif