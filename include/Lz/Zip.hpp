#pragma once

#include <vector>
#include <array>

#include <Lz/detail/LzTools.hpp>
#include <Lz/detail/ZipIterator.hpp>


namespace lz {
    template<class... Containers>
    class Zip {
    public:
        using value_type = std::tuple<typename std::decay_t<Containers>::value_type...>;

        using iterator = detail::ZipIterator<Containers...>;
        using const_iterator = iterator;

    private:
        iterator _begin;
        iterator _end;

    public:
        /**
         * @brief This object can be used to iterate over multiple containers. It stops at its smallest container.
         * Its `begin()` function returns a random access iterator. The operators `<, <=, >, >=` will return true
         * if one of the containers returns true with its corresponding `operator<`/`operator<=`/`operator>`/
         * `operator>=`.
         * @details The tuple that is returned by `operator*` returns a `std::tuple` by value and its elements by
         * reference e.g. `std::tuple<Args&...>`. So it is possible to alter the values in the container/iterable),
         * unless the iterator is const, making it a const reference.
         * to alter the values in the iterator (and therefore also the container/iterable), unless the iterator is const,
         * making it a const reference.
         * @param containers
         */
        explicit Zip(Containers&& ... containers) :
            _begin(std::make_tuple(containers.begin()...)),
            _end(std::make_tuple(containers.end()...)) {
        }

        /**
         * @brief Returns the beginning of the zip iterator.
         * @return The beginning of the zip iterator.
         */
        iterator begin() const {
            return _begin;
        }

        /**
         * @brief Returns the ending of the zip iterator.
         * @return The ending of the zip iterator.
         */
        iterator end() const {
            return _end;
        }

        /**
         * @brief Creates a container from this enumerate object. The container will consists of i.e.:
         * `SomeContainer<std::tuple<T...>>`.
         * @details There is no need to specify its value type. So e.g. `to<std::list>()` will make a `std::list`
         * container, containing a `std::tuple<T...>`
         * @tparam ContainerType The type of the container. The first two parameters of this container must be in
         * an STL-like fashion e.g. `std::list(InputIterator begin, InputIterator end, args). The args can be `void`,
         * but can be specified to pass an allocator or other parameters, depending on the signature of the container.
         * @tparam Args This is automatically deduced.
         * @param args Additional arguments for the container constructor. Mostly, this will be an allocator.
         * @return A container of type `ContainerType<std::tuple<T...>[, Args...]>`.
         */
        template<template<class, class...> class ContainerType, class... Args>
        ContainerType<value_type, Args...> to() const {
            return ContainerType<value_type, Args...>(begin(), end());
        }

        /**
        * @brief Creates a `std::vector<std::tuple<T...>>` with default `std::allocator`.
        * @return A `std::vector<std::tuple<T...>>` and a default `std::allocator`.
        */
        std::vector<value_type> toVector() const {
            return toVector < std::allocator<value_type>>
            ();
        }

        /**
         * @brief Creates a `std::vector<value_type, Allocator>` with a specified Allocator.
         * @tparam Allocator The allocator type, is automatic deduced.
         * @param alloc An instance of the allocator.
         * @return A `std::vector<std::tuple<T...>, Allocator>` with a specified Allocator.
         */
        template<typename Allocator>
        std::vector<value_type, Allocator> toVector(const Allocator& alloc = Allocator()) const {
            return std::vector<value_type, Allocator>(begin(), end(), alloc);
        }

        /**
         * @brief Creates a `std::array<value_type, N>`.
         * @tparam N The size of the array.
         * @return A `std::array<std::tuple<T...>, N>`.
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
     * @brief This function can be used to iterate over multiple containers. It stops at its smallest container.
     * Its `begin()` function returns a random access iterator. The operators `<, <=, >, >=` will return true
     * if one of the containers returns true with its corresponding `operator<`/`operator<=`/`operator>`/`operator>=`.
     * @details The tuple that is returned by `operator*` returns a `std::tuple` by value and its elements by
     * reference e.g. `std::tuple<Args&...>`. So it is possible to alter the values in the container/iterable),
     * unless the iterator is const, making it a const reference.
     * @tparam Iterables Is automatically deduced.
     * @param iterables The iterables to iterate simultaneously over.
     * @return A Take object that can be converted to an arbitrary container or can be iterated over using
     * `for (auto tuple :  lz::zip(...))`.
     */
    template<class... Iterables>
    auto zip(Iterables&& ... iterables) {
        return Zip<Iterables...>(iterables...);
    }

    // End of group
    /**
     * @}
     */
}
