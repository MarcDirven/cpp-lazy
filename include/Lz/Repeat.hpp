#pragma once

#include <utility>
#include <Lz/detail/RepeatIterator.hpp>
#include <Lz/detail/LzTools.hpp>


namespace lz {
    template<class T>
    class Repeat {
    public:
        using iterator = detail::RepeatIterator<T>;
        using value_type = T;

    private:
        detail::RepeatIteratorHelper<T> _iteratorHelper;
        size_t _amount;

    public:
        /**
         * @brief Repeat object constructor.
         * @param toRepeat The value to repeat `amount` times.
         * @param amount The amount of times to repeat the loop, returning `toRepeat`.
         */
        Repeat(T&& toRepeat, size_t amount):
            _iteratorHelper{std::move(toRepeat), amount == std::numeric_limits<size_t>::max()},
            _amount(amount){
        }

        /**
         * @brief Returns the beginning of the sequence.
         * @return The beginning of the sequence.
         */
        iterator begin() const {
            return iterator(&_iteratorHelper, 0);
        }

        /**
         * @brief Returns the ending of the sequence.
         * @return The ending of the sequence.
         */
        iterator end() const {
            return iterator(&_iteratorHelper, _amount);
        }

        /**
         * @brief Creates a container from this repeat object. The container will consists of i.e.:
         * `SomeContainer<value_type>` with the values `toRepeat`, `amount` of times.
         * @details There is no need to specify its value type. So e.g. `to<std::list>()` will make a `std::list`
         * container, containing the `value_type`.
         * @tparam ContainerType The type of the container. The first two parameters of this container must be in
         * an STL-like fashion e.g. `std::list(InputIterator begin, InputIterator end, args). The args can be `void`,
         * but can be specified to pass an allocator or other parameters, depending on the signature of the container.
         * @tparam Args This is automatically deduced.
         * @param args Additional arguments for the container constructor. Mostly, this will be an allocator.
         * @return A container of type ContainerType<value_type[,Args...]>.
         */
        template<template<typename, typename...> class Container, typename... Args>
        Container<value_type, Args...> to(Args&&... args) const {
            return Container<value_type, Args...>(begin(), end(), std::forward<Args>(args)...);
        }

        /**
        * @brief Creates a `std::vector<value_type>` with default `std::allocator` with the values `toRepeat`, `amount`
         * of times.
        * @return A `std::vector<value_type>` with the values with the values `toRepeat`, `amount` of times and a
         * default `std::allocator`.
        */
        std::vector<value_type> toVector() const {
            return toVector<std::allocator<value_type>>();
        }

        /**
         * @brief Creates a `std::vector<value_type, Allocator>` with a specified Allocator, with the values with the
         * values `toRepeat`, `amount` of times
         * @tparam Allocator The allocator type, is automatic deduced.
         * @param alloc An instance of the allocator.
         * @return A `std::vector<value_type, Allocator>` with a specified Allocator and with the values with the values
         * `toRepeat`, `amount` of times and a specified Allocator.
         */

        template<typename Allocator>
        std::vector<value_type, Allocator> toVector(const Allocator& alloc = Allocator()) const {
            return std::vector<value_type, Allocator>(begin(), end(), alloc);
        }

        /**
         * @brief Creates a `std::array<value_type, N>` with the values `toRepeat`, `amount` of times.
         * @tparam N The size of the array.
         * @return A `std::array<value_type, N>` with the values with the values `toRepeat`, `amount` of times.
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
     * @brief Returns `toRepeat`, `amount` of times. If amount is left empty, it never stops and is interpreted as a
     * `while-true` loop.
     * @tparam T Is automatically deduced by the compiler.
     * @param toRepeat The value to repeat `amount` times.
     * @param amount The amount of times to repeat the loop, returning `toRepeat`.
     * @return A repeat object, containing the random access iterator.
     */
    template<class T>
    auto repeat(T toRepeat, size_t amount = std::numeric_limits<size_t>::max()) {
        return Repeat<T>(std::move(toRepeat), amount);
    }

    //End of group
    /**
     * @}
     */
}