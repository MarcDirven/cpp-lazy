#pragma once

#include <type_traits>
#include <algorithm>
#include <vector>
#include <array>

#include <Lz/it/detail/LzTools.hpp>
#include <Lz/it/detail/FilterIterator.hpp>


namespace lz { namespace it {
    template<class Iterator, class Function>
    class Filter {
    public:
        using iterator = detail::FilterIterator<Iterator, Function>;
        using const_iterator = iterator;

        using value_type = typename iterator::value_type;

    private:
        iterator _begin{};
        iterator _end{};

    public:
        /**
         * @brief The filter constructor.
         * @param begin Beginning of the iterator.
         * @param end End of the iterator.
         * @param function A function with parameter the value type of the iterable and must return a bool.
         */
        Filter(Iterator begin, Iterator end, Function function) :
            _begin(begin, end, function),
            _end(begin, end, function) {
        }

        /**
        * @brief Returns the beginning of the filter iterator object.
        * @return A forward iterator FilterIterator.
        */
        iterator begin() const {
            return _begin;
        }

        /**
        * @brief Returns the ending of the filter iterator object.
        * @return A forward iterator FilterIterator.
        */
        iterator end() const {
            return _end;
        }

        /**
         * @brief Creates a container from this filter object. The container will consists of i.e.:
         * `SomeContainer<value_type>` with the filtered elements.
         * @details There is no need to specify its value type. So e.g. `to<std::list>()` will make a `std::list`
         * container, containing a `value_type` with the filtered elements.
         * @tparam ContainerType The type of the container. The first two parameters of this container must be in
         * an STL-like fashion e.g. `std::list(InputIterator begin, InputIterator end, args). The args can be `void`,
         * but can be specified to pass an allocator or other parameters, depending on the signature of the container.
         * @tparam Args This is automatically deduced.
         * @param args Additional arguments for the container constructor. Mostly, this will be an allocator.
         * @return A container of type `ContainerType<value_type [, Args...]>`.
         */
        template<template<class, class...> class ContainerType, class... Args>
        ContainerType<value_type, Args...> to() const {
            return ContainerType<value_type, Args...>(begin(), end());
        }

        /**
        * @brief Creates a `std::vector<value_type>` with default `std::allocator` and filtered elements.
        * @return A `std::vector<value_type>` with the filtered elements with default `std::allocator`.
        */
        std::vector<value_type> toVector() const {
            return toVector<std::allocator<value_type>>();
        }

        /**
         * @brief Creates a `std::vector<value_type>` with a specified Allocator and filtered elements.
         * @tparam Allocator The allocator type, is automatic deduced.
         * @param alloc An instance of the allocator.
         * @return A `std::vector<value_type, Allocator>` with a specified Allocator and filtered elements.
         */
        template<typename Allocator>
        std::vector<value_type, Allocator> toVector(const Allocator& alloc = Allocator()) const {
            return std::vector<value_type, Allocator>(begin(), end(), alloc);
        }

        /**
         * @brief Creates a `std::array<value_type, N>` with filtered elements.
         * @tparam N The size of the array.
         * @return A `std::array<value_type, N>` with filtered elements.
         */
        template<size_t N>
        std::array<value_type, N> toArray() const {
            return detail::fillArray<value_type, N>(begin());
        }
    };

    /**
     * @addtogroup ItFns
     * @{
     */

    /**
     * @brief Returns a forward filter iterator. If the `predicate` returns false, it is excluded.
     * @details I.e. `lz::filter({1, 2, 3, 4, 5}, [](int i){ return i % 2 == 0; });` will eventually remove all
     * elements that are not even.
     * @tparam Iterator Is automatically deduced.
     * @tparam Function Is automatically deduced, but must be a function, lambda or functor.
     * @param begin The beginning of the range.
     * @param end The ending of the range.
     * @param predicate A function that must return a bool, and needs a value type of the container as parameter.
     * @return A filter object from [begin, end) that can be converted to an arbitrary container or can be iterated
     * over.
     */
    template<class Iterator, class Function>
    auto filterrange(Iterator begin, Iterator end, Function predicate) {
        return Filter<Iterator, Function>(begin, end, predicate);
    }

    /**
     * @brief Returns a forward filter iterator. If the `predicate` returns false, the value it is excluded.
     * @details I.e. `lz::filter({1, 2, 3, 4, 5}, [](int i){ return i % 2 == 0; });` will eventually remove all
     * elements that are not even.
     * @tparam Iterable Is automatically deduced.
     * @tparam Function Is automatically deduced, but must be a function, lambda or functor.
     * @param iterable An iterable, e.g. a container / object with `begin()` and `end()` methods.
     * @param predicate A function that must return a bool, and needs a value type of the container as parameter.
     * @return A filter iterator that can be converted to an arbitrary container or can be iterated
     * over using `for (auto... lz::filter(...))`.
     */
    template<class Iterable, class Function>
    auto filter(Iterable&& iterable, Function predicate) {
        return filterrange(iterable.begin(), iterable.end(), predicate);
    }

    // End of group
    /**
     * @}
     */
}}