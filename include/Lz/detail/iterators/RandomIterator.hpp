#pragma once

#ifndef LZ_RANDOM_ITERATOR_HPP
#define LZ_RANDOM_ITERATOR_HPP

#include "Lz/IterBase.hpp"
#include "Lz/detail/CompilerChecks.hpp"
#include "Lz/detail/FakePointerProxy.hpp"

namespace lz {
namespace detail {
template<class Arithmetic, class Distribution, class Generator>
class RandomIterator : public IterBase<RandomIterator<Arithmetic, Distribution, Generator>, Arithmetic,
                                       FakePointerProxy<Arithmetic>, std::ptrdiff_t, std::random_access_iterator_tag> {
public:
    using iterator_category = std::random_access_iterator_tag;
    using value_type = Arithmetic;
    using difference_type = std::ptrdiff_t;
    using pointer = FakePointerProxy<Arithmetic>;
    using reference = value_type;
    using result_type = value_type;

private:
    mutable Distribution _distribution{};
    Generator* _generator{ nullptr };
    std::ptrdiff_t _current{};
    bool _isWhileTrueLoop{};

public:
    RandomIterator(const Distribution& distribution, Generator& generator, const std::ptrdiff_t current,
                   const bool isWhileTrueLoop) :
        _distribution(distribution),
        _generator(&generator),
        _current(current),
        _isWhileTrueLoop(isWhileTrueLoop) {
    }

    RandomIterator() = default;

    LZ_NODISCARD value_type dereference() const {
        return _distribution(*_generator);
    }

    LZ_NODISCARD value_type operator()() const {
        return dereference();
    }

    void increment() noexcept {
        if (!_isWhileTrueLoop) {
            ++_current;
        }
    }

    void decrement() noexcept {
        if (!_isWhileTrueLoop) {
            --_current;
        }
    }

    LZ_NODISCARD pointer arrow() const {
        return FakePointerProxy<decltype(**this)>(**this);
    }

    LZ_NODISCARD result_type(min)() const noexcept {
        return (_distribution->min)();
    }

    LZ_NODISCARD result_type(max)() const noexcept {
        return (_distribution->max)();
    }

    void plusIs(const difference_type offset) noexcept {
        if (!_isWhileTrueLoop) {
            _current += offset;
        }
    }

    LZ_NODISCARD difference_type difference(const RandomIterator& b) const noexcept {
        LZ_ASSERT(_isWhileTrueLoop == b._isWhileTrueLoop, "incompatible iterator types: both must be while true or not");
        return _current - b._current;
    }

    LZ_NODISCARD bool eq(const RandomIterator& b) const noexcept {
        LZ_ASSERT(_isWhileTrueLoop == b._isWhileTrueLoop, "incompatible iterator types: both must be while true or not");
        return _current == b._current;
    }
};
} // namespace detail
} // namespace lz

#endif