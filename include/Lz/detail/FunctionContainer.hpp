#pragma once

#ifndef LZ_FUNCTION_CONTAINER_HPP
#define LZ_FUNCTION_CONTAINER_HPP

#include "LzTools.hpp"

#include <utility>

namespace lz {
namespace internal {
template<class>
struct AlwaysFalse : std::false_type {};

template<class Func>
class FunctionContainer {
    mutable Func _func;
    bool _isConstructed{ false };

    constexpr explicit FunctionContainer(std::false_type /*isDefaultConstructible*/) {
        static_assert(AlwaysFalse<Func>::value, "Please use std::function instead of a lambda in this case, because "
                                                "lambda's are not default constructible pre C++20");
    }

    constexpr explicit FunctionContainer(std::true_type /*isDefaultConstructible*/) : _func(), _isConstructed(true) {
    }

    template<class F>
    LZ_CONSTEXPR_CXX_20 void construct(F&& f) {
        ::new (static_cast<void*>(std::addressof(_func))) Func(static_cast<F&&>(f));
        _isConstructed = true;
    }

    constexpr void move(Func&& f, std::true_type /* isMoveAssignable */) {
        _func = std::move(f);
    }

    LZ_CONSTEXPR_CXX_20 void move(Func&& f, std::false_type /* isMoveAssignable */) {
        reset();
        construct(std::move(f));
    }

    constexpr void reset() noexcept {
        if (_isConstructed) {
            _func.~Func();
            _isConstructed = false;
        }
    }

    constexpr void copy(const Func& f, std::true_type /*isCopyAssignable*/) {
        _func = f;
    }

    LZ_CONSTEXPR_CXX_20 void copy(const Func& f, std::false_type /*isCopyAssignable*/) {
        reset();
        construct(f);
    }

public:
    constexpr explicit FunctionContainer(const Func& func) : _func(func), _isConstructed(true) {
    }

    constexpr explicit FunctionContainer(Func&& func) noexcept : _func(std::move(func)), _isConstructed(true) {
    }

    constexpr FunctionContainer() : FunctionContainer(std::is_default_constructible<Func>()) {
    }

    LZ_CONSTEXPR_CXX_14 FunctionContainer(FunctionContainer&& other) noexcept :
        _func(std::move(other._func)),
        _isConstructed(true) {
        other._isConstructed = false;
    }

    constexpr FunctionContainer(const FunctionContainer& other) : _func(other._func), _isConstructed(true) {
    }

    LZ_CONSTEXPR_CXX_20 FunctionContainer& operator=(const FunctionContainer& other) {
        if (_isConstructed && other._isConstructed) {
            copy(other._func, std::is_copy_assignable<Func>());
        }
        else if (other._isConstructed) {
            construct(other._func);
        }
        else if (_isConstructed) {
            reset();
        }
        return *this;
    }

    LZ_CONSTEXPR_CXX_20 FunctionContainer& operator=(FunctionContainer&& other) noexcept {
        if (_isConstructed && other._isConstructed) {
            move(std::move(other._func), std::is_move_assignable<Func>());
        }
        else if (other._isConstructed) {
            construct(std::move(other._func));
        }
        else if (_isConstructed) {
            reset();
        }
        return *this;
    }

    template<class... Args>
    constexpr auto operator()(Args&&... args) const -> decltype(_func(std::forward<Args>(args)...)) {
        return _func(std::forward<Args>(args)...);
    }
};
} // namespace internal
} // namespace lz
#endif // LZ_FUNCTION_CONTAINER_HPP
