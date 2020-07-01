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
        /**
         * @param begin Beginning of the iterator.
         * @param end Ending of the iterator.
         * @param start The start of the counting index. 0 is assumed by default.
         */
        Enumerate(Iterator begin, Iterator end, IntType start = 0) :
            _begin(start, begin),
            _end(start, end) {
        }

        /**
         * @brief Returns the beginning of the enumerate iterator object.
         * @return A random access EnumerateIterator.
         */
        iterator begin() const {
            return _begin;
        }

        /**
         * @brief Returns the ending of the enumerate object.
         * @return A random access EnumerateIterator.
         */
        iterator end() const {
            return _end;
        }

        /**
         * @brief Creates a container from this enumerate object. The container will consists of i.e.:
         * `SomeContainer<std::pair<IntType, value_type>>`.
         * @details There is no need to specify its value type. So e.g. `to<std::list>()` will make a `std::list`
         * container, containing a `std::pair<IntType, value_type>`
         * @tparam ContainerType The type of the container. The first two parameters of this container must be in
         * an STL-like fashion e.g. `std::list(InputIterator begin, InputIterator end, args). The args can be `void`,
         * but can be specified to pass an allocator or other parameters, depending on the signature of the container.
         * @tparam Args This is automatically deduced.
         * @param args Additional arguments for the container constructor. Mostly, this will be an allocator.
         * @return A container of type `ContainerType<std::pair<IntType, value_type>[, Args...]>`.
         */
        template<template<class, class...> class ContainerType, class... Args>
        ContainerType<value_type, Args...> to(Args&& ... args) const {
            return ContainerType<value_type, Args...>(begin(), end(), std::forward<Args>(args)...);
        }

        /**
         * @brief Creates a `std::vector<std::pair<IntType, value_type>>` with default `std::allocator`.
         * @return A `std::vector<std::pair<IntType, value_type>>` with default `std::allocator`.
         */
        std::vector<value_type> toVector() const {
            return toVector < std::allocator<value_type>>
            ();
        }

        /**
         * @brief Creates a `std::vector<std::pair<IntType, value_type>>` with a specified Allocator.
         * @tparam Allocator The allocator type, is automatic deduced.
         * @param alloc An instance of the allocator.
         * @return A `std::vector<std::pair<IntType, value_type>, Allocator>` with a specified Allocator.
         */
        template<typename Allocator>
        std::vector<value_type, Allocator> toVector(const Allocator& alloc = Allocator()) const {
            return std::vector<value_type, Allocator>(begin(), end(), alloc);
        }

        /**
         * @brief Creates a `std::array<std::pair<IntType, value_type>, N>`.
         * @tparam N The size of the array.
         * @return A `std::array<std::pair<IntType, value_type>, N>`.
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
     * @brief Creates an Enumerate (random access) object from two iterators. This can be useful when an index and a
     * value type of a container is needed.
     * @details Creates an Enumerate object. The enumerator consists of a `std::pair<IntType, value_type&>`. The
     * elements of the enumerate iterator are by reference. The `std:::pair<IntType, value_type&>::first` is the
     * counter index. The `std:::pair<IntType, value_type&>::second` is the element of the iterator by reference.
     * Furthermore, the `operator*` of this iterator returns an std::pair by value.
     * @tparam IntType The type of the iterator integer. By default, `int` is assumed.
     * @tparam Iterator The type of the iterator. Is automatically deduced by default.
     * @param begin Beginning of the iterator.
     * @param end Ending of the iterator.
     * @param start The start of the counting index. 0 is assumed by default.
     * @return Enumerate iterator object from [begin, end).
     */
    template<class IntType = int, class Iterator>
    auto enumeraterange(Iterator begin, Iterator end, IntType start = 0) {
        return Enumerate<Iterator, IntType>(begin, end, start);
    }

    /**
     * @brief Creates an Enumerate (random access) object from an iterable. This can be useful when an index and a value
     * type of a iterable is needed.  If MSVC and the type is an STL  iterator, pass a pointer iterator, not an actual
     * iterator object.
     * @details Creates an Enumerate object. The enumerator consists of a `std::pair<IntType, value_type&>`. The
     * elements of the enumerate iterator are by reference. The `std:::pair<IntType, value_type&>::first` is the
     * counter index. The `std:::pair<IntType, value_type&>::second` is the element of the iterator by reference.
     * Furthermore, the `operator*` of this iterator returns an std::pair by value.
     * @tparam IntType The type of the iterator integer. By default, `int` is assumed.
     * @tparam Iterable The type of the Iterable. Is automatically deduced by default.
     * @param iterable An iterable, e.g. a container / object with `begin()` and `end()` methods.
     * @param start The start of the counting index. 0 is assumed by default.
     * @return Enumerate iterator object. One can iterate over this using `for (auto pair : lz::enumerate(..))`
     */
    template<class IntType = int, class Iterable>
    auto enumerate(Iterable&& iterable, IntType start = 0) {
#ifdef _MSC_VER
        // If MSVC Compiler is the defined, the operator + of an arbitrary STL container contains a
        // _Verify_Offset(size_t) method which causes the program to crash if the amount added to the iterator is
        // past-the-end and also causing the operator>= never to be used.
        if (iterable.begin() == iterable.end()) {  // Prevent UB when subtracting 1 and dereference it
            return enumeraterange(&(*iterable.begin()), &(*iterable.begin()), start);
        }
        return enumeraterange(&(*iterable.begin()), &(*(iterable.end() - 1)) + 1, start);
#else
        return enumeraterange(iterable.begin(), iterable.end(), start);
#endif
    }

    // End of group
    /**
     * @}
     */
}