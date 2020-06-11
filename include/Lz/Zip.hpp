#pragma once

#include <vector>
#include <array>

#include <Lz/detail/LzTools.hpp>
#include <Lz/detail/ZipIterator.hpp>


namespace lz {
    template<class... Containers>
    class Zip {
    public:
        using value_type = std::tuple<decltype(*std::declval<Containers>().begin())...>;

        using iterator = detail::ZipIterator<Containers...>;
        using const_iterator = iterator;

    private:
        iterator _begin;
        iterator _end;

    public:
        explicit Zip(Containers&& ... containers) :
            _begin(std::make_tuple(containers.begin()...)),
            _end(std::make_tuple(containers.end()...)) {
        }

        iterator begin() const {
            return _begin;
        }

        iterator end() const {
            return _end;
        }

        template<template<class, class...> class ContainerType, class... Args>
        ContainerType<value_type, Args...> to() const {
            return ContainerType<value_type, Args...>(begin(), end());
        }

        std::vector<value_type> toVector() const {
            return to<std::vector>();
        }

        template<size_t N>
        std::array<value_type, N> toArray() const {
            return detail::fillArray<N>(begin());
        }
    };

    template<class... Containers>
    Zip<Containers...> zip(Containers&& ... containers) {
        return Zip<Containers...>(containers...);
    }
}
