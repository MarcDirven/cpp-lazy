#pragma once

#ifndef LZ_GENERATE_ITERATOR_HPP
#define LZ_GENERATE_ITERATOR_HPP

#include "Lz/detail/FakePointerProxy.hpp"
#include "Lz/detail/FunctionContainer.hpp"
#include "Lz/detail/Procs.hpp"

namespace lz {
namespace detail {
template<class GeneratorFunc, class... Args>
class GenerateIterator {
    using TupleInvoker = decltype(makeExpandFn(std::declval<GeneratorFunc>(), MakeIndexSequence<sizeof...(Args)>()));
    std::size_t _current{};
    mutable TupleInvoker _tupleInvoker{};
    mutable std::tuple<Args...> _args{};
    bool _isWhileTrueLoop{ false };

public:
    using iterator_category = std::forward_iterator_tag;
    using reference = decltype(_tupleInvoker(_args));
    using value_type = Decay<reference>;
    using difference_type = std::ptrdiff_t;
    using pointer = FakePointerProxy<reference>;

    constexpr GenerateIterator() = default;

    LZ_CONSTEXPR_CXX_14
    GenerateIterator(const std::size_t start, GeneratorFunc generatorFunc, const bool isWhileTrueLoop, std::tuple<Args...> args) :
        _current(start),
        _tupleInvoker(makeExpandFn(std::move(generatorFunc), MakeIndexSequence<sizeof...(Args)>())),
        _args(std::move(args)),
        _isWhileTrueLoop(isWhileTrueLoop) {
    }

    LZ_NODISCARD constexpr reference operator*() {
        return _tupleInvoker(_args);
    }

    LZ_NODISCARD constexpr pointer operator->() const {
        return FakePointerProxy<decltype(**this)>(**this);
    }

    LZ_CONSTEXPR_CXX_14 GenerateIterator& operator++() {
        if (!_isWhileTrueLoop) {
            ++_current;
        }
        return *this;
    }

    LZ_CONSTEXPR_CXX_14 GenerateIterator operator++(int) {
        GenerateIterator tmp(*this);
        ++*this;
        return tmp;
    }

    LZ_NODISCARD constexpr friend bool operator==(const GenerateIterator& a, const GenerateIterator& b) noexcept {
        return !(a != b); // NOLINT
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 friend bool operator!=(const GenerateIterator& a, const GenerateIterator& b) noexcept {
        LZ_ASSERT(a._isWhileTrueLoop == b._isWhileTrueLoop, "incompatible iterator types: both must be while true or not");
        return a._current != b._current;
    }
};
} // namespace detail
} // namespace lz

#endif