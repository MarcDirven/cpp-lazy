#pragma once

#ifndef LZ_RANDOM_ITERATOR_HPP
#define LZ_RANDOM_ITERATOR_HPP

#include "Lz/detail/compiler_checks.hpp"
#include "Lz/detail/fake_ptr_proxy.hpp"
#include "Lz/iter_base.hpp"


namespace lz {
namespace detail {
template<class Arithmetic, class Distribution, class Generator>
class random_iterator
    : public iter_base<random_iterator<Arithmetic, Distribution, Generator>, Arithmetic, fake_ptr_proxy<Arithmetic>,
                       std::ptrdiff_t, std::forward_iterator_tag, default_sentinel> {
public:
    using value_type = Arithmetic;
    using difference_type = std::ptrdiff_t;
    using pointer = fake_ptr_proxy<Arithmetic>;
    using reference = value_type;
    using result_type = value_type;

private:
    mutable Distribution _distribution{};
    Generator* _generator{ nullptr };
    std::ptrdiff_t _current{};

public:
    random_iterator(const Distribution& distribution, Generator& generator, const std::ptrdiff_t current) :
        _distribution(distribution),
        _generator(&generator),
        _current(current) {
    }

    random_iterator() = default;

    LZ_NODISCARD value_type dereference() const {
        return _distribution(*_generator);
    }

    LZ_NODISCARD value_type operator()() const {
        return dereference();
    }

    void increment() noexcept {
        --_current;
    }

    LZ_NODISCARD pointer arrow() const {
        return fake_ptr_proxy<decltype(**this)>(**this);
    }

    LZ_NODISCARD result_type(min)() const noexcept {
        return (_distribution->min)();
    }

    LZ_NODISCARD result_type(max)() const noexcept {
        return (_distribution->max)();
    }

    LZ_NODISCARD difference_type difference(const random_iterator& b) const noexcept {
        return _current - b._current;
    }

    LZ_NODISCARD bool eq(const random_iterator& b) const noexcept {
        return _current == b._current;
    }

    LZ_NODISCARD bool eq(default_sentinel) const noexcept {
        return _current == 0;
    }
};
} // namespace detail
} // namespace lz

#endif