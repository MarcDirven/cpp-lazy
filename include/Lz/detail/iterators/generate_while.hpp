#pragma once

#ifndef LZ_GENERATE_WHILE_ITERATOR_HPP
#define LZ_GENERATE_WHILE_ITERATOR_HPP

#include "Lz/detail/fake_ptr_proxy.hpp"
#include "Lz/detail/func_container.hpp"
#include "Lz/detail/procs.hpp"
#include "Lz/detail/traits.hpp"
#include "Lz/iterator_base.hpp"

namespace lz {
namespace detail {
template<class GeneratorFunc, class... Args>
using generate_while_ref = tup_element<1, tuple_invoker_ret<GeneratorFunc, Args...>>;

template<class GeneratorFunc, class... Args>
class generate_while_iterator
    : public iter_base<generate_while_iterator<GeneratorFunc, Args...>, generate_while_ref<GeneratorFunc, Args...>,
                       fake_ptr_proxy<generate_while_ref<GeneratorFunc, Args...>>, std::ptrdiff_t, std::forward_iterator_tag,
                       default_sentinel> {

    std::tuple<Args...> _args;

    using fn_return_type = tuple_invoker_ret<GeneratorFunc, Args...>;
    tuple_invoker<GeneratorFunc, Args...> _tuple_invoker;
    fn_return_type _last_returned;

public:
    using iterator_category = std::forward_iterator_tag;
    using reference = tup_element<1, fn_return_type>;
    using value_type = decay<reference>;
    using difference_type = std::ptrdiff_t;
    using pointer = fake_ptr_proxy<reference>;

    constexpr generate_while_iterator() = default;

    LZ_CONSTEXPR_CXX_14 generate_while_iterator(GeneratorFunc generator_func, std::tuple<Args...> args) :
        _args(std::move(args)),
        _tuple_invoker(make_expand_fn(std::move(generator_func), make_index_sequence<sizeof...(Args)>())),
        _last_returned(_tuple_invoker(_args)) {
    }

    LZ_NODISCARD constexpr reference dereference() const {
        return std::get<1>(_last_returned);
    }

    LZ_NODISCARD constexpr pointer arrow() const {
        return fake_ptr_proxy<decltype(**this)>(**this);
    }

    LZ_CONSTEXPR_CXX_14 void increment() {
        _last_returned = _tuple_invoker(_args);
    }

    LZ_NODISCARD constexpr bool eq(const generate_while_iterator&) const noexcept {
        return !std::get<0>(_last_returned);
    }

    LZ_NODISCARD constexpr bool eq(default_sentinel) const noexcept {
        return !std::get<0>(_last_returned);
    }
};
} // namespace detail
} // namespace lz

#endif