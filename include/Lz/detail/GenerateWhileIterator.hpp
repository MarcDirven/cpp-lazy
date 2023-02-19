#pragma once

#ifndef LZ_GENERATE_WHILE_ITERATOR_HPP
#    define LZ_GENERATE_WHILE_ITERATOR_HPP

#    include "FunctionContainer.hpp"

namespace lz {
namespace internal {
template<class GeneratorFunc, class... Args>
class GenerateWhileIterator {
    mutable FunctionContainer<GeneratorFunc> _generator{};
    std::tuple<Args...> _args{};

    using FunctionReturnType = decltype(tupleInvoker(_generator, _args, MakeIndexSequence<sizeof...(Args)>()));
    FunctionReturnType _lastReturned{};

public:
    using iterator_category = std::forward_iterator_tag;
    using reference = decltype(std::get<1>(_lastReturned));
    using value_type = Decay<reference>;
    using difference_type = std::ptrdiff_t;
    using pointer = FakePointerProxy<reference>;

    constexpr GenerateWhileIterator() = default;

    constexpr GenerateWhileIterator(GeneratorFunc generatorFunc, std::tuple<Args...> args, const bool isEndIterator) :
        _generator(std::move(generatorFunc)),
        _args(std::move(args)),
        _lastReturned(tupleInvoker(_generator, _args, MakeIndexSequence<sizeof...(Args)>())) {
        if (isEndIterator) {
            std::get<0>(_lastReturned) = false;
        }
    }

    LZ_NODISCARD constexpr reference operator*() {
        return std::get<1>(_lastReturned);
    }

    LZ_NODISCARD constexpr pointer operator->() const {
        return FakePointerProxy<decltype(**this)>(**this);
    }

    LZ_CONSTEXPR_CXX_14 GenerateWhileIterator& operator++() {
        _lastReturned = tupleInvoker(_generator, _args, MakeIndexSequence<sizeof...(Args)>());
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