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
        Enumerate(Iterator begin, Iterator end, IntType start = 0) :
            _begin(start, begin),
            _end(start, end) {
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
#ifdef _MSC_VER
    typename std::enable_if<
        detail::IsContiguousContainer<Iterable>::value,
        decltype(enumeraterange(&*std::begin(std::declval<Iterable>()),
                                &*std::begin(std::declval<Iterable>()), 0))>::type
#else
    auto
#endif
    enumerate(Iterable&& iterable, IntType start = 0) {
#ifdef _MSC_VER
        // If MSVC Compiler is the defined, the operator + of an arbitrary STL container contains a
        // _Verify_Offset(size_t) method which causes the program to crash if the amount added to the iterator is
        // past-the-end and also causing the operator>= never to be used.
        auto begin = std::begin(iterable);
        auto end = std::end(iterable);

        if (begin == end) {  // Prevent UB when subtracting 1 and dereference it
            return enumeraterange(&*begin, &*end, start);
        }

        --end;
        return enumeraterange(&*begin, &(*end) + 1, start);
#else
        return enumeraterange(std::begin(iterable), std::end(iterable), start);
#endif
    }

#ifdef _MSC_VER

    template<class IntType = int, class Iterable>
    typename std::enable_if<!detail::IsContiguousContainer<Iterable>::value,
        decltype(enumeraterange(std::begin(std::declval<Iterable>()),
                                std::begin(std::declval<Iterable>()),
                                0))>::type
    enumerate(Iterable&& iterable, IntType start = 0) {
        return enumeraterange(std::begin(iterable), std::end(iterable), start);
    }

#endif
    // End of group
    /**
     * @}
     */
}