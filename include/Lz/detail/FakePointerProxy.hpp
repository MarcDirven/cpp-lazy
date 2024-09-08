#pragma once

#ifndef LZ_FAKE_POINTER_PROXY_HPP
#define LZ_FAKE_POINTER_PROXY_HPP

#include "Lz/detail/CompilerChecks.hpp"

#include <memory>

namespace lz {
namespace detail {

template<class T>
class FakePointerProxy {
    T _t;

    using Pointer = decltype(std::addressof(_t));

public:
    constexpr explicit FakePointerProxy(const T& t) : _t(t) {
    }

    LZ_CONSTEXPR_CXX_17 Pointer operator->() const noexcept {
        return std::addressof(_t);
    }

    LZ_CONSTEXPR_CXX_17 Pointer operator->() noexcept {
        return std::addressof(_t);
    }
};
} // namespace detail
} // namespace lz
#endif // LZ_FAKE_POINTER_PROXY_HPP