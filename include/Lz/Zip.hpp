#pragma once

#include <vector>
#include <array>

#include <Lz/detail/LzTools.hpp>
#include <Lz/detail/ZipIterator.hpp>


namespace lz {
    template<class... Containers>
    class Zip {
        using RemovedRefValueType = std::tuple<typename std::decay_t<Containers>::value_type...>;

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
        ContainerType<RemovedRefValueType, Args...> to() const {
            return ContainerType<RemovedRefValueType, Args...>(begin(), end());
        }

        std::vector<value_type> toVector() const {
            return toVector<std::allocator<value_type>>();
        }

        template<typename Allocator>
        std::vector<value_type, Allocator> toVector(const Allocator& alloc = Allocator()) const {
            return detail::makeVector<value_type>(begin(), end(), alloc);
        }

        template<size_t N>
        std::array<RemovedRefValueType, N> toArray() const {
            return detail::fillArray<RemovedRefValueType, N>(begin());
        }
    };

    template<class... Containers>
    Zip<Containers...> zip(Containers&& ... containers) {
        return Zip<Containers...>(containers...);
    }
}
