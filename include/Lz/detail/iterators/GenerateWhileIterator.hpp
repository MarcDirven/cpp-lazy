#pragma once

#ifndef LZ_GENERATE_WHILE_ITERATOR_HPP
#define LZ_GENERATE_WHILE_ITERATOR_HPP

#include "Lz/IterBase.hpp"
#include "Lz/detail/FakePointerProxy.hpp"
#include "Lz/detail/FunctionContainer.hpp"
#include "Lz/detail/Procs.hpp"
#include "Lz/detail/Traits.hpp"

namespace lz {
namespace detail {
template<class GeneratorFunc, class... Args>
using GenerateWhileRef = TupleElement<1, TupleInvokerType<GeneratorFunc, Args...>>;

template<class GeneratorFunc, class... Args>
class GenerateWhileIterator
    : public IterBase<GenerateWhileIterator<GeneratorFunc, Args...>, GenerateWhileRef<GeneratorFunc, Args...>,
                      FakePointerProxy<GenerateWhileRef<GeneratorFunc, Args...>>, std::ptrdiff_t, std::forward_iterator_tag> {

    TupleInvoker<GeneratorFunc, Args...> _tupleInvoker{};
    std::tuple<Args...> _args{};

    using FunctionReturnType = TupleInvokerType<GeneratorFunc, Args...>;
    FunctionReturnType _lastReturned;

public:
    using iterator_category = std::forward_iterator_tag;
    using reference = TupleElement<1, FunctionReturnType>;
    using value_type = Decay<reference>;
    using difference_type = std::ptrdiff_t;
    using pointer = FakePointerProxy<reference>;

    constexpr GenerateWhileIterator() = default;

    template<class P>
    LZ_CONSTEXPR_CXX_14
    GenerateWhileIterator(GeneratorFunc generatorFunc, std::tuple<Args...> args, const bool isEndIterator, P&& p) :
        _tupleInvoker(makeExpandFn(std::move(generatorFunc), MakeIndexSequence<sizeof...(Args)>())),
        _args(std::move(args)),
        _lastReturned(std::forward<P>(p)) {
        if (isEndIterator) {
            using FirstType = Decay<decltype(std::get<0>(_lastReturned))>;
            std::get<0>(_lastReturned) = static_cast<FirstType>(false);
        }
    }

    LZ_NODISCARD constexpr reference dereference() const {
        return std::get<1>(_lastReturned);
    }

    LZ_NODISCARD constexpr pointer arrow() const {
        return FakePointerProxy<decltype(**this)>(**this);
    }

    LZ_CONSTEXPR_CXX_14 void increment() {
        _lastReturned = _tupleInvoker(_args);
    }

    LZ_NODISCARD constexpr bool eq(const GenerateWhileIterator&) const noexcept {
        return !std::get<0>(_lastReturned);
    }
};
} // namespace detail
} // namespace lz

#endif