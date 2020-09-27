#pragma once

#ifndef LZ_ENUMERATE_HPP
#define LZ_ENUMERATE_HPP

#include "detail/BasicIteratorView.hpp"
#include "detail/EnumerateIterator.hpp"


namespace lz {
    template<LZ_CONCEPT_ITERATOR Iterator, LZ_CONCEPT_INTEGRAL IntType>
    class Enumerate final : public detail::BasicIteratorView<detail::EnumerateIterator<Iterator, IntType>> {
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
        Enumerate(const Iterator begin, const Iterator end, const IntType start = 0) :
            _begin(start, begin),
            _end(static_cast<IntType>(std::distance(begin, end)), end) {
        }

        Enumerate() = default;

        /**
         * @brief Returns the beginning of the enumerate iterator object.
         * @return A random access EnumerateIterator.
         */
        iterator begin() const override {
            return _begin;
        }

        /**
         * @brief Returns the ending of the enumerate object.
         * @return A random access EnumerateIterator.
         */
        iterator end() const override {
            return _end;
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
     * @tparam IntType The type of the iterator integer. By default, `int` is assumed. Can be any arithmetic type.
     * @tparam Iterator The type of the iterator. Is automatically deduced by default.
     * @param begin Beginning of the iterator.
     * @param end Ending of the iterator.
     * @param start The start of the counting index. 0 is assumed by default.
     * @return Enumerate iterator object from [begin, end).
     */
    template<LZ_CONCEPT_INTEGRAL IntType = int, LZ_CONCEPT_ITERATOR Iterator>
    Enumerate<Iterator, IntType> enumerateRange(const Iterator begin, const Iterator end, const IntType start = 0) {
        static_assert(std::is_arithmetic<IntType>::value, "the template parameter IntType is meant for integrals only");
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
     * @tparam IntType The type of the iterator integer. By default, `int` is assumed. Can be any arithmetic type.
     * @tparam Iterable The type of the Iterable. Is automatically deduced by default.
     * @param iterable An iterable, e.g. a container / object with `begin()` and `end()` methods.
     * @param start The start of the counting index. 0 is assumed by default.
     * @return Enumerate iterator object. One can iterate over this using `for (auto pair : lz::enumerate(..))`
     */
    template<LZ_CONCEPT_INTEGRAL IntType = int, LZ_CONCEPT_ITERABLE Iterable>
    Enumerate<detail::IterType<Iterable>, IntType> enumerate(Iterable&& iterable, const  IntType start = 0) {
        return enumerateRange(std::begin(iterable), std::end(iterable), start);
    }

    // End of group
    /**
     * @}
     */
}

#endif