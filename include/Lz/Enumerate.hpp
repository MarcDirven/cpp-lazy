#pragma once


#include <vector>
#include <array>

#include <Lz/detail/BasicIteratorView.hpp>
#include <Lz/detail/EnumerateIterator.hpp>


namespace lz {
    template<class Iterator, class IntType>
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
            _end(end, end) {
        }

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
     * @tparam IntType The type of the iterator integer. By default, `int` is assumed.
     * @tparam Iterator The type of the iterator. Is automatically deduced by default.
     * @param begin Beginning of the iterator.
     * @param end Ending of the iterator.
     * @param start The start of the counting index. 0 is assumed by default.
     * @return Enumerate iterator object from [begin, end).
     */
    template<class IntType = int, class Iterator>
    auto enumeraterange(const Iterator begin, const Iterator end, const IntType start = 0) {
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
    auto enumerate(Iterable&& iterable, const  IntType start = 0) {
        return enumeraterange(std::begin(iterable), std::end(iterable), start);
    }

    // End of group
    /**
     * @}
     */
}