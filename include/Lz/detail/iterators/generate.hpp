#pragma once

#ifndef LZ_GENERATE_ITERATOR_HPP
#define LZ_GENERATE_ITERATOR_HPP

#include "Lz/detail/fake_ptr_proxy.hpp"
#include "Lz/detail/func_container.hpp"
#include "Lz/detail/procs.hpp"
#include "Lz/iterator_base.hpp"

namespace lz {
namespace detail {
// TODO: Remove current and amount for one variable and check if 0
template<class GeneratorFunc, class... Args>
class generate_iterator : public iter_base<generate_iterator<GeneratorFunc, Args...>, tuple_invoker_ret<GeneratorFunc, Args...>,
                                           fake_ptr_proxy<tuple_invoker_ret<GeneratorFunc, Args...>>, std::ptrdiff_t,
                                           std::forward_iterator_tag, default_sentinel> {

    mutable std::tuple<Args...> _args{};
    std::size_t _current{};
    std::size_t _amount{};
    mutable tuple_invoker<GeneratorFunc, Args...> _tupleInvoker{};
    bool _is_inf_loop{ false };

public:
    using iterator_category = std::forward_iterator_tag;
    using reference = tuple_invoker_ret<GeneratorFunc, Args...>;
    using value_type = decay<reference>;
    using difference_type = std::ptrdiff_t;
    using pointer = fake_ptr_proxy<reference>;

    constexpr generate_iterator() = default;

    LZ_CONSTEXPR_CXX_14
    generate_iterator(const std::size_t amount, GeneratorFunc generator_func, const bool is_inf_loop, std::tuple<Args...> args) :
        _args(std::move(args)),
        _amount(amount),
        _tupleInvoker(make_expand_fn(std::move(generator_func), make_index_sequence<sizeof...(Args)>())),
        _is_inf_loop(is_inf_loop) {
    }

    LZ_NODISCARD constexpr reference dereference() const {
        return _tupleInvoker(_args);
    }

    LZ_NODISCARD constexpr pointer arrow() const {
        return fake_ptr_proxy<decltype(**this)>(**this);
    }

    LZ_CONSTEXPR_CXX_14 void increment() {
        if (!_is_inf_loop) {
            ++_current;
        }
    }

    LZ_NODISCARD constexpr bool eq(const generate_iterator& b) const noexcept {
        LZ_ASSERT(_is_inf_loop == b._is_inf_loop, "incompatible iterator types: both must be while true or not");
        return _current == b._current;
    }

    LZ_NODISCARD constexpr bool eq(default_sentinel) const noexcept {
        return _current == _amount;
    }
};
} // namespace detail
} // namespace lz

#endif