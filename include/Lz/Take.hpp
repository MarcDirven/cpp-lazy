#pragma once

#include <vector>
#include <array>

#include <Lz/detail/TakeIterator.hpp>
#include <Lz/detail/LzTools.hpp>


namespace lz {
    template<class Iterator, class Function>
    class Take {
    public:
        using iterator = detail::TakeIterator<Iterator, Function>;
        using const_iterator = iterator;

        using value_type = typename std::iterator_traits<Iterator>::value_type;

    private:
        iterator _begin{};
        iterator _end{};

    public:
        /**
         * @brief Takes elements from an iterator from [begin, ...) while the function returns true. If the function
         * returns false, the iterator stops.
         * @param begin The beginning of the iterator.
         * @param end The ending of the iterator.
         * @param function Function that must contain a the value type in its arguments and must return a bool. If the
         * function returns false, the iterator stops.
         */
        Take(Iterator begin, Iterator end, Function function):
            _begin(begin, end, function),
            _end(begin, end, function)
        {
        }

        /**
         * @brief Returns the beginning of the iterator.
         * @return The beginning of the iterator.
         */
        iterator begin() const {
            return _begin;
        }

        /**
         * @brief Returns the ending of the iterator.
         * @return The ending of the iterator.
         */
        iterator end() const {
            return _end;
        }

        /**
         * @brief Creates a container from this take object. The container will consists of i.e.:
         * `SomeContainer<value_type>` with the values while the function returns true.
         * @details There is no need to specify its value type. So e.g. `to<std::list>()` will make a `std::list`
         * container, containing the `value_type`.
         * @tparam ContainerType The type of the container. The first two parameters of this container must be in
         * an STL-like fashion e.g. `std::list(InputIterator begin, InputIterator end, args). The args can be `void`,
         * but can be specified to pass an allocator or other parameters, depending on the signature of the container.
         * @tparam Args This is automatically deduced.
         * @param args Additional arguments for the container constructor. Mostly, this will be an allocator.
         * @return A container of type ContainerType<value_type[,Args...]>.
         */
        template<template<class, class...> class ContainerType, class... Args>
        ContainerType<value_type, Args...> to() const {
            return ContainerType<value_type, Args...>(begin(), end());
        }

        /**
        * @brief Creates a `std::vector<value_type>` with default `std::allocator` with the values while the function
        * returns true.
        * @return A `std::vector<value_type>` with the values while the function returns true and a default
        * `std::allocator`.
        */
        std::vector<value_type> toVector() const {
            return toVector<std::allocator<value_type>>();
        }

        /**
         * @brief Creates a `std::vector<value_type, Allocator>` with a specified Allocator, with the values while the
         * function returns true.
         * @tparam Allocator The allocator type, is automatic deduced.
         * @param alloc An instance of the allocator.
         * @return A `std::vector<value_type, Allocator>` with a specified Allocator and with the values while the
         * function returns true and a specified Allocator.
         */
        template<typename Allocator>
        std::vector<value_type, Allocator> toVector(const Allocator& alloc = Allocator()) const {
            return std::vector<value_type, Allocator>(begin(), end(), alloc);
        }

        /**
         * @brief Creates a `std::array<value_type, N>` with the values while the function returns true.
         * @tparam N The size of the array.
         * @return A `std::array<value_type, N>`  with the values while the function returns true.
         */
        template<size_t N>
        std::array<value_type, N> toArray() const {
            return detail::fillArray<value_type, N>(begin());
        }
    };

    // Start of group
    /**
     * @defgroup ItFns Iterator free functions.
     * These are the iterator functions and can all be used to iterate over in a
     * `for (auto var : lz::someiterator(...))`-like fashion. Also, all objects contain a `toVector`,
     * `toVector<Allocator>`, `toArray<N>`, `to<container>` (specifying its value type of the container is not
     *  necessary, so e.g. `to<std::list>()` will do), `begin()`, `end()` methods and `value_type` and `iterator`
     *  typedefs.
     * @{
     */

    /**
     * @brief Takes elements from an iterator from [begin, ...) while the function returns true. If the function
     * returns false, the iterator stops. Its `begin()` function returns a random access iterator.
     * @tparam Iterator Is automatically deduced.
     * @tparam Function Is automatically deduced.
     * @param begin The beginning of the iterator.
     * @param end The beginning of the iterator.
     * @param predicate A function that returns a bool and passes a value type in its argument. If the function returns
     * false, the iterator stops.
     * @return A Take object that can be converted to an arbitrary container or can be iterated over using
     * `for (auto... lz::takewhilerange(...))`.
     */
    template<class Iterator, class Function>
    auto takewhilerange(Iterator begin, Iterator end, Function predicate) {
        return Take<Iterator, Function>(begin, end, predicate);
    }

    /**
     * @brief This function does the same as `lz::takewhilerange` except that it takes an iterable as parameter.
     * Its `begin()` function returns a random access iterator.
     * @tparam Iterator Is automatically deduced.
     * @tparam Function Is automatically deduced.
     * @param iterable An object that has methods `begin()` and `end()`.
     * @param predicate A function that returns a bool and passes a value type in its argument. If the function returns
     * false, the iterator stops.
     * @return A Take object that can be converted to an arbitrary container or can be iterated over using
     * `for (auto... lz::takewhile(...))`.
     */
    template<class Iterable, class Function>
    auto takewhile(Iterable&& iterable, Function predicate) {
        return takewhilerange(iterable.begin(), iterable.end(), predicate);
    }

    /**
     * @brief This function takes a range between two iterators from [begin, end). Its `begin()` function returns a
     * random access iterator.
     * @tparam Iterator Is automatically deduced.
     * @param begin The beginning of the 'view'.
     * @param end The ending of the 'view'.
     * @return A Take object that can be converted to an arbitrary container or can be iterated over using
     * `for (auto... lz::takerange(...))`.
     */
    template<class Iterator>
    auto takerange(Iterator begin, Iterator end) {
        return takewhilerange(begin, end, [](const auto&) { return true; });
    }

    /**
     * @brief This function takes an iterable and slices `amount` from the beginning of the array. Essentially it is
     * equivalent to [`iterable.begin(), iterable.begin() + amount`). Its `begin()` function returns a random
     * access iterator.
     * @tparam Iterable Is automatically deduced.
     * @param iterable An iterable with method `begin()`.
     * @param amount The amount of elements to take from the beginning of the `iterable`.
     * @return A Take object that can be converted to an arbitrary container or can be iterated over using
     * `for (auto... lz::take(...))`.}@
     */
    template<class Iterable>
    auto take(Iterable&& iterable, const size_t amount) {
        auto begin = iterable.begin();
        return takerange(begin, begin + amount);
    }

    /**
     * @brief This function slices an iterable. It is equivalent to [`begin() + from, begin() + to`).
     * Its `begin()` function returns a random access iterator.
     * @tparam Iterable Is automatically deduced.
     * @param iterable An iterable with method `begin()`.
     * @param from The offset from the beginning of the iterable.
     * @param to The offset from the beginning to take. `from` must be higher than `to`.
     * @return A Take object that can be converted to an arbitrary container or can be iterated over using
     * `for (auto... lz::slice(...))`.
     */
    template<class Iterable>
    auto slice(Iterable&& iterable, const size_t from, const size_t to) {
        auto begin = iterable.begin();
        return takerange(begin + from, begin + to);
    }

    // End of group
    /**
     * @}
     */
}
