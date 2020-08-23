#pragma once

#include <iterator>
#include <algorithm>
#include <iostream>


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
        std::tuple<Iterators...> _iterators{};

        template<size_t... I>
        reference dereference(std::index_sequence<I...> /*is*/) const {
            return reference{*std::get<I>(_iterators)...};
        }

        template<size_t... I>
        void increment(std::index_sequence<I...> /*is*/) {
            auto expand = {(++std::get<I>(_iterators), 0)...};
            static_cast<void>(expand);
        }

        template<size_t... I>
        void decrement(std::index_sequence<I...> /*is*/) {
            auto expand = {(--std::get<I>(_iterators), 0)...};
            static_cast<void>(expand);
        }

        template<size_t... I>
        void plusIs(std::index_sequence<I...> /*is*/, const difference_type differenceType) {
            auto expand = {(std::get<I>(_iterators) = std::next(std::get<I>(_iterators), differenceType), 0)...};
            static_cast<void>(expand);
        }

        template<size_t... I>
        void minIs(std::index_sequence<I...> /*is*/, const difference_type differenceType) {
            auto expand = {(std::get<I>(_iterators) = std::prev(std::get<I>(_iterators), differenceType), 0)...};
            static_cast<void>(expand);
        }

        template<size_t... I>
        difference_type iteratorMin(std::index_sequence<I...> /*is*/, const ZipIterator& other) const {
            return static_cast<difference_type>(std::min(
                {(std::distance(std::get<I>(other._iterators), std::get<I>(_iterators)))...}));
        }

        template<size_t... I>
        bool lessThan(std::index_sequence<I...> /*is*/, const ZipIterator& other) const {
            std::initializer_list<difference_type> distances = {(std::distance(std::get<I>(_iterators), std::get<I>(other._iterators)))...};
            return std::find_if(distances.begin(), distances.end(), [](const difference_type diff) {
                return diff > 0;
            }) != distances.end();
        }

        template<size_t... I>
        bool notEqual(std::index_sequence<I...> /*is*/, const ZipIterator& other) const {
            std::initializer_list<bool> boolValues = {(std::get<I>(_iterators) != std::get<I>(other._iterators))...};
            auto end = boolValues.end();
            // Check if false not in boolValues
            return std::find(boolValues.begin(), end, false) == end;
        }

    public:
        explicit ZipIterator(const std::tuple<Iterators...>& iterators) :
            _iterators(iterators) {
        }

        reference operator*() const {
            return dereference(std::index_sequence_for<Iterators...>{});
        }

        pointer operator->() const {
            return FakePointerProxy<decltype(**this)>(**this);
        }

        ZipIterator& operator++() {
            increment(std::index_sequence_for<Iterators...>{});
            return *this;
        }

        ZipIterator operator++(int) {
            ZipIterator tmp(*this);
            ++*this;
            return tmp;
        }

        ZipIterator& operator--() {
            decrement(std::index_sequence_for<Iterators...>{});
            return *this;
        }

        ZipIterator operator--(int) {
            auto tmp(*this);
            --*this;
            return tmp;
        }

        ZipIterator& operator+=(const difference_type offset) {
            plusIs(std::index_sequence_for<Iterators...>{}, offset);
            return *this;
        }

        ZipIterator operator+(const difference_type offset) const {
            ZipIterator tmp(*this);
            tmp += offset;
            return tmp;
        }

        ZipIterator& operator-=(const difference_type offset) {
            minIs(std::index_sequence_for<Iterators...>{}, offset);
            return *this;
        }

        ZipIterator operator-(const difference_type offset) const {
            ZipIterator tmp(*this);
            tmp -= offset;
            return tmp;
        }

        difference_type operator-(const ZipIterator& other) const {
            return iteratorMin(std::index_sequence_for<Iterators...>{}, other);
        }

        reference operator[](const difference_type offset) const {
            return *(*this + offset);
        }

        bool operator==(const ZipIterator& other) const {
            return !(*this != other);
        }

        bool operator!=(const ZipIterator& other) const {
            return notEqual(std::index_sequence_for<Iterators...>{}, other);
        }

        bool operator<(const ZipIterator& other) const {
            return lessThan(std::index_sequence_for<Iterators...>{}, other);
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
