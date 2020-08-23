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
        using FnParamType = typename detail::MapIterator<Iterator, Function>::FnParamType;
        using FnReturnType = typename detail::MapIterator<Iterator, Function>::FnReturnType;

        std::function<FnReturnType(FnParamType)> _function{};
        Iterator _begin{};
        Iterator _end{};

    public:
        /**
         * @brief The Map constructor.
         * @param begin Beginning of the iterator.
         * @param end End of the iterator.
         * @param function A function with parameter the value type. It may return anything.
         */
        Map(const Iterator begin, const Iterator end, const Function& function) :
            _function(function),
            _begin(begin),
            _end(end) {
        }

        /**
        * @brief Returns the beginning of the map iterator object.
        * @return A bidirectional iterator MapIterator.
        */
        iterator begin() const override {
            return iterator(_begin, &_function);
        }

        /**
        * @brief Returns the ending of the map iterator object.
        * @return A bidirectional iterator MapIterator.
        */
        iterator end() const override {
            return iterator(_end, &_function);
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
    Map<Iterator, Function> maprange(const Iterator begin, const Iterator end, const Function& function) {
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
    auto map(Iterable&& iterable, const Function& function) -> Map<decltype(std::begin(iterable)), Function> {
        return maprange(std::begin(iterable), std::end(iterable), function);
    }

    // End of group
    /**
     * @}
     */
}
