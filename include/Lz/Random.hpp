#pragma once

#include <type_traits>
#include <limits>
#include <cstddef>

#include <Lz/detail/RandomIterator.hpp>


namespace lz {
    template<class Arithmetic, class Distribution>
    class Random {
    public:
        using iterator = detail::RandomIterator<Arithmetic, Distribution>;
        using const_iterator = iterator;
        using value_type = Arithmetic;

    private:
        size_t _amount{};
        detail::RandomIteratorHelper<Arithmetic, Distribution> _helper;

    public:
        Random(Arithmetic min, Arithmetic max, size_t amount) :
            _amount(amount),
            _helper(min, max, amount) {}

        iterator begin() const {
            return iterator(0, &_helper);
        }

        iterator end() const {
            return iterator(_amount, &_helper);
        }

        template<template<typename, typename...> class Container, typename... Args>
        Container<value_type, Args...> to(Args&&... args) const {
            return Container<value_type, Args...>(begin(), end(), std::forward<Args>(args)...);
        }

        std::vector<value_type> toVector() const {
            return toVector<std::allocator<value_type>>();
        }

        template<typename Allocator>
        std::vector<value_type, Allocator> toVector(const Allocator& alloc = Allocator()) const {
            return std::vector<value_type, Allocator>(begin(), end(), alloc);
        }

        template<size_t N>
        std::array<value_type, N> toArray() const {
            std::array<value_type, N> container;
            detail::fillContainer(begin(), container);
            return container;
        }
    };

    template<class Arithmetic>
    static auto random(Arithmetic min, Arithmetic max, size_t amount = std::numeric_limits<size_t>::max()) {
        static_assert(std::is_arithmetic<Arithmetic>::value, "template parameter is not arithmetic");

        return Random<Arithmetic, std::uniform_int_distribution<Arithmetic>>(min, max, amount);
    }

    template<>
    auto random(float min, float max, size_t amount) {
        return Random<float, std::uniform_real_distribution<float>>(min, max, amount);
    }

    template<>
    auto random(double min, double max, size_t amount) {
        return Random<double, std::uniform_real_distribution<double>>(min, max, amount);
    }

    template<>
    auto random(long double min, long double max, size_t amount) {
        return Random<long double, std::uniform_real_distribution<long double>>(min, max, amount);
    }
}