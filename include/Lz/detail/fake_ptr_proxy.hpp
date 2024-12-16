#pragma once

#ifndef LZ_FAKE_POINTER_PROXY_HPP
#define LZ_FAKE_POINTER_PROXY_HPP

#include "Lz/detail/compiler_checks.hpp"

#include <memory>

namespace lz {
namespace detail {

template<class T>
class fake_ptr_proxy {
    T _t;

    using ptr = decltype(std::addressof(_t));

public:
    constexpr explicit fake_ptr_proxy(const T& t) : _t(t) {
    }

    LZ_CONSTEXPR_CXX_17 ptr operator->() const noexcept {
        return std::addressof(_t);
    }

    LZ_CONSTEXPR_CXX_17 ptr operator->() noexcept {
        return std::addressof(_t);
    }
};
} // namespace detail
} // namespace lz
#endif // LZ_FAKE_POINTER_PROXY_HPP