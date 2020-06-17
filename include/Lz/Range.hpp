#pragma once

#include <vector>
#include <array>

#include <Lz/detail/LzTools.hpp>
#include <Lz/detail/RangeIterator.hpp>


namespace lz {
    template<class Arithmetic>
    class Range {
        Arithmetic _begin{};
        Arithmetic _end{};
        Arithmetic _step{};

    public:
        using iterator = detail::RangeIterator<Arithmetic>;;
        using reverse_iterator = std::reverse_iterator<iterator>;
        using value_type = typename iterator::value_type;

        /**
         * @brief Range iterator constructor from [start, end) with step.
         * @param start The start of the counting.
         * @param end The end of the counting.
         * @param step The step that gets added every iteration.
         */
        Range(Arithmetic start, Arithmetic end, Arithmetic step) :
            _begin(start),
            _end(end),
            _step(step) {
        }

        /**
         * @brief Returns the beginning of the random access Range iterator
         * @return The beginning of the random access Range iterator
         */
        iterator begin() const {
            return iterator(_begin, _end, _step);
        }

        /**
         * @brief Returns the ending of the random access Range iterator
         * @return The ending of the random access Range iterator
         */
        iterator end() const {
            return iterator(_begin, _end, _step);
        }

        /**
         * @brief Returns the reverse beginning of the random access Range iterator
         * @return The reverse beginning of the random access Range iterator
         */
        reverse_iterator rbegin() const {
            return reverse_iterator(begin());
        }

        /**
         * @brief Returns the reverse ending of the random access Range iterator
         * @return The reverse ending of the random access Range iterator
         */
        reverse_iterator rend() const {
            return reverse_iterator(end());
        }

        /**
         * @brief Creates a container from this range object. The container will consists of i.e.:
         * `SomeContainer<Arithmetic>` with ranges [start, end)[,step].
         * @details There is no need to specify its value type. So e.g. `to<std::list>()` will make a `std::list`
         * container, containing `Arithmetic`.
         * @tparam ContainerType The type of the container. The first two parameters of this container must be in
         * an STL-like fashion e.g. `std::list(InputIterator begin, InputIterator end, args). The args can be `void`,
         * but can be specified to pass an allocator or other parameters, depending on the signature of the container.
         * @tparam Args This is automatically deduced.
         * @param args Additional arguments for the container constructor. Mostly, this will be an allocator.
         * @return A container of type ContainerType<Arithmetic[,Args...]>.
         */
        template<template<typename, typename...> class Container, typename... Args>
        Container<value_type, Args...> to(Args&&... args) const {
            return Container<value_type, Args...>(begin(), end(), std::forward<Args>(args)...);
        }

        /**
        * @brief Creates a `std::vector<Arithmetic>` with default `std::allocator` with ranges [start, end).
        * @return A `std::vector<FunctionReturnType>` with the with ranges [start, end)[,step] and default
         * `std::allocator`.
        */
        std::vector<value_type> toVector() const {
            return toVector<std::allocator<value_type>>();
        }

        /**
         * @brief Creates a `std::vector<Arithmetic>` with a specified Allocator and with ranges [start, end)[,step].
         * @tparam Allocator The allocator type, is automatic deduced.
         * @param alloc An instance of the allocator.
         * @return A `std::vector<Arithmetic, Allocator>` with a specified Allocator and with ranges
         * [start, end)[,step].
         */
        template<typename Allocator>
        std::vector<value_type, Allocator> toVector(const Allocator& alloc = Allocator()) const {
            return std::vector<value_type, Allocator>(begin(), end(), alloc);
        }

        /**
         * @brief Creates a `std::array<Arithmetic, N>` with ranges [start, end)[,step].
         * @tparam N The size of the array.
         * @return A `std::array<Arithmetic, N>` with ranges [start, end)[,step].
         */
        template<size_t N>
        std::array<value_type, N> toArray() const {
            std::array<value_type, N> container;
            detail::fillContainer(begin(), container);
            return container;
        }
    };

    // Start of group
    /**
     * @addtogroup ItFns
     * @{
     */

    /**
     * @brief Returns a bidirectional range object with specified [start, end) and a step.
     * @details E.g. `lz::range(3, 20, 2)` will return all values between [3, 20) with a step of 2 when iterating over
     * its iterator.
     * @tparam Arithmetic Int is automatically assumed, may be any arithmetic type.
     * @param end Specifies when to stop iterator after `end` count. It is assumed from [start, end) with a step.
     * @param step The incrementing value after each loop.
     * @param start Specifies where to start counting.
     * @return A Range object that can be converted to an arbitrary container or can be iterated over using
     * `for (auto... lz::range(...))`.
     */
    template<class Arithmetic = int>
    Range<Arithmetic> range(const Arithmetic start, const Arithmetic end, const Arithmetic step = 1) {
        static_assert(std::is_arithmetic<Arithmetic>::value, "type must be of type arithmetic");
        return Range<Arithmetic>(start, end, step);
    }

    /**
     * @brief Returns a bidirectional range object.
     * @details E.g. `lz::range(50)` will return all values between [0, 50) when iterating over its iterator.
     * @tparam Arithmetic Int is automatically assumed, may be any arithmetic type.
     * @param end Specifies when to stop iterator after `end` count. It is assumed from [0, end).
     * @return A Range object that can be converted to an arbitrary container or can be iterated over using
     * `for (auto... lz::range(...))`.
     */
    template<class Arithmetic = int>
    Range<Arithmetic> range(const Arithmetic end) {
        return range(0, end, 1);
    }

    // End of group
    /**
     * @}
     */
}