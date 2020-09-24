#pragma once

#ifndef LZ_MAP_HPP
#define LZ_MAP_HPP

#include "detail/BasicIteratorView.hpp"
#include "detail/MapIterator.hpp"


namespace lz {
    template<LZ_CONCEPT_ITERATOR Iterator, class Function>
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
        Map(const Iterator begin, const Iterator end, const Function& function) :
            _begin(begin, function),
            _end(end, function) {
        }

        Map() = default;

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
     * @tparam Iterator Is automatically deduced.
     * @tparam Function Is automatically deduced.
     * @param begin The beginning of the sequence.
     * @param end The ending of the sequence.
     * @param function A function that takes a value type as parameter. It may return anything.
     * @return A Map object from [begin, end) that can be converted to an arbitrary container or can be iterated over
     * using `for (auto... lz::map(...))`.
     */
    template<class Function, LZ_CONCEPT_ITERATOR Iterator>
    Map<Iterator, Function> mapRange(const Iterator begin, const Iterator end, const Function& function) {
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
    template<class Function, LZ_CONCEPT_ITERABLE Iterable>
    Map<detail::IterType<Iterable>, Function> map(Iterable&& iterable, const Function& function) {
        return mapRange(std::begin(iterable), std::end(iterable), function);
    }

    // End of group
    /**
     * @}
     */
}

#endif