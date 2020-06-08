#pragma once


#include "Tools.hpp"

namespace detail {
    template<size_t I, typename Tuple>
    struct NeqTup {
        bool operator()(const Tuple& a, const Tuple& b) {
            return NeqTup<I - 1, Tuple>{}(a, b) && NeqTup<I - 2, Tuple>{}(a, b);
        }
    };

    template<typename Tuple>
    struct NeqTup<1, Tuple> {
        bool operator()(const Tuple& a, const Tuple& b) {
            return std::get<0>(a) != std::get<0>(b) && std::get<1>(a) != std::get<1>(b);
        }
    };

    template<typename Tuple>
    struct NeqTup<0, Tuple> {
        bool operator()(const Tuple& a, const Tuple& b) {
            return std::get<0>(a) != std::get<0>(b);
        }
    };

    template<typename Tuple>
    bool neqtup(const Tuple& a, const Tuple& b) {
        return NeqTup<std::tuple_size<Tuple>::value - 1, Tuple>{}(a, b);
    }

    template<typename... Containers>
    class ZipIterator {
    public:
        using iterator_category = std::bidirectional_iterator_tag;
        using value_type = std::tuple<decltype(*std::declval<Containers>().begin())...>;
        using difference_type = ptrdiff_t ;
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
            (void)expand;
        }

        template<size_t... I>
        void decrement(std::index_sequence<I...>) {
            auto expand = {(--std::get<I>(_iterators), 0)...};
            (void)expand;
        }

    public:
        explicit ZipIterator(decltype(_iterators) iters) :
            _iterators(std::move(iters))
        {
        }

        value_type operator*() const {
            return dereference(std::index_sequence_for<Containers...>{});
        }

        bool operator==(const ZipIterator& other) const {
            return !(*this == other);
        }

        bool operator!=(const ZipIterator& other) const {
            return neqtup<decltype(_iterators)>(_iterators, other._iterators);
        }

        ZipIterator& operator++() {
            increment(std::index_sequence_for<Containers...>{});
            return *this;
        }

        ZipIterator operator++(int) {
            auto tmp(*this);
            increment(std::index_sequence_for<Containers...>{});
            return tmp;
        }

        ZipIterator& operator--() {
            decrement(std::index_sequence_for<Containers...>{});
            return *this;
        }

        ZipIterator operator--(int) {
            auto tmp(*this);
            decrement(std::index_sequence_for<Containers...>{});
            return tmp;
        }
    };
}
namespace lz {
    template<typename... Containers>
    class ZipObject {
    public:
        using value_type = std::tuple<decltype(*std::declval<Containers>().begin())...>;
        using size_type = std::tuple<typename std::decay_t<Containers>::size_type...>;
        using difference_type = std::tuple<typename std::decay_t<Containers>::difference_type...>;
        using pointer = std::tuple<typename std::decay_t<Containers>::pointer...>;
        using const_pointer = std::tuple<typename std::decay_t<Containers>::const_pointer...>;
        using reference = std::tuple<typename std::decay_t<Containers>::reference...>;
        using const_reference = std::tuple<typename std::decay_t<Containers>::const_reference...>;

        using iterator = detail::ZipIterator<Containers...>;

    private:
        iterator _begin;
        iterator _end;

    public:
        explicit ZipObject(Containers&& ... containers) :
            _begin(std::make_tuple(containers.begin()...)),
            _end(std::make_tuple(containers.end()...)){
        }

        iterator begin() const {
            return _begin;
        }

        iterator end() const {
            return _end;
        }
    };

    template<typename... Containers>
    ZipObject<Containers...> zip(Containers&& ... containers) {
        return ZipObject<Containers...>(containers...);
    }
}
