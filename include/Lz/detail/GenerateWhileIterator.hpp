#pragma once

#ifndef LZ_GENERATE_WHILE_ITERATOR_HPP
#define LZ_GENERATE_WHILE_ITERATOR_HPP

#include "FunctionContainer.hpp"

namespace lz {
namespace internal {
template<class GeneratorFunc, class... Args>
class GenerateWhileIterator {
    using TupleInvoker = decltype(makeExpandFn(std::declval<GeneratorFunc>(), MakeIndexSequence<sizeof...(Args)>()));
    mutable TupleInvoker _tupleInvoker{};
    mutable std::tuple<Args...> _args{};

    using FunctionReturnType = decltype(_tupleInvoker(_args));
    FunctionReturnType _lastReturned;

public:
    using iterator_category = std::forward_iterator_tag;
    using reference = typename FunctionReturnType::second_type;
    using value_type = Decay<reference>;
    using difference_type = std::ptrdiff_t;
    using pointer = FakePointerProxy<reference>;

    constexpr GenerateWhileIterator() = default;

    template<class P>
    LZ_CONSTEXPR_CXX_14 GenerateWhileIterator(GeneratorFunc generatorFunc, std::tuple<Args...> args, const bool isEndIterator, P p) :
        _tupleInvoker(makeExpandFn(std::move(generatorFunc), MakeIndexSequence<sizeof...(Args)>())),
        _args(std::move(args)),
        _lastReturned(std::move(p)) {
        if (isEndIterator) {
            std::get<0>(_lastReturned) = static_cast<typename FunctionReturnType::first_type>(false);
        }
    }

    LZ_NODISCARD constexpr reference operator*() {
        return _lastReturned.second;
    }

    LZ_NODISCARD constexpr pointer operator->() {
        return FakePointerProxy<decltype(**this)>(**this);
    }

    LZ_CONSTEXPR_CXX_14 GenerateWhileIterator& operator++() {
        _lastReturned = _tupleInvoker(_args);
        return *this;
    }

    LZ_CONSTEXPR_CXX_14 GenerateWhileIterator operator++(int) {
        GenerateWhileIterator tmp(*this);
        ++*this;
        return tmp;
    }

    LZ_NODISCARD constexpr friend bool operator==(const GenerateWhileIterator& a, const GenerateWhileIterator& b) noexcept {
        return not(a != b); // NOLINT
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 friend bool
    operator!=(const GenerateWhileIterator& a, const GenerateWhileIterator&) noexcept {
        return std::get<0>(a._lastReturned);
    }
};
} // namespace internal
} // namespace lz

#endif