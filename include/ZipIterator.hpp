#pragma once


#include "Tools.hpp"


namespace detail {
    template<size_t I, class IteratorTuple>
    struct NeqTup {
        bool operator()(const IteratorTuple& a, const IteratorTuple& b) {
            return NeqTup<I - 1, IteratorTuple>{}(a, b) && NeqTup<I - 2, IteratorTuple>{}(a, b);
        }
    };

    template<class IteratorTuple>
    struct NeqTup<1, IteratorTuple> {
        bool operator()(const IteratorTuple& a, const IteratorTuple& b) {
            return std::get<0>(a) != std::get<0>(b) && std::get<1>(a) != std::get<1>(b);
        }
    };

    template<class Tuple>
    struct NeqTup<0, Tuple> {
        bool operator()(const Tuple& a, const Tuple& b) {
            return std::get<0>(a) != std::get<0>(b);
        }
    };

    template<class IteratorTuple>
    bool neqtup(const IteratorTuple& a, const IteratorTuple& b) {
        return NeqTup<std::tuple_size<IteratorTuple>::value, IteratorTuple>{}(a, b);
    }

    template<class... Containers>
    class ZipIterator {
    public:
        using iterator_category = std::bidirectional_iterator_tag;
        using value_type = std::tuple<decltype(*std::declval<Containers>().begin())...>;
        using difference_type = std::ptrdiff_t;
        using pointer = value_type;
        using reference = value_type;

    private:
        std::tuple<decltype(std::declval<Containers>().begin())...> _iterators;

        template<size_t... I>
        value_type dereference(std::index_sequence<I...>) const {
            return value_type{*std::get<I>(_iterators)...};
        }

        template<size_t... I>
        void increment(std::index_sequence<I...>) {
            auto expand = {(++std::get<I>(_iterators), 0)...};
            (void) expand;
        }

        template<size_t... I>
        void decrement(std::index_sequence<I...>) {
            auto expand = {(--std::get<I>(_iterators), 0)...};
            (void) expand;
        }

        template<size_t... I>
        void plusIs(std::index_sequence<I...>, const difference_type differenceType) {
            auto expand = {(std::get<I>(_iterators) += differenceType, 0)...};
            (void) expand;
        }

        template<size_t... I>
        void minIs(std::index_sequence<I...>, const difference_type differenceType) {
            auto expand = {(std::get<I>(_iterators) -= differenceType, 0)...};
            (void) expand;
        }

    public:
        explicit ZipIterator(decltype(_iterators) iters) :
            _iterators(iters) {
        }

        value_type operator*() const {
            return dereference(std::index_sequence_for<Containers...>{});
        }

        bool operator==(const ZipIterator& other) const {
            return !(*this != other);
        }

        bool operator!=(const ZipIterator& other) const {
            return neqtup<decltype(_iterators)>(_iterators, other._iterators);
        }

        ZipIterator& operator++() {
            increment(std::index_sequence_for<Containers...>{});
            return *this;
        }

        ZipIterator& operator--() {
            decrement(std::index_sequence_for<Containers...>{});
            return *this;
        }

        ZipIterator& operator+=(const difference_type offset) {
            plusIs(std::index_sequence_for<Containers...>{}, offset);
            return *this;
        }

        ZipIterator operator+(const difference_type offset) {
            auto tmp(*this);
            tmp += offset;
            return tmp;
        }

        ZipIterator& operator-=(const difference_type offset) {
            minIs(std::index_sequence_for<Containers...>{}, offset);
            return *this;
        }

        ZipIterator operator-(const difference_type offset) {
            auto tmp(*this);
            tmp -= offset;
            return *this;
        }
    };
}

namespace lz {
    template<class... Containers>
    class ZipObject {
    public:
        using value_type = std::tuple<decltype(*std::declval<Containers>().begin())...>;
        using size_type = size_t;
        using difference_type = std::ptrdiff_t;
        using pointer = std::tuple<typename std::decay_t<Containers>::pointer...>;
        using const_pointer = std::tuple<typename std::decay_t<Containers>::const_pointer...>;
        using reference = std::tuple<typename std::decay_t<Containers>::reference...>;
        using const_reference = std::tuple<typename std::decay_t<Containers>::const_reference...>;

        using iterator = detail::ZipIterator<Containers...>;
        using const_iterator = iterator;

    private:
        iterator _begin;
        iterator _end;

    public:
        explicit ZipObject(Containers&& ... containers) :
            _begin(std::make_tuple(containers.begin()...)),
            _end(std::make_tuple(containers.end()...)) {
        }

        iterator begin() const {
            return _begin;
        }

        iterator end() const {
            return _end;
        }

        template<template<class, class...> class ContainerType, class... Args>
        ContainerType<value_type, Args...> to() const {
            return ContainerType<value_type, Args...>(begin(), end());
        }

        std::vector<value_type> toVector() const {
            return to<std::vector>();
        }

        template<size_t N>
        std::array<value_type, N> toArray() const {
            std::array<value_type, N> container{};
            detail::fillContainer(begin(), container);
            return container;
        }
    };

    template<class... Containers>
    ZipObject<Containers...> zip(Containers&& ... containers) {
        return ZipObject<Containers...>(containers...);
    }
}
