#pragma once

#include <Lz/detail/BasicIteratorView.hpp>
#include <Lz/detail/MapIterator.hpp>

#include <vector>
#include <array>
#include <map>


namespace lz {
    template<class Iterator, class Function>
    class Map final : public detail::BasicIteratorView<detail::MapIterator<Iterator, Function>> {
    public:
        using iterator = detail::MapIterator<Iterator, Function>;
        using const_iterator = iterator;

        using value_type = typename iterator::value_type;

    private:
        iterator _begin{};
        iterator _end{};

    public:
        /**
         * @brief The Map constructor.
         * @param begin Beginning of the iterator.
         * @param end End of the iterator.
         * @param function A function with parameter the value type. It may return anything.
         */
        Map(Iterator begin, Iterator end, Function function) :
            _begin(begin, function),
            _end(end, function) {
        }

        /**
        * @brief Returns the beginning of the map iterator object.
        * @return A bidirectional iterator MapIterator.
        */
        iterator begin() const override {
            return _begin;
        }

        /**
        * @brief Returns the ending of the map iterator object.
        * @return A bidirectional iterator MapIterator.
        */
        iterator end() const override {
            return _end;
        }
    };

    // Start of group
    /**
     * @addtogroup ItFns
     * @{
     */

    /**
     * @brief Returns a random access map object. If MSVC and the type is an STL iterator, pass a pointer iterator, not
     * an actual iterator object.
     * @details E.g. `map({std::pair(1, 2), std::pair(3, 2)}, [](std::pair<int, int> pairs) { return pair.first; });`
     * will return all pairs first values in the sequence, that is, `1` and `3`.
     * @tparam Iterable Is automatically deduced.
     * @tparam Function Is automatically deduced.
     * @param iterable The iterable to do the mapping over.
     * @param function A function that takes a value type as parameter. It may return anything.
     * @return A Map object from [begin, end) that can be converted to an arbitrary container or can be iterated over
     * using `for (auto... lz::map(...))`.
     */
    template<class Iterator, class Function>
    auto maprange(Iterator begin, Iterator end, Function function) {
        return Map<Iterator, Function>(begin, end, function);
    }

    /**
     * @brief Returns a bidirectional map object.
     * @details E.g. `map({std::pair(1, 2), std::pair(3, 2)}, [](std::pair<int, int> pairs) { return pair.first; });`
     * will return all pairs first values in the sequence, that is, `1` and `3`.
     * @tparam Iterable Is automatically deduced.
     * @tparam Function Is automatically deduced.
     * @param iterable The iterable to do the mapping over.
     * @param function A function that takes a value type as parameter. It may return anything.
     * @return A Map object that can be converted to an arbitrary container or can be iterated over using
     * `for (auto... lz::map(...))`.
     */
    template<class Iterable, class Function>
    auto map(Iterable&& iterable, Function function) {
#ifdef _MSC_VER
        // If MSVC Compiler is the defined, the operator + of an arbitrary STL container contains a
        // _Verify_Offset(size_t) method which causes the program to crash if the amount added to the iterator is
        // past-the-end and also causing the operator>= never to be used.
        if (iterable.begin() == iterable.end()) {  // Prevent UB when subtracting 1 and dereference it
            return maprange(&(*iterable.begin()), &(*iterable.begin()), function);
        }
        return maprange(&(*iterable.begin()), &(*(iterable.end() - 1)) + 1, function);
#else
        return maprange(iterable.begin(), iterable.end(), function);
#endif
    }

    // End of group
    /**
     * @}
     */
}
