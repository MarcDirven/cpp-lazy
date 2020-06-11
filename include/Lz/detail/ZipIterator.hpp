#pragma once

#include <iterator>


namespace lz { namespace detail {
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
}}
