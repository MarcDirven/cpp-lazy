#pragma once

#ifndef LZ_FUNCTION_TOOLS_HPP
#define LZ_FUNCTION_TOOLS_HPP

#include <numeric>
#include <iterator>
#include <algorithm>

#include "StringSplitter.hpp"
#include "Join.hpp"
#include "Zip.hpp"
#include "Map.hpp"
#include "Filter.hpp"

#ifndef CXX_LT_17
  #define LZ_INLINE_VAR inline
#else
  #define LZ_INLINE_VAR
#endif


namespace lz {
    namespace detail {
        template<class Iterable>
        using IterType = std::decay_t<decltype(std::begin(std::declval<Iterable>()))>;

        template<class Iterator>
        using ValueType = typename std::iterator_traits<Iterator>::value_type;

        template<class Iterator>
        using DifferenceType = typename std::iterator_traits<Iterator>::difference_type;

        template<class Iterable>
        using ValueTypeIterable = typename std::iterator_traits<IterType<Iterable>>::value_type;
    }

    /**
     * Gets the mean of a sequence.
     * @tparam Iterator Is automatically deduced.
     * @param begin The beginning of the sequence.
     * @param end The ending of the sequence.
     * @return The mean of the sequence.
     */
    template<class Iterator>
    double mean(const Iterator begin, const Iterator end) {
        const detail::DifferenceType<Iterator> distance = std::distance(begin, end);
        const detail::ValueType<Iterator> sum = std::accumulate(begin, end, detail::ValueType<Iterator>(0));
        return static_cast<double>(sum) / distance;
    }

    /**
     * Gets the mean of a sequence.
     * @tparam Iterable Is automatically deduced.
     * @param container The container to calculate the mean of.
     * @return The mean of the container.
     */
    template<class Iterable>
    double mean(const Iterable& container) {
        return mean(std::begin(container), std::end(container));
    }

    /**
     * Gets the median of a sequence.
     * @tparam Iterator Is automatically deduced.
     * @tparam Compare Is automatically deduced.
     * @param begin The beginning of the sequence
     * @param end The ending of the sequence
     * @param compare The sequence gets sorted with nth_element. A default operator of < is used, however a custom compare can be used.
     * @return The median of the sequence.
     */
    template<class Iterator, class Compare>
    double median(const Iterator begin, const Iterator end, const Compare compare) {
        const detail::DifferenceType<Iterator> len = std::distance(begin, end);
        if (len == 0) {
            throw std::invalid_argument(__LZ_FILE_LINE__ ": the length of the sequence cannot be 0");
        }

        const detail::DifferenceType<Iterator> mid = len >> 1;
        const Iterator midIter = std::next(begin, mid);
        std::nth_element(begin, midIter, end, compare);

        if (detail::isEven(len)) {
            const Iterator leftHalf = std::max_element(begin, midIter);
            return (*leftHalf + *midIter) / 2.;
        }
        return *midIter;
    }

    /**
     * Gets the median of a sequence.
     * @tparam Iterable Is automatically deduced.
     * @tparam Compare Is automatically deduced.
     * @param iterable The container/sequence by reference.
     * @param compare The sequence gets sorted with nth_element. A default operator of < is used, however a custom compare can be used.
     * @return The median of the sequence.
     */
    template<class Iterable, class Compare>
    double median(Iterable& iterable, const Compare compare) {
        return median(std::begin(iterable), std::end(iterable), compare);
    }

    /**
     * Gets the median of a sequence.
     * @tparam Iterator Is automatically deduced.
     * @param begin The beginning of the sequence
     * @param end The ending of the sequence
     * @return The median of the sequence.
     */
    template<class Iterator>
    double median(const Iterator begin, const Iterator end) {
        return median(begin, end, std::less<detail::ValueType<Iterator>>());
    }

    /**
     * Gets the median of a sequence.
     * @tparam Iterable Is automatically deduced.
     * @param iterable The container/sequence by reference.
     * @return The median of the sequence.
     */
    template<class Iterable>
    double median(Iterable& iterable) {
        return median(std::begin(iterable), std::end(iterable), std::less<detail::ValueTypeIterable<Iterable>>());
    }

    /**
     * Returns a StringSplitter iterator, that splits the string on `'\n'`.
     * @tparam SubString The string type that the `StringSplitter::value_type` must return. Must either be std::string or std::string_view.
     * @tparam String The string type. `std::string` is assumed but can be specified.
     * @param string The string to split on.
     * @return Returns a StringSplitter iterator, that splits the string on `'\n'`.
     */
    template<class SubString = std::string, class String = std::string>
    StringSplitter<SubString, String> lines(String&& string) {
        return split<SubString, String>(string, "\n");
    }

    /**
     * The exact opposite of `lines`. It joins a container of `std::string` or `std::string_view` container with `'\n'` as delimiter.
     * @tparam Strings Is automatically deduced, but must be a container of `std::string` or `std::string_view`
     * @param strings The container of `std::string` or `std::string_view`.
     * @return A Join iterator that joins the strings in the container on `'\n'`.
     */
    template<class Strings, class Iterator = detail::IterType<Strings>>
    auto unlines(Strings&& strings) -> Join<Iterator> {
        static_assert(std::is_same<std::string, detail::ValueType<Iterator>>::value
#ifndef CXX_LT_17
            || std::is_same<std::string_view, detail::ValueType<Iterator>>::value
#endif
            , "the type of the container should be std::string or std::string_view");
        return join(strings, "\n");
    }

    /**
     * For every element in the sequence, perform the function `binaryOp(init, *iterator)` where init is the initial value. For example:
     * to sum all string sizes in a container, use:
     * ```cpp
     * std::vector<std::string> s = {"hello", "world", "!"};
     * size_t totalSize = lz::transaccumulate(s.begin(), s.end(), 0, [](size_t i, const std::string& rhs) {
     *      return i + rhs.size();
     * }); // totalSize = 11
     * ```
     * @tparam Iterator Is automatically deduced.
     * @tparam Init Is automatically deduced.
     * @tparam SelectorFunc Is automatically deduced.
     * @param begin The beginning of the sequence
     * @param end The ending of the sequence
     * @param init The starting value.
     * @param selectorFunc Function that specifies what to add to `init`.
     * @return The result of the transfold operation.
     */
    template<class Iterator, class Init, class SelectorFunc>
    Init transAccumulate(Iterator begin, const Iterator end, Init init, const SelectorFunc selectorFunc) {
        for (; begin != end; ++begin) {
            init = selectorFunc(std::move(init), *begin);
        }
        return init;
    }

    /**
     * For every element in the sequence, perform the function `binaryOp(init, *iterator)` where init is the initial value. For example:
     * to sum all string sizes in a container, use:
     * ```cpp
     * std::vector<std::string> s = {"hello", "world", "!"};
     * size_t totalSize = lz::transaccumulate(s, 0, [](size_t i, const std::string& rhs) {
     *      return i + rhs.size();
     * }); // totalSize = 11
     * ```
     * @tparam Iterable Is automatically deduced.
     * @tparam Init Is automatically deduced.
     * @tparam SelectorFunc Is automatically deduced.
     * @param it The container to iterate over.
     * @param init The starting value.
     * @param selectorFunc Function that specifies what to add to `init`.
     * @return The result of the transfold operation.
     */
    template<class Iterable, class Init, class SelectorFunc>
    Init transAccumulate(const Iterable& it, Init init, const SelectorFunc selectorFunc) {
        return transAccumulate(std::begin(it), std::end(it), std::move(init), selectorFunc);
    }

    /**
     * Returns an iterator that accesses two adjacent elements of one container in a std::tuple<T, T> like fashion.
     * @tparam Iterator Is automatically deduced.
     * @param begin The beginning of the sequence.
     * @param end The ending of the sequence.
     * @return A zip iterator that accesses two adjacent elements of one container.
     */
    template<class Iterator>
    auto pairwise(const Iterator begin, const Iterator end) -> Zip<Iterator, Iterator> {
        Iterator next = begin;
        if (begin != end) {
            next = std::next(begin);
        }
        return zipRange(std::make_tuple(begin, next), std::make_tuple(end, end));
    }

    /**
     * Returns an iterator that accesses two adjacent elements of one container in a std::tuple<T, T> like fashion.
     * @tparam Iterable Is automatically deduced.
     * @param iterable A container/iterable.
     * @return A zip iterator that accesses two adjacent elements of one container.
     */
    template<class Iterable, class Iterator = detail::IterType<Iterable>>
    auto pairwise(Iterable&& iterable) -> Zip<Iterator, Iterator> {
        return pairwise(std::begin(iterable), std::end(iterable));
    }

    /**
     * Returns an iterator that constructs type T from the given container. E.g. `lz::as<floats>(...begin(), ...end())` constructs
     * floating points from the given values in the container.
     * @tparam T The type to construct from the elements in the given container
     * @tparam Iterator Is automatically deduced.
     * @param begin The beginning of the sequence.
     * @param end The ending of the sequence.
     * @return A map iterator that constructs T from each of the elements in the given container.
     */
    template<class T, class Iterator, class ValueType = detail::ValueType<Iterator>>
    auto as(const Iterator begin, const Iterator end) -> Map<Iterator, std::function<T(ValueType)>> {
        static_assert(std::is_convertible<ValueType, T>::value, "the value type of the container is not convertible to T");
        return mapRange(begin, end, static_cast<std::function<T(ValueType)>>([](const ValueType& v) {
            return static_cast<T>(v);
        }));
    }

    /**
     * Returns an iterator that constructs type T from the given container. E.g. `lz::as<floats>(container)` constructs
     * floating points from the given values in the container.
     * @tparam T The type to construct from the elements in the given container
     * @tparam Iterable A container/iterable.
     * @return A map iterator that constructs T from each of the elements in the given container.
     */
    template<class T, class Iterable, class Iterator = detail::IterType<Iterable>>
    auto as(Iterable&& iterable) -> Map<Iterator, std::function<T(typename detail::ValueType<Iterator>)>> {
        return as<T>(std::begin(iterable), std::end(iterable));
    }

    /**
     * Checks if `toFind` is in the sequence (begin, end]. If so, it returns `toFind`, otherwise it returns `defaultValue`.
     * @tparam Iterator Is automatically deduced.
     * @tparam T Is automatically deduced.
     * @param begin The beginning of the sequence.
     * @param end The ending of the sequence.
     * @param toFind The value to find. One can use `std::move(defaultValue)` to avoid copies.
     * @param defaultValue The value to return if `toFind` is not found.  One can use `std::move(toFind)` to avoid copies.
     * @return Either `toFind` or `defaultValue`.
     */
    template<class Iterator, class T, class ValueType = detail::ValueType<Iterator>>
    ValueType findOrDefault(const Iterator begin, const Iterator end, ValueType&& toFind, T&& defaultValue) {
        return std::find(begin, end, toFind) == end ? defaultValue : toFind;
    }

    /**
     * Checks if `toFind` is in the sequence (begin, end]. If so, it returns `toFind`, otherwise it returns `defaultValue`.
     * @tparam Iterable Is automatically deduced.
     * @tparam T Is automatically deduced.
     * @param iterable The iterable to search.
     * @param toFind The value to find. One can use `std::move(defaultValue)` to avoid copies.
     * @param defaultValue The value to return if `toFind` is not found.  One can use `std::move(toFind)` to avoid copies.
     * @return Either `toFind` or `defaultValue`.
     */
    template<class Iterable, class T, class ValueType = detail::ValueTypeIterable<Iterable>>
    ValueType findOrDefault(const Iterable& iterable, ValueType&& toFind, T&& defaultValue) {
        return findOrDefault(std::begin(iterable), std::end(iterable), toFind, defaultValue);
    }

    /**
     * Uses `std::find_if(begin, end, predicate)` to check if for one of the elements, `predicate` returns true. If so, its corresponding
     * value is returned. If not, `defaultValue` is returned.
     * @tparam Iterator Is automatically deduced.
     * @tparam T Is automatically deduced.
     * @tparam UnaryPredicate Is automatically deduced.
     * @param begin The beginning of the sequence.
     * @param end The ending of the sequence.
     * @param predicate A function with one argument that returns a bool for its corresponding value type of the iterator.
     * (see `std::find_if`)
     * @param defaultValue The default value to return if predicate is `false` for all elements. Use `std::move` to avoid copies.
     * @return Either the element that has been found by `predicate` or `defaultValue` if no such item exists.
     */
    template<class Iterator, class T, class UnaryPredicate, class ValueType = detail::ValueType<Iterator>>
    ValueType findOrDefaultIf(const Iterator begin, const Iterator end, const UnaryPredicate predicate, T&& defaultValue) {
        const Iterator pos = std::find_if(begin, end, predicate);
        return pos == end ? defaultValue : *pos;
    }

    /**
     * Uses `std::find_if(begin, end, predicate)` to check if for one of the elements, `predicate` returns true. If so, its corresponding
     * value is returned. If not, `defaultValue` is returned.
     * @tparam Iterable Is automatically deduced.
     * @tparam T Is automatically deduced.
     * @tparam UnaryPredicate Is automatically deduced.
     * @param iterable The sequence to search.
     * @param predicate A function with one argument that returns a bool for its corresponding value type of the iterator.
     * (see `std::find_if`)
     * @param defaultValue The default value to return if predicate is `false` for all elements. Use `std::move` to avoid copies.
     * @return Either the element that has been found by `predicate` or `defaultValue` if no such item exists.
     */
    template<class Iterable, class T, class UnaryPredicate, class ValueType = detail::ValueTypeIterable<Iterable>>
    ValueType findOrDefaultIf(const Iterable& iterable, const UnaryPredicate predicate, T&& defaultValue) {
        return findOrDefaultIf(std::begin(iterable), std::end(iterable), predicate, defaultValue);
    }

    /**
     * This value is returned when indexOf does not find the value specified.
     */
    constexpr LZ_INLINE_VAR size_t npos = std::numeric_limits<size_t>::max();

    /**
     * Searches (begin, end] for val, and returns its corresponding index, or lz::npos if no such value exists.
     * @tparam Iterator Is automatically deduced.
     * @tparam T Is automatically deduced.
     * @param begin The beginning of the sequence.
     * @param end The ending of the sequence.
     * @param val The value to search.
     * @return The index of `val` or lz::npos of no such value exists.
     */
    template<class Iterator, class T>
    size_t indexOf(const Iterator begin, const Iterator end, const T& val) {
        const Iterator pos = std::find(begin, end, val);
        return pos == end ? npos : static_cast<size_t>(std::distance(begin, pos));
    }

    /**
    * Searches `iterable` for val, and returns its corresponding index, or lz::npos if no such value exists.
    * @tparam Iterable Is automatically deduced.
    * @tparam T Is automatically deduced.
    * @param iterable The iterable to search.
    * @param val The value to search.
    * @return The index of `val` or lz::npos of no such value exists.
    */
    template<class Iterable, class T>
    size_t indexOf(const Iterable& iterable, const T& val) {
        return indexOf(std::begin(iterable), std::end(iterable), val);
    }

    /**
    * Searches (begin, end] with unary predicate `predicate`, and returns its corresponding index, or lz::npos if no such value exists.
    * @tparam Iterator Is automatically deduced.
    * @tparam UnaryFunc Is automatically deduced.
    * @param begin The beginning of the sequence.
    * @param end The ending of the sequence.
    * @param predicate The search predicate. Uses `std::find_if`.
    * @return The index of the predicate where it returns `true` or lz::npos of no such predicate is present.
    */
    template<class Iterator, class UnaryFunc>
    size_t indexOfIf(const Iterator begin, const Iterator end, const UnaryFunc predicate) {
        const Iterator pos = std::find_if(begin, end, predicate);
        return pos == end ? npos : static_cast<size_t>(std::distance(begin, pos));
    }

    /**
    * Searches iterable with unary predicate `predicate`, and returns its corresponding index, or lz::npos if no such value exists.
    * @tparam Iterator Is automatically deduced.
    * @tparam UnaryFunc Is automatically deduced.
    * @param iterable The sequence to search.
    * @param predicate The search predicate. Uses `std::find_if`.
    * @return The index of the predicate where it returns `true` or lz::npos of no such predicate is present.
    */
    template<class Iterable, class UnaryFunc>
    size_t indexOfIf(const Iterable& iterable, const UnaryFunc predicate) {
        return indexOfIf(std::begin(iterable), std::end(iterable), predicate);
    }

    /**
     * Creates a map object with filter iterator that, if the filter function returns true, the map function is executed.
     * @tparam Iterator Is automatically deduced.
     * @tparam UnaryMapFunc Is automatically deduced.
     * @tparam UnaryFilterFunc Is automatically deduced.
     * @param begin The beginning of the sequence.
     * @param end The ending of the sequence.
     * @param filterFunc The function that filters the elements. If this function returns `true`, its corresponding container value is
     * passed to the `mapFunc`.
     * @param mapFunc The function that returns the (new) type.
     * @return A map object that can be iterated over. The `value_type` of the this view object is equal to the return value of `mapFunc`.
     */
    template<class UnaryFilterFunc, class UnaryMapFunc, class Iterator>
    Map<detail::FilterIterator<Iterator, UnaryFilterFunc>, UnaryMapFunc>
    filterMap(const Iterator begin, const Iterator end, const UnaryFilterFunc& filterFunc, const UnaryMapFunc& mapFunc) {
        Filter<Iterator, UnaryFilterFunc> filterView = filterRange(begin, end, filterFunc);
        return map(filterView, mapFunc);
    }

    /**
     * Creates a map object with filter iterator that, if the filter function returns true, the map function is executed.
     * @tparam Iterable Is automatically deduced.
     * @tparam UnaryMapFunc Is automatically deduced.
     * @tparam UnaryFilterFunc Is automatically deduced.
     * @param iterable The iterable to filter/map.
     * @param filterFunc The function that filters the elements. If this function returns `true`, its corresponding container value is
     * passed to the `mapFunc`.
     * @param mapFunc The function that returns the (new) type.
     * @return A map object that can be iterated over. The `value_type` of the this view object is equal to the return value of `mapFunc`.
     */
    template<class UnaryFilterFunc, class UnaryMapFunc, class Iterable>
    auto filterMap(Iterable&& iterable, const UnaryFilterFunc& filterFunc, const UnaryMapFunc& mapFunc) ->
    Map<detail::FilterIterator<decltype(std::begin(iterable)), UnaryFilterFunc>, UnaryMapFunc> {
        return filterMap(std::begin(iterable), std::end(iterable), filterFunc, mapFunc);
    }
}

#endif