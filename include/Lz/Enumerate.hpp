#pragma once


#include <vector>
#include <array>

#include <Lz/detail/LzTools.hpp>
#include <Lz/detail/EnumerateIterator.hpp>


namespace lz {
    template<class Iterator, class IntType>
    class Enumerate {
    public:
        using iterator = detail::EnumerateIterator<Iterator, IntType>;
        using const_iterator = iterator;

        using value_type = typename iterator::value_type;

    private:
        iterator _begin{};
        iterator _end{};

    public:
        Enumerate(Iterator begin, Iterator end, IntType start) :
            _begin(start, begin),
            _end(start, end) {
        }

        iterator begin() const {
            return _begin;
        }

        iterator end() const {
            return _end;
        }

        template<template<class, class...> class ContainerType, class... Args>
        ContainerType<value_type, Args...> to(Args&&... args) const {
            return ContainerType<value_type, Args...>(begin(), end(), std::forward<Args>(args)...);
        }

        std::vector<value_type> toVector() const {
            return toVector<std::allocator<value_type>>();
        }

        template<typename Allocator>
        std::vector<value_type, Allocator> toVector(const Allocator& alloc = Allocator()) const {
            return detail::makeVector<value_type>(begin(), end(), alloc);
        }

        template<size_t N>
        std::array<value_type, N> toArray() const {
            return detail::fillArray<value_type, N>(begin());
        }
    };

    template<class IntType = int, class Iterator>
    auto enumeraterange(Iterator begin, Iterator end, IntType start = 0) {
        return Enumerate<Iterator, IntType>(begin, end, start);
    }

    template<class IntType = int, class Container>
    auto enumerate(Container&& container, IntType start = 0) {
        return enumeraterange(container.begin(), container.end(), start);
    }
}
