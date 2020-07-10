#pragma once

#include <iterator>
#include <Lz/detail/TakeEveryIterator.hpp>
#include <Lz/detail/LzTools.hpp>


namespace lz {
    template<class Iterator>
    class TakeEvery {
    public:
        using iterator = detail::TakeEveryIterator<Iterator>;
        using const_iterator = iterator;

        using value_type = typename iterator::value_type;

    private:
        iterator _begin;
        iterator _end;

    public:
        /**
         * @brief TakeEvery constructor.
         * @param begin The beginning of the sequence.
         * @param end The ending of the sequence.
         * @param offset The offset to add each iteration, aka the amount of elements to skip.
         */
        TakeEvery(Iterator begin, Iterator end, const size_t offset):
            _begin(begin, offset),
            _end(end, offset){
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
         * @brief Creates a container from this TakeEvery object. The container will consists of i.e.:
         * `SomeContainer<value_type>` with all elements except the elements skipped by `amount`.
         * @details There is no need to specify its value type. So e.g. `to<std::list>()` will make a `std::list`
         * container, containing `value_type`.
         * @tparam ContainerType The type of the container. The first two parameters of this container must be in
         * an STL-like fashion e.g. `std::list(InputIterator begin, InputIterator end, args). The args can be `void`,
         * but can be specified to pass an allocator or other parameters, depending on the signature of the container.
         * @tparam Args This is automatically deduced.
         * @param args Additional arguments for the container constructor. Mostly, this will be an allocator.
         * @return A container of type ContainerType<Arithmetic[,Args...]>.
         */
        template<template<class, class...> class ContainerType, class... Args>
        ContainerType<value_type, Args...> to() const {
            return ContainerType<value_type, Args...>(begin(), end());
        }

        /**
        * @brief Creates a `std::vector<value_type>` with default `std::allocator` with all elements except the elements
        * skipped by `amount`.
        * @return A `std::vector<FunctionReturnType>` with all elements except the elements skipped by `amount` and
        * default `std::allocator`.
        */
        std::vector<value_type> toVector() const {
            return toVector<std::allocator<value_type>>();
        }

        /**
         * @brief Creates a `std::vector<value_type>` with all elements except the elements skipped by `amount`.
         * @tparam Allocator The allocator type, is automatic deduced.
         * @param alloc An instance of the allocator.
         * @return A `std::vector<Arithmetic, Allocator>` with a specified Allocator and with all elements except the
         * elements skipped by `amount`.
         */
        template<typename Allocator>
        std::vector<value_type, Allocator> toVector(const Allocator& alloc = Allocator()) const {
            return std::vector<value_type, Allocator>(begin(), end(), alloc);
        }

        /**
         * @brief Creates a `std::array<value_type, N>` with all elements except the elements skipped by `amount`.
         * @tparam N The size of the array.
         * @return A `std::array<value_type, N>` with all elements except the elements skipped by `amount`.
         */
        template<size_t N>
        std::array<value_type, N> toArray() const {
            return detail::fillArray<value_type, N>(begin());
        }
    };

    /**
     * @brief This random access iterator can be used to select elements with `offset` amount. If MSVC and the type is
     * an STL iterator, pass a pointer iterator, not an actual iterator object.
     * @details If one would like to select every 2nd argument one can use this iterator. Example (psuedocode):
     * `takeevery({1, 2, 3}, 2)`. This will select `1` and `3`. If you would like to skip the first element as well
     * one can use: `takeevery({1, 2, 3}, 2, 2)` the second `2` is the start position, making it select only `3`.
     * @tparam Iterator Is automatically deduced.
     * @param begin The beginning of the sequence.
     * @param end The ending of the sequence.
     * @param offset The index to add every iteration, aka the index to 'select'.
     * @param start The start position, optional. Can be used to skip the first element as well.
     * @return A TakeEvery object.
     */
    template<class Iterator>
    TakeEvery<Iterator> takeeveryrange(Iterator begin, Iterator end, const size_t offset, const size_t start = 0) {
        return TakeEvery<Iterator>(begin + start, end, offset);
    }

    /**
     * @brief This random access iterator can be used to select elements with `offset` amount.
     * @details If one would like to select every 2nd argument one can use this iterator. Example (psuedocode):
     * `takeevery({1, 2, 3}, 2)`. This will select `1` and `3`. If you would like to skip the first element as well
     * one can use: `takeevery({1, 2, 3}, 2, 2)` the second `2` is the start position, making it select only `3`.
     * @tparam Iterable Is automatically deduced.
     * @param iterable An object that can be iterated over.
     * @param offset The index to add every iteration, aka the index to 'select'.
     * @param start The start position, optional. Can be used to skip the first element as well.
     * @return A TakeEvery object.
     */
    template<class Iterable>
    auto takeevery(Iterable&& iterable, const size_t offset, const size_t start = 0) {
#ifdef _MSC_VER
        // If MSVC Compiler is the defined, the operator + of an arbitrary STL container contains a
        // _Verify_Offset(size_t) method which causes the program to crash if the amount added to the iterator is
        // past-the-end and also causing the operator>= never to be used.
        if (iterable.begin() == iterable.end()) { // Prevent UB when subtracting 1 and dereference it
            return takeeveryrange(&(*iterable.begin(), &(*iterable.begin()), offset, start));
        }
        return takeeveryrange(&(*iterable.begin()), &(*(iterable.end() - 1)) + 1, offset, start);
#else
        return takeeveryrange(iterable.begin(), iterable.end(), offset, start);
#endif
    }
}