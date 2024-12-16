#pragma once

#ifndef LZ_OPTIONAL_HPP
#define LZ_OPTIONAL_HPP

#include "Lz/detail/compiler_checks.hpp"

#include <type_traits>

#ifdef __cpp_lib_optional
#include <optional>
#endif // __cpp_lib_optional

namespace lz {
namespace detail {
#ifdef __cpp_lib_optional
template<class T>
using optional = std::optional<T>;
#else
template<class T>
class optional {
    static_assert(!std::is_array<T>::value && std::is_object<T>::value, "T may not be an array and must be an object");

    union {
        typename std::remove_const<T>::type _value;
        std::uint8_t _dummy{};
    };
    bool _has_value{ false };

    template<class U>
    void construct(U&& obj) noexcept(noexcept(::new(static_cast<void*>(std::addressof(_value))) T(std::forward<U>(obj)))) {
        ::new (static_cast<void*>(std::addressof(_value))) T(std::forward<U>(obj));
        _has_value = true;
    }

public:
    constexpr optional() noexcept {
    }

    template<class U = T>
    constexpr optional(U&& value) : _value(std::forward<U>(value)), _has_value(true) {
    }

    LZ_CONSTEXPR_CXX_14 optional(optional<T>&& right) noexcept {
        if (right) {
            construct(std::move(right.value()));
        }
    }

    ~optional() {
        if LZ_CONSTEXPR_IF (std::is_trivially_destructible<T>::value) {
            return;
        }
        if (_has_value) {
            _value.~T();
        }
    }

    template<class U = T>
    LZ_CONSTEXPR_CXX_14 optional&
    operator=(U&& value) noexcept {
        if (_has_value) {
            _value = std::forward<U>(value);
        }
        else {
            construct(std::forward<U>(value));
        }
        return *this;
    }

    constexpr explicit operator bool() const noexcept {
        return _has_value;
    }

    LZ_CONSTEXPR_CXX_14 const T& value() const {
        if (_has_value) {
            return _value;
        }
        throw std::runtime_error("Cannot get uninitialized optional");
    }

    LZ_CONSTEXPR_CXX_14 T& value() {
        if (_has_value) {
            return _value;
        }
        throw std::runtime_error("Cannot get uninitialized optional");
    }

    LZ_CONSTEXPR_CXX_14 T& operator*() noexcept {
        return _value;
    }

    LZ_CONSTEXPR_CXX_14 const T& operator*() const noexcept {
        return _value;
    }

    template<class U>
    LZ_CONSTEXPR_CXX_14 T value_or(U&& v) const& {
        return bool(*this) ? this->value() : static_cast<T>(std::forward<U>(v));
    }

    template<class U>
    LZ_CONSTEXPR_CXX_14 T value_or(U&& v) && {
        return bool(*this) ? std::move(this->value()) : static_cast<T>(std::forward<U>(v));
    }
};
#endif // __cpp_lib_optional
} // namespace detail
} // namespace lz

#endif