#pragma once


#include <Lz/detail/LzTools.hpp>
#include <Lz/detail/ExceptIterator.hpp>


namespace lz {
    template<class Iterator, class IteratorToExcept>
    class Except {
    public:
        using iterator = detail::ExceptIterator<Iterator, IteratorToExcept>;
        using const_iterator = iterator;

        using value_type = typename iterator::value_type;

    private:
        detail::ExceptIteratorHelper<Iterator, IteratorToExcept> _iteratorHelper;

    public:
        /**
         * Except constructor. Excepts all elements between [begin, end) contained by [toExceptBegin, toExceptEnd)
         * @param begin The beginning of the iterator to skip.
         * @param end The ending of the iterator to skip.
         * @param toExceptBegin The beginning of the actual elements to except.
         * @param toExceptEnd The ending of the actual elements to except.
         */
        Except(Iterator begin, Iterator end, IteratorToExcept toExceptBegin, IteratorToExcept toExceptEnd):
            _iteratorHelper{begin, end, toExceptBegin, toExceptEnd}
        {}

        /**
         * Returns an iterator to the beginning.
         * @return An iterator to the beginning.
         */
        iterator begin() const {
            iterator begin(&_iteratorHelper);
            begin.find();
            return begin;
        }

        /**
         * Returns an iterator to the ending.
         * @return An iterator to the ending.
         */
        iterator end() const {
            iterator end(&_iteratorHelper);
            return end;
        }

        /**
         * @brief Creates a container from this except object. The container will consists of i.e.:
         * `SomeContainer<value_type>` with all elements except the elements between [`toExceptBegin, toExceptEnd`).
         * @details There is no need to specify its value type. So e.g. `to<std::list>()` will make a `std::list`
         * container, containing `value_type`.
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
        * @brief Creates a `std::vector<value_type>` with default `std::allocator` with all elements except the elements
        * between [`toExceptBegin, toExceptEnd`).
        * @return A `std::vector<FunctionReturnType>` with all elements except the elements between
        * [`toExceptBegin, toExceptEnd`) and default `std::allocator`.
        */
        std::vector<value_type> toVector() const {
            return toVector<std::allocator<value_type>>();
        }

        /**
         * @brief Creates a `std::vector<value_type>` with all elements except the elements between
         * [`toExceptBegin, toExceptEnd`).
         * @tparam Allocator The allocator type, is automatic deduced.
         * @param alloc An instance of the allocator.
         * @return A `std::vector<Arithmetic, Allocator>` with a specified Allocator and with all elements except the
         * elements between [`toExceptBegin, toExceptEnd`).
         */
        template<typename Allocator>
        std::vector<value_type, Allocator> toVector(const Allocator& alloc = Allocator()) const {
            return std::vector<value_type, Allocator>(begin(), end(), alloc);
        }

        /**
         * @brief Creates a `std::array<value_type, N>` with all elements except the elements between
         * [`toExceptBegin, toExceptEnd`).
         * @tparam N The size of the array.
         * @return A `std::array<value_type, N>` with all elements except the elements between
         * [`toExceptBegin, toExceptEnd`).
         */
        template<size_t N>
        std::array<value_type, N> toArray() const {
            std::array<value_type, N> container;
            detail::fillContainer(begin(), container);
            return container;
        }
    };

    /**
     *
     * @tparam Iterator
     * @tparam IteratorToExcept
     * @param begin
     * @param end
     * @param toExceptBegin
     * @param toExceptEnd
     * @return
     */
    template<class Iterator, class IteratorToExcept>
    auto exceptrange(Iterator begin, Iterator end, IteratorToExcept toExceptBegin, IteratorToExcept toExceptEnd) {
        return Except<Iterator, IteratorToExcept>(begin, end, toExceptBegin, toExceptEnd);
    }

    /**
     *
     * @tparam Iterable
     * @tparam IterableToExcept
     * @param iterable
     * @param toExcept
     * @return
     */
    template<class Iterable, class IterableToExcept>
    auto except(Iterable&& iterable, IterableToExcept&& toExcept) {
        return exceptrange(iterable.begin(), iterable.end(), toExcept.begin(), toExcept.end());
    }
}