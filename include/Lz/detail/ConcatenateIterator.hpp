#pragma once

#include <iterator>
#include <Lz/detail/LzTools.hpp>


namespace lz { namespace detail {
#ifdef CXX_LT_17
    template<typename Same, typename First, typename... More>
    struct IsAllSame {
        static const bool value = std::is_same<Same, First>::value && IsAllSame<First, More...>::value;
    };

    template<typename Same, typename First>
    struct IsAllSame<Same, First> : std::is_same<Same, First> {
    };
#endif


    template<class Tuple, size_t I, class = void>
    struct ApplyCurrentIterator {
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4715)
#else
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wreturn-type"
#endif

        template<class LastValue = void, class Func>
        auto operator()(Tuple begin, Tuple end, Func func, LastValue last) const -> decltype(func(std::get<I>(begin),
                                                                                                  std::get<I>(end))) {
            if (std::get<I>(begin) != std::get<I>(end)) {
                return func(std::get<I>(begin), std::get<I>(end));
            }
            ApplyCurrentIterator<Tuple, I + 1>()(begin, end, func, last);
        }

#ifdef _MSC_VER
#pragma warning(pop)
#else
#pragma GCC diagnostic pop
#endif
    }; // end ApplyCurrentIterator

    template<class Tuple, size_t I>
    struct ApplyCurrentIterator<Tuple, I, typename std::enable_if<
        I == std::tuple_size<std::decay_t<Tuple>>::value>::type> {
        template<class LastValue = void, class Func>
        auto operator()(Tuple, Tuple, Func, LastValue lastValue) const {
            return lastValue;
        }
    };

    template<class Tuple, size_t I>
    struct MinusMinus {
        void operator()(Tuple& begin, Tuple& end) const {
            if (std::get<I>(begin) == std::get<I>(end)) {
                --std::get<I>(begin);
            }
            else {
                MinusMinus<Tuple, I - 1>()(begin, end);
            }
        }
    };

    template<class Tuple>
    struct MinusMinus<Tuple, 0> {
        void operator()(Tuple& begin, Tuple&) const {
            --std::get<0>(begin);
        }
    };

    template<class Tuple, size_t I>
    struct MinIs {
        template<class DifferenceType>
        void operator()(Tuple& iterators, Tuple& begin, Tuple& end, const DifferenceType offset) {
            auto current = std::get<I>(iterators);
            auto currentBegin = std::get<I>(begin);

            // Current is begin, move on to next iterator
            if (current == currentBegin) {
                MinIs<Tuple, I - 1>()(iterators, begin, end, offset);
            }
            else {
                auto distance = static_cast<DifferenceType>(std::distance(std::get<I>(begin), std::get<I>(end)));
                if (distance <= offset) {
                    std::get<I>(iterators) = std::get<I>(begin);
                    MinIs<Tuple, I - 1>()(iterators, begin, end, distance == 0 ? 1 : offset - distance);
                }
                else {
                    std::get<I>(iterators) = std::prev(std::get<I>(iterators), offset);
                }
            }
        }
    };

    template<class Tuple>
    struct MinIs<Tuple, 0> {
        template<class DifferenceType>
        void operator()(Tuple& iterators, Tuple& begin, Tuple& end, const DifferenceType offset) {
            auto& current = std::get<0>(iterators);
            auto currentBegin = std::get<0>(begin);
            auto distance = static_cast<DifferenceType>(std::distance(current, std::get<0>(end)));

            // first iterator is at position begin, and distance bigger than 0
            if (current == currentBegin && distance > 0) {
                throw std::out_of_range("cannot access elements before begin");
            }
            current = std::prev(current, offset);
        }
    };

    template<class Tuple, size_t I, class = void>
    struct PlusIs {
        template<class DifferenceType>
        void operator()(Tuple& iterators, const Tuple& end, const DifferenceType offset) {
            auto& currentIterator = std::get<I>(iterators);
            auto currentEnd = std::get<I>(end);

            auto distance = static_cast<DifferenceType>(std::distance(currentIterator, currentEnd));

            if (distance > offset) {
                currentIterator = std::next(currentIterator, offset);
            }
            else {
                currentIterator = currentEnd;
                PlusIs<Tuple, I + 1>()(iterators, end, offset - distance);
            }
        }
    };

    template<class Tuple, size_t I>
    struct PlusIs<Tuple, I, typename std::enable_if<I == std::tuple_size<std::decay_t<Tuple>>::value>::type> {
        template<class DifferenceType>
        void operator()(Tuple&, const Tuple&, const DifferenceType) {
        }
    };


    template<class... Iterators>
    class ConcatenateIterator {
        using IterTuple = std::tuple<Iterators...>;
        IterTuple _iterators{};
        IterTuple _begin{};
        IterTuple _end{};

        using FirstTupleIterator = std::iterator_traits<std::decay_t<decltype(std::get<0>(_iterators))>>;

    public:
        using value_type = typename FirstTupleIterator::value_type;
        using difference_type = std::ptrdiff_t;
        using reference = typename FirstTupleIterator::reference;
        using pointer = typename FirstTupleIterator::pointer;
        using iterator_category = std::random_access_iterator_tag;

        static_assert(sizeof...(Iterators) >= 2, "amount of iterators/containers cannot be less than or equal to 1");
#ifdef CXX_LT_17
        static_assert(IsAllSame<typename std::iterator_traits<Iterators>::value_type...>::value,
                      "value types of iterators do not match");
#else
        static_assert(std::conjunction<std::is_same<value_type,
                                                    typename std::iterator_traits<Iterators>::value_type>...>::value,
                      "value types of iterators do not match");
#endif
    private:
        template<size_t... I>
        difference_type minus(std::index_sequence<I...>, const ConcatenateIterator& other) const {
            std::array<difference_type, sizeof...(Iterators)> totals = {
                static_cast<difference_type>(std::distance(std::get<I>(other._iterators), std::get<I>(_end)))...};

            return std::accumulate(totals.begin(), totals.end(), static_cast<difference_type>(0));
        }

    public:
        ConcatenateIterator(const IterTuple& iterators, const IterTuple& end) :  // NOLINT(modernize-pass-by-value)
            _iterators(iterators),
            _begin(iterators),
            _end(end) {
        }

        reference operator*() const {
            return ApplyCurrentIterator<const IterTuple&, 0>()(_iterators, _end,
                                                               [](const auto it, const auto) -> reference {
                                                                   return *it;
                                                               }, 0);
        }

        ConcatenateIterator& operator++() {
            ApplyCurrentIterator<IterTuple&, 0>()(_iterators, _end, [](auto& it, auto&) -> void {
                ++it;
            }, 0);
            return *this;
        }

        ConcatenateIterator operator++(int) {
            auto tmp(*this);
            ++*this;
            return tmp;
        }

        ConcatenateIterator& operator--() {
            MinusMinus<IterTuple, sizeof...(Iterators) - 1>()(_iterators, _end);
            return *this;
        }

        ConcatenateIterator operator--(int) {
            auto tmp(*this);
            ++*this;
            return tmp;
        }

        ConcatenateIterator& operator+=(const difference_type offset) {
            PlusIs<IterTuple, 0>()(_iterators, _end, offset);
            return *this;
        }

        ConcatenateIterator& operator-=(const difference_type offset) {
            MinIs<IterTuple, sizeof...(Iterators) - 1>()(_iterators, _begin, _end, offset);
            return *this;
        }

        ConcatenateIterator operator+(const difference_type offset) const {
            auto tmp(*this);
            tmp += offset;
            return tmp;
        }

        ConcatenateIterator operator-(const difference_type offset) const {
            auto tmp(*this);
            tmp -= offset;
            return tmp;
        }

        difference_type operator-(const ConcatenateIterator& other) const {
            return minus(std::index_sequence_for<Iterators...>{}, other);
        }

        bool operator!=(const ConcatenateIterator& other) const {
            return ApplyCurrentIterator<const IterTuple&, 0>()(_iterators, other._end,
                                                               [](const auto& it, const auto& end) {
                                                                   return it != end;
                                                               }, false);
        }

        bool operator==(const ConcatenateIterator& other) const {
            return !(*this != other);
        }

        reference operator[](const difference_type offset) const {
            return *(*this + offset);
        }

        bool operator<(const ConcatenateIterator& other) const {
            return other - *this > 0;
        }

        bool operator>(const ConcatenateIterator& other) const {
            return other < *this;
        }

        bool operator<=(const ConcatenateIterator& other) const {
            return !(other < *this);
        }

        bool operator>=(const ConcatenateIterator& other) const {
            return !(*this < other);
        }
    };
}}