#pragma once

#ifndef LZ_FUNCTION_CONTAINER_HPP
#define LZ_FUNCTION_CONTAINER_HPP

#include "Lz/detail/traits.hpp"

#include <type_traits>

namespace lz {
namespace detail {
template<class Func>
class func_container {
    mutable Func _func;
    bool _is_constructed{ false };

    constexpr explicit func_container(std::false_type /* is_default_constructible */) {
        static_assert(always_false<Func>::value, "Please use std::function instead of a lambda in this case, because "
                                                 "lambda's are not default constructible pre C++20");
    }

    constexpr explicit func_container(std::true_type /* is_default_constructible */) : _func(), _is_constructed(true) {
    }

    template<class F>
    LZ_CONSTEXPR_CXX_20 void construct(F&& f) {
        ::new (static_cast<void*>(std::addressof(_func))) Func(static_cast<F&&>(f));
        _is_constructed = true;
    }

    LZ_CONSTEXPR_CXX_14 void reset() noexcept {
        if (_is_constructed) {
            _func.~Func();
            _is_constructed = false;
        }
    }

#ifdef __cpp_if_constexpr
    template<class F = Func>
    LZ_CONSTEXPR_CXX_20 void copy(const Func& f) {
        if constexpr (std::is_copy_assignable_v<F>) {
            _func = f;
        }
        else {
            reset();
            construct(f);
        }
    }

    template<class F = Func>
    LZ_CONSTEXPR_CXX_20 void move(Func&& f) {
        if constexpr (std::is_move_assignable_v<F>) {
            _func = std::move(f);
        }
        else {
            reset();
            construct(std::move(f));
        }
    }
#else
    template<class F = Func>
    LZ_CONSTEXPR_CXX_20 enable_if<std::is_copy_assignable<F>::value> copy(const Func& f) {
        _func = f;
    }

    template<class F = Func>
    LZ_CONSTEXPR_CXX_20 enable_if<!std::is_copy_assignable<F>::value> copy(const Func& f) {
        reset();
        construct(f);
    }

    template<class F = Func>
    LZ_CONSTEXPR_CXX_14 enable_if<std::is_move_assignable<F>::value> move(Func&& f) {
        _func = std::move(f);
    }

    template<class F = Func>
    LZ_CONSTEXPR_CXX_20 enable_if<!std::is_move_assignable<F>::value> move(Func&& f) {
        reset();
        construct(std::move(f));
    }
#endif

public:
    constexpr explicit func_container(const Func& func) : _func(func), _is_constructed(true) {
    }

    constexpr explicit func_container(Func&& func) noexcept : _func(std::move(func)), _is_constructed(true) {
    }

    constexpr func_container() : func_container(std::is_default_constructible<Func>()) {
    }

    LZ_CONSTEXPR_CXX_14 func_container(func_container&& other) noexcept : _func(std::move(other._func)), _is_constructed(true) {
        other._is_constructed = false;
    }

    constexpr func_container(const func_container& other) : _func(other._func), _is_constructed(true) {
    }

    LZ_CONSTEXPR_CXX_20 func_container& operator=(const func_container& other) {
        if (_is_constructed && other._is_constructed) {
            copy(other._func);
        }
        else if (other._is_constructed) {
            construct(other._func);
        }
        else if (_is_constructed) {
            reset();
        }
        return *this;
    }

    LZ_CONSTEXPR_CXX_20 func_container& operator=(func_container&& other) noexcept {
        if (_is_constructed && other._is_constructed) {
            move(std::move(other._func));
        }
        else if (other._is_constructed) {
            construct(std::move(other._func));
        }
        else if (_is_constructed) {
            reset();
        }
        return *this;
    }

    template<class... Args>
    LZ_CONSTEXPR_CXX_14 auto operator()(Args&&... args) const -> decltype(_func(std::forward<Args>(args)...)) {
        return _func(std::forward<Args>(args)...);
    }

    template<class... Args>
    LZ_CONSTEXPR_CXX_14 auto operator()(Args&&... args) -> decltype(_func(std::forward<Args>(args)...)) {
        return _func(std::forward<Args>(args)...);
    }
};

template<class Func, class... Iterators>
using func_container_ret_type = func_ret_type<func_container<Func>, decltype(*std::declval<Iterators>())...>;

} // namespace detail
} // namespace lz
#endif // LZ_FUNCTION_CONTAINER_HPP
