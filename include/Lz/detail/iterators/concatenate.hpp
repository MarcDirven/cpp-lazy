#pragma once

#ifndef LZ_CONCATENATE_ITERATOR_HPP
#define LZ_CONCATENATE_ITERATOR_HPP

#include "Lz/detail/fake_ptr_proxy.hpp"
#include "Lz/detail/traits.hpp"
#include "Lz/iter_base.hpp"

#include <numeric>

namespace lz {
namespace detail {
#ifndef __cpp_if_constexpr
template<class Tuple, class sentinel_tuple, std::size_t I, class = void>
struct plus_plus {
    LZ_CONSTEXPR_CXX_20 void operator()(Tuple& iterators, const sentinel_tuple& end) const {
        if (std::get<I>(iterators) != std::get<I>(end)) {
            ++std::get<I>(iterators);
        }
        else {
            plus_plus<Tuple, sentinel_tuple, I + 1>()(iterators, end);
        }
    }
};

template<class Tuple, class sentinel_tuple, std::size_t I>
struct plus_plus<Tuple, sentinel_tuple, I, enable_if<I == std::tuple_size<decay<Tuple>>::value>> {
    LZ_CONSTEXPR_CXX_20 void operator()(const Tuple& /*iterators*/, const sentinel_tuple& /*end*/) const {
    }
};

template<class Tuple, class sentinel_tuple, std::size_t I, class = void>
struct not_equal {
    LZ_CONSTEXPR_CXX_20 bool operator()(const Tuple& iterators, const sentinel_tuple& end) const noexcept {
        const bool has_value = std::get<I>(iterators) != std::get<I>(end);
        return has_value ? has_value : not_equal<Tuple, sentinel_tuple, I + 1>()(iterators, end);
    }
};

template<class Tuple, class sentinel_tuple, std::size_t I>
struct not_equal<Tuple, sentinel_tuple, I, enable_if<I == std::tuple_size<decay<Tuple>>::value - 1>> {
    LZ_CONSTEXPR_CXX_20 bool operator()(const Tuple& iterators, const sentinel_tuple& end) const noexcept {
        return std::get<I>(iterators) != std::get<I>(end);
    }
};

template<class Tuple, class sentinel_tuple, std::size_t I, class = void>
struct deref {
    LZ_CONSTEXPR_CXX_20 auto operator()(const Tuple& iterators, const sentinel_tuple& end) const
        -> decltype(*std::get<I>(iterators)) {
        return std::get<I>(iterators) != std::get<I>(end) ? *std::get<I>(iterators)
                                                          : deref<Tuple, sentinel_tuple, I + 1>()(iterators, end);
    }
};

template<class Tuple, class sentinel_tuple, std::size_t I>
struct deref<Tuple, sentinel_tuple, I, enable_if<I == std::tuple_size<decay<Tuple>>::value - 1>> {
    LZ_CONSTEXPR_CXX_20 auto operator()(const Tuple& iterators, const sentinel_tuple&) const
        -> decltype(*std::get<I>(iterators)) {
        return *std::get<I>(iterators);
    }
};

template<class Tuple, std::size_t I>
struct minus_minus {
    LZ_CONSTEXPR_CXX_20 void operator()(Tuple& iterators, const Tuple& begin, const Tuple& end) const {
        auto& current = std::get<I>(iterators);
        if (current != std::get<I>(begin)) {
            --current;
        }
        else {
            minus_minus<Tuple, I - 1>()(iterators, begin, end);
        }
    }
};

template<class Tuple>
struct minus_minus<Tuple, 0> {
    LZ_CONSTEXPR_CXX_20 void operator()(Tuple& iterators, const Tuple&, const Tuple&) const {
        --std::get<0>(iterators);
    }
};

template<class Tuple, std::size_t I>
struct min_is {
    template<class DifferenceType>
    LZ_CONSTEXPR_CXX_20 void
    operator()(Tuple& iterators, const Tuple& begin, const Tuple& end, const DifferenceType offset) const {
        using TupElem = tuple_element<I, Tuple>;
        const TupElem current = std::get<I>(iterators);
        const TupElem current_begin = std::get<I>(begin);
        // Current is begin, move on to next iterator
        if (current == current_begin) {
            min_is<Tuple, I - 1>()(iterators, begin, end, offset);
        }
        else {
            const auto dist = std::get<I>(end) - current;
            if (dist <= offset) {
                std::get<I>(iterators) = std::get<I>(begin);
                min_is<Tuple, I - 1>()(iterators, begin, end, dist == 0 ? DifferenceType{ 1 } : offset - dist);
            }
            else {
                std::get<I>(iterators) -= offset;
            }
        }
    }
};

template<class Tuple>
struct min_is<Tuple, 0> {
    template<class DifferenceType>
    LZ_CONSTEXPR_CXX_20 void
    operator()(Tuple& iterators, const Tuple& /* begin */, const Tuple& /*end*/, const DifferenceType offset) const {
        using TupElem = tuple_element<0, Tuple>;
        TupElem& current = std::get<0>(iterators);
        current -= offset;
    }
};

template<class Tuple, std::size_t I, class = void>
struct plus_is {
    template<class DifferenceType>
    LZ_CONSTEXPR_CXX_20 void operator()(Tuple& iterators, const Tuple& end, const DifferenceType offset) const {
        using TupElem = tuple_element<I, Tuple>;
        TupElem& currentIterator = std::get<I>(iterators);
        const TupElem current_end = std::get<I>(end);
        const auto dist = current_end - currentIterator;
        if (dist > offset) {
            currentIterator += offset;
        }
        else {
            // Moves to end
            currentIterator += dist;
            plus_is<Tuple, I + 1>()(iterators, end, offset - dist);
        }
    }
};

template<class Tuple, std::size_t I>
struct plus_is<Tuple, I, enable_if<I == std::tuple_size<decay<Tuple>>::value - 1>> {
    template<class DifferenceType>
    LZ_CONSTEXPR_CXX_14 void operator()(Tuple& /*iterators*/, const Tuple& /*end*/, const DifferenceType /*offset*/) const {
    }
};
#else
template<class Tuple, class SentinelTuple, std::size_t I>
struct plus_plus {
    LZ_CONSTEXPR_CXX_20 void operator()(Tuple& iterators, const SentinelTuple& end) const {
        if constexpr (I == std::tuple_size_v<decay<Tuple>>) {
            static_cast<void>(iterators);
            static_cast<void>(end);
            return;
        }
        else {
            if (std::get<I>(iterators) != std::get<I>(end)) {
                ++std::get<I>(iterators);
            }
            else {
                plus_plus<Tuple, SentinelTuple, I + 1>()(iterators, end);
            }
        }
    }
};

template<class Tuple, class SentinelTuple, std::size_t I>
struct not_equal {
    LZ_CONSTEXPR_CXX_20 bool operator()(const Tuple& iterators, const SentinelTuple& end) const {
        if constexpr (I == std::tuple_size_v<decay<Tuple>> - 1) {
            return std::get<I>(iterators) != std::get<I>(end);
        }
        else {
            const bool iterHasValue = std::get<I>(iterators) != std::get<I>(end);
            return iterHasValue ? iterHasValue : not_equal<Tuple, SentinelTuple, I + 1>()(iterators, end);
        }
    }
};

template<class Tuple, class SentinelTuple, std::size_t I>
struct deref {
    LZ_CONSTEXPR_CXX_20 auto operator()(const Tuple& iterators, const SentinelTuple& end) const
        -> decltype(*std::get<I>(iterators)) {
        if constexpr (I == std::tuple_size_v<decay<Tuple>> - 1) {
            static_cast<void>(end);
            return *std::get<I>(iterators);
        }
        else {
            return std::get<I>(iterators) != std::get<I>(end) ? *std::get<I>(iterators)
                                                              : deref<Tuple, SentinelTuple, I + 1>()(iterators, end);
        }
    }
};

template<class Tuple, std::size_t I>
struct minus_minus {
    LZ_CONSTEXPR_CXX_20 void operator()(Tuple& iterators, const Tuple& begin, const Tuple& end) const {
        if constexpr (I == 0) {
            static_cast<void>(begin);
            static_cast<void>(end);
            --std::get<0>(iterators);
        }
        else {
            auto& current = std::get<I>(iterators);
            if (current != std::get<I>(begin)) {
                --current;
            }
            else {
                minus_minus<Tuple, I - 1>()(iterators, begin, end);
            }
        }
    }
};

template<class Tuple, std::size_t I>
struct min_is {
    template<class DifferenceType>
    LZ_CONSTEXPR_CXX_20 void
    operator()(Tuple& iterators, const Tuple& begin, const Tuple& end, const DifferenceType offset) const {
        using TupElem = tuple_element<I, Tuple>;

        if constexpr (I == 0) {
            static_cast<void>(begin);
            static_cast<void>(end);
            TupElem& current = std::get<0>(iterators);
            current -= offset;
        }
        else {
            const TupElem current_begin = std::get<I>(begin);
            const TupElem current = std::get<I>(iterators);
            // Current is begin, move on to next iterator
            if (current == current_begin) {
                min_is<Tuple, I - 1>()(iterators, begin, end, offset);
            }
            else {
                const auto dist = std::get<I>(end) - current;
                if (dist <= offset) {
                    std::get<I>(iterators) = std::get<I>(begin);
                    min_is<Tuple, I - 1>()(iterators, begin, end, dist == 0 ? DifferenceType{ 1 } : offset - dist);
                }
                else {
                    std::get<I>(iterators) -= offset;
                }
            }
        }
    }
};

template<class Tuple, std::size_t I>
struct plus_is {
    template<class DifferenceType>
    LZ_CONSTEXPR_CXX_20 void operator()(Tuple& iterators, const Tuple& end, const DifferenceType offset) const {
        if constexpr (I == std::tuple_size_v<decay<Tuple>> - 1) {
            static_cast<void>(iterators);
            static_cast<void>(end);
            static_cast<void>(offset);
            return;
        }
        else {
            using TupElem = tuple_element<I, Tuple>;
            TupElem& current_it = std::get<I>(iterators);
            const TupElem current_end = std::get<I>(end);
            const auto dist = current_end - currentIterator;
            if (dist > offset) {
                current_it += offset;
            }
            else {
                // Moves to end
                current_it += dist;
                plus_is<Tuple, I + 1>()(iterators, end, offset - dist);
            }
        }
    }
};
#endif // __cpp_if_constexpr

template<class Tuple>
using first_it = tuple_element<0, Tuple>;

template<typename Tuple>
struct iter_cat_tuple_helper;

template<typename... Iterators>
struct iter_cat_tuple_helper<std::tuple<Iterators...>> {
    using type = common_type<iter_cat<Iterators>...>;
};

template<typename Tuple>
struct diff_type_tuple_helper;

template<typename... Iterators>
struct diff_type_tuple_helper<std::tuple<Iterators...>> {
    using type = common_type<diff_type<Iterators>...>;
};

template<class Tuple>
using iter_cat_tuple = typename iter_cat_tuple_helper<Tuple>::type;

template<class Tuple>
using diff_type_tuple = typename diff_type_tuple_helper<Tuple>::type;

template<class IterTuple, class SentinelTuple>
class concatenate_iterator
    : public iter_base<concatenate_iterator<IterTuple, SentinelTuple>, ref<first_it<IterTuple>>,
                       fake_ptr_proxy<ref<first_it<IterTuple>>>, diff_type_tuple<IterTuple>, iter_cat_tuple<IterTuple>,
                       sentinel_selector<iter_cat_tuple<IterTuple>, concatenate_iterator<IterTuple, SentinelTuple>>> {

    IterTuple _iterators{};
    IterTuple _begin{};
    SentinelTuple _end{};

    using first_tuple_iterator = std::iterator_traits<first_it<IterTuple>>;

public:
    using value_type = typename first_tuple_iterator::value_type;
    using difference_type = diff_type_tuple<IterTuple>;
    using reference = typename first_tuple_iterator::reference;
    using pointer = fake_ptr_proxy<reference>;

private:
    template<std::size_t... I>
    LZ_CONSTEXPR_CXX_20 difference_type minus(index_sequence<I...>, const concatenate_iterator& other) const {
        const difference_type totals[] = { static_cast<difference_type>(std::get<I>(_iterators) -
                                                                        std::get<I>(other._iterators))... };
        return std::accumulate(std::begin(totals), std::end(totals), difference_type{ 0 });
    }

public:
    LZ_CONSTEXPR_CXX_20 concatenate_iterator(IterTuple iterators, IterTuple begin, SentinelTuple end) :
        _iterators(std::move(iterators)),
        _begin(std::move(begin)),
        _end(std::move(end)) {
    }

    constexpr concatenate_iterator() = default;

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 reference dereference() const {
        return deref<IterTuple, SentinelTuple, 0>()(_iterators, _end);
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 pointer arrow() const {
        return fake_ptr_proxy<decltype(**this)>(**this);
    }

    LZ_CONSTEXPR_CXX_20 void increment() {
        plus_plus<IterTuple, SentinelTuple, 0>()(_iterators, _end);
    }

    LZ_CONSTEXPR_CXX_20 void decrement() {
        minus_minus<IterTuple, std::tuple_size<IterTuple>::value - 1>()(_iterators, _begin, _end);
    }

    LZ_CONSTEXPR_CXX_20 void plus_is(const difference_type offset) {
        if (offset < 0) {
            min_is<IterTuple, std::tuple_size<IterTuple>::value - 1>()(_iterators, _begin, _end, -offset);
        }
        else {
            plus_is<IterTuple, 0>()(_iterators, _end, offset);
        }
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 difference_type difference(const concatenate_iterator& other) const {
        return minus(make_index_sequence<std::tuple_size<IterTuple>::value>(), other);
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 bool eq(const concatenate_iterator& b) const {
        return !not_equal<IterTuple, SentinelTuple, 0>()(_iterators, b._iterators);
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 bool eq(default_sentinel) const {
        return !not_equal<IterTuple, SentinelTuple, 0>()(_iterators, _end);
    }
};

} // namespace detail
} // namespace lz

#endif // LZ_CONCATENATE_ITERATOR_HPP