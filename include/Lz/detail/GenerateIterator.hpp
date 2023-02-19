#pragma once

#ifndef LZ_GENERATE_ITERATOR_HPP
#    define LZ_GENERATE_ITERATOR_HPP

#    include "FunctionContainer.hpp"

namespace lz {
namespace internal {
template<class GeneratorFunc, class... Args>
class GenerateIterator {
    std::size_t _current{};
    mutable FunctionContainer<GeneratorFunc> _generator{};
    std::tuple<Args...> _args{};
    bool _isWhileTrueLoop{ false };

public:
    using iterator_category = std::forward_iterator_tag;
    using reference = decltype(tupleInvoker(_generator, _args, MakeIndexSequence<sizeof...(Args)>()));
    using value_type = Decay<reference>;
    using difference_type = std::ptrdiff_t;
    using pointer = FakePointerProxy<reference>;

    constexpr GenerateIterator() = default;

    constexpr GenerateIterator(const std::size_t start, GeneratorFunc generatorFunc, const bool isWhileTrueLoop,
                               std::tuple<Args...> args) :
        _current(start),
        _generator(std::move(generatorFunc)),
        _args(std::move(args)),
        _isWhileTrueLoop(isWhileTrueLoop) {
    }

    LZ_NODISCARD constexpr reference operator*() {
        return tupleInvoker(_generator, _args, MakeIndexSequence<sizeof...(Args)>());
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
} // namespace internal
} // namespace lz

#endif