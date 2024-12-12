#pragma once

#ifndef LZ_GENERATE_ITERATOR_HPP
#define LZ_GENERATE_ITERATOR_HPP

#include "Lz/IterBase.hpp"
#include "Lz/detail/FakePointerProxy.hpp"
#include "Lz/detail/FunctionContainer.hpp"
#include "Lz/detail/Procs.hpp"

namespace lz {
namespace detail {
template<class GeneratorFunc, class... Args>
class GenerateIterator : public IterBase<GenerateIterator<GeneratorFunc, Args...>, TupleInvokerType<GeneratorFunc, Args...>,
                                         FakePointerProxy<TupleInvokerType<GeneratorFunc, Args...>>, std::ptrdiff_t,
                                         std::forward_iterator_tag, DefaultSentinel> {

    mutable std::tuple<Args...> _args{};
    mutable TupleInvoker<GeneratorFunc, Args...> _tupleInvoker{};
    std::size_t _current{};
    std::size_t _amount{};
    bool _isWhileTrueLoop{ false };

public:
    using iterator_category = std::forward_iterator_tag;
    using reference = TupleInvokerType<GeneratorFunc, Args...>;
    using value_type = Decay<reference>;
    using difference_type = std::ptrdiff_t;
    using pointer = FakePointerProxy<reference>;

    constexpr GenerateIterator() = default;

    LZ_CONSTEXPR_CXX_14
    GenerateIterator(const std::size_t amount, GeneratorFunc generatorFunc, const bool isWhileTrueLoop,
                     std::tuple<Args...> args) :
        _args(std::move(args)),
        _tupleInvoker(makeExpandFn(std::move(generatorFunc), MakeIndexSequence<sizeof...(Args)>())),
        _amount(amount),
        _isWhileTrueLoop(isWhileTrueLoop) {
    }

    LZ_NODISCARD constexpr reference dereference() const {
        return _tupleInvoker(_args);
    }

    LZ_NODISCARD constexpr pointer arrow() const {
        return FakePointerProxy<decltype(**this)>(**this);
    }

    LZ_CONSTEXPR_CXX_14 void increment() {
        if (!_isWhileTrueLoop) {
            ++_current;
        }
    }

    LZ_NODISCARD constexpr bool eq(const GenerateIterator& b) const noexcept {
        LZ_ASSERT(_isWhileTrueLoop == b._isWhileTrueLoop, "incompatible iterator types: both must be while true or not");
        return _current == b._current;
    }

    LZ_NODISCARD constexpr bool eq(DefaultSentinel) const noexcept {
        return _current == _amount;
    }
};
} // namespace detail
} // namespace lz

#endif