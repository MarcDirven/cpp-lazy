#pragma once

#include <vector>
#include <array>

#include "detail/TakeIterator.hpp"
#include "detail/BasicIteratorView.hpp"


namespace lz {
    template<class Iterator, class Function>
    class Take final : public detail::BasicIteratorView<detail::TakeIterator<Iterator, Function>> {
    public:
        using iterator = detail::TakeIterator<Iterator, Function>;
        using const_iterator = iterator;

        using value_type = typename std::iterator_traits<Iterator>::value_type;

    private:
        std::function<value_type(value_type)> _predicate{};
        Iterator _begin{};
        Iterator _end{};

    public:
        /**
         * @brief Takes elements from an iterator from [begin, ...) while the function returns true. If the function
         * returns false, the iterator stops.
         * @param begin The beginning of the iterator.
         * @param end The ending of the iterator.
         * @param predicate Function that must contain a the value type in its arguments and must return a bool. If the
         * function returns false, the iterator stops.
         */
        Take(const Iterator begin, const Iterator end, const Function& predicate) :
            _predicate(predicate),
            _begin(begin),
            _end(end) {
        }

        /**
         * @brief Returns the beginning of the iterator.
         * @return The beginning of the iterator.
         */
        iterator begin() const override {
            return iterator(_begin, _end, &_predicate);
        }

        /**
         * @brief Returns the ending of the iterator.
         * @return The ending of the iterator.
         */
        iterator end() const override {
            return iterator(_end, _end, &_predicate);
        }
    };

    // Start of group
    /**
     * @defgroup ItFns Iterator free functions.
     * These are the iterator functions and can all be used to iterate over in a
     * `for (auto var : lz::someiterator(...))`-like fashion. Also, all objects contain a `toVector`,
     * `toVector<Allocator>`, `toArray<N>`, `to<container>. toMap, toUnorderedMap` (specifying its value type of the container is not
     *  necessary, so e.g. `to<std::list>()` will do), `begin()`, `end()` methods and `value_type` and `iterator`
     *  typedefs.
     * @{
     */

    /**
     * @brief Takes elements from an iterator from [begin, ...) while the function returns true. If the function
     * returns false, the iterator stops. Its `begin()` function returns a random access iterator.
     * If MSVC and the type is an STL iterator, pass a pointer iterator, not an actual iterator object.
     * @tparam Iterator Is automatically deduced.
     * @tparam Function Is automatically deduced.
     * @param begin The beginning of the iterator.
     * @param end The beginning of the iterator.
     * @param predicate A function that returns a bool and passes a value type in its argument. If the function returns
     * false, the iterator stops.
     * @return A Take object that can be converted to an arbitrary container or can be iterated over using
     * `for (auto... lz::takewhilerange(...))`.
     */
    template<class Iterator, class Function>
    Take<Iterator, Function> takewhilerange(const Iterator begin, const Iterator end, const Function& predicate) {
        return Take<Iterator, Function>(begin, end, predicate);
    }

    /**
     * @brief This function does the same as `lz::takewhilerange` except that it takes an iterable as parameter.
     * Its `begin()` function returns a random access iterator.
     * @tparam Iterator Is automatically deduced.
     * @tparam Function Is automatically deduced.
     * @param iterable An object that has methods `begin()` and `end()`.
     * @param predicate A function that returns a bool and passes a value type in its argument. If the function returns
     * false, the iterator stops.
     * @return A Take object that can be converted to an arbitrary container or can be iterated over using
     * `for (auto... lz::takewhile(...))`.
     */
    template<class Iterable, class Function>
    auto takewhile(Iterable&& iterable, const Function& predicate) -> Take<decltype(std::begin(iterable)), Function> {
        return takewhilerange(std::begin(iterable), std::end(iterable), predicate);
    }

    /**
     * @brief This function takes a range between two iterators from [begin, end). Its `begin()` function returns a
     * random access iterator. If MSVC and the type is an STL iterator, pass a pointer iterator, not an actual
     * iterator object.
     * @tparam Iterator Is automatically deduced.
     * @param begin The beginning of the 'view'.
     * @param end The ending of the 'view'.
     * @return A Take object that can be converted to an arbitrary container or can be iterated over using
     * `for (auto... lz::takerange(...))`.
     */
    template<class Iterator>
    auto takerange(const Iterator begin, const Iterator end) {
        using ValueType = typename std::iterator_traits<Iterator>::value_type;
        return takewhilerange(begin, end, [](const ValueType&) { return true; });
    }

    /**
     * @brief This function takes an iterable and slices `amount` from the beginning of the array. Essentially it is
     * equivalent to [`iterable.begin(), iterable.begin() + amount`). Its `begin()` function returns a random
     * access iterator.
     * @tparam Iterable Is automatically deduced.
     * @param iterable An iterable with method `begin()`.
     * @param amount The amount of elements to take from the beginning of the `iterable`.
     * @return A Take object that can be converted to an arbitrary container or can be iterated over using
     * `for (auto... lz::take(...))`.
     */
    template<class Iterable>
    auto take(Iterable&& iterable, const size_t amount) -> decltype(takerange(std::begin(iterable), std::begin(iterable))) {
        auto begin = std::begin(iterable);
        return takerange(begin, std::next(begin, amount));
    }

    /**
     * @brief This function slices an iterable. It is equivalent to [`begin() + from, begin() + to`).
     * Its `begin()` function returns a random access iterator.
     * @tparam Iterable Is automatically deduced.
     * @param iterable An iterable with method `begin()`.
     * @param from The offset from the beginning of the iterable.
     * @param to The offset from the beginning to take. `from` must be higher than `to`.
     * @return A Take object that can be converted to an arbitrary container or can be iterated over using
     * `for (auto... lz::slice(...))`.
     */
    template<class Iterable>
    auto slice(Iterable&& iterable, const size_t from, const size_t to) -> decltype(takerange(std::begin(iterable), std::begin(iterable))) {
        auto begin = std::begin(iterable);
        return takerange(std::next(begin, from), std::next(begin, to));
    }

    // End of group
    /**
     * @}
     */
}
