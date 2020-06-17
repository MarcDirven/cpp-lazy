#pragma once

#include <Lz/detail/LzTools.hpp>
#include <Lz/detail/MapIterator.hpp>

#include <vector>
#include <array>


namespace lz {
    template<class Iterator, class Function>
    class Map {
    public:
        using iterator = detail::MapIterator<Iterator, Function>;
        using const_iterator = iterator;

        using value_type = typename iterator::value_type;

    private:
        iterator _begin{};
        iterator _end{};

    public:
        /**
         * @brief The Map constructor.
         * @param begin Beginning of the iterator.
         * @param end End of the iterator.
         * @param function A function with parameter the value type. It may return anything.
         */
        Map(Iterator begin, Iterator end, Function function) :
            _begin(begin, function),
            _end(end, function) {
        }

        /**
        * @brief Returns the beginning of the map iterator object.
        * @return A bidirectional iterator MapIterator.
        */
        iterator begin() const {
            return _begin;
        }

        /**
        * @brief Returns the ending of the map iterator object.
        * @return A bidirectional iterator MapIterator.
        */
        iterator end() const {
            return _end;
        }

        /**
         * @brief Creates a container from this map object. The container will consists of i.e.:
         * `SomeContainer<FunctionReturnType>`.
         * @details There is no need to specify its value type. So e.g. `to<std::list>()` will make a `std::list`
         * container, containing the function return value.
         * @tparam ContainerType The type of the container. The first two parameters of this container must be in
         * an STL-like fashion e.g. `std::list(InputIterator begin, InputIterator end, args). The args can be `void`,
         * but can be specified to pass an allocator or other parameters, depending on the signature of the container.
         * @tparam Args This is automatically deduced.
         * @param args Additional arguments for the container constructor. Mostly, this will be an allocator.
         * @return A container of type `ContainerType<FunctionReturnType [, Args...]>`.
         */
        template<template<typename, typename...> class Container, typename... Args>
        Container<value_type, Args...> to() const {
            return Container<value_type, Args...>(begin(), end());
        }

        /**
        * @brief Creates a `std::vector<FunctionReturnType>` with default `std::allocator` and mapped elements.
        * @return A `std::vector<FunctionReturnType>` with the mapped elements with default `std::allocator`.
        */
        std::vector<value_type> toVector() const {
            return toVector<std::allocator<value_type>>();
        }

        /**
         * @brief Creates a `std::vector<FunctionReturnType>` with a specified Allocator and filtered elements.
         * @tparam Allocator The allocator type, is automatic deduced.
         * @param alloc An instance of the allocator.
         * @return A `std::vector<FunctionReturnType, Allocator>` with a specified Allocator and mapped elements.
         */
        template<typename Allocator>
        std::vector<value_type, Allocator> toVector(const Allocator& alloc = Allocator()) const {
            return std::vector<value_type, Allocator>(begin(), end(), alloc);
        }

        /**
         * @brief Creates a `std::array<FunctionReturnType, N>` with mapped elements.
         * @tparam N The size of the array.
         * @return A `std::array<FunctionReturnType, N>` with mapped elements.
         */
        template<size_t N>
        std::array<value_type, N> toArray() const {
            return detail::fillArray<value_type, N>(begin());
        }
    };

    // Start of group
    /**
     * @addtogroup ItFns
     * @{
     */

    /**
     * @brief Returns a bidirectional map object.
     * @details E.g. `map({std::pair(1, 2), std::pair(3, 2)}, [](std::pair<int, int> pairs) { return pair.first; });`
     * will return all pairs first values in the sequence, that is, `1` and `3`.
     * @tparam Iterable Is automatically deduced.
     * @tparam Function Is automatically deduced.
     * @param iterable The iterable to do the mapping over.
     * @param function A function that takes a value type as parameter. It may return anything.
     * @return A Map object from [begin, end) that can be converted to an arbitrary container or can be iterated over
     * using `for (auto... lz::map(...))`.
     */
    template<class Iterator, class Function>
    auto maprange(Iterator begin, Iterator end, Function function) {
        return Map<Iterator, Function>(begin, end, function);
    }

    /**
     * @brief Returns a bidirectional map object.
     * @details E.g. `map({std::pair(1, 2), std::pair(3, 2)}, [](std::pair<int, int> pairs) { return pair.first; });`
     * will return all pairs first values in the sequence, that is, `1` and `3`.
     * @tparam Iterable Is automatically deduced.
     * @tparam Function Is automatically deduced.
     * @param iterable The iterable to do the mapping over.
     * @param function A function that takes a value type as parameter. It may return anything.
     * @return A Map object that can be converted to an arbitrary container or can be iterated over using
     * `for (auto... lz::map(...))`.
     */
    template<class Iterable, class Function>
    auto map(Iterable&& iterable, Function function) {
        return maprange(iterable.begin(), iterable.end(), function);
    }

    // End of group
    /**
     * @}
     */
}
