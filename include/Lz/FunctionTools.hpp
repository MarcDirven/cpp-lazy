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
#include "Take.hpp"

#ifdef LZ_HAS_CXX17
#define LZ_INLINE_VAR inline
#else // ^^^ inline var vvv !inline var
#define LZ_INLINE_VAR
#endif // lz has cxx17

#include <cassert>

namespace lz {
	// ReSharper disable once CppUnnamedNamespaceInHeaderFile
	namespace detail { namespace {
        template<class Iterator>
        using DifferenceType = typename std::iterator_traits<Iterator>::difference_type;

        // ReSharper disable once CppNonInlineFunctionDefinitionInHeaderFile
        bool stringReplaceImpl(std::string& string, const std::string& oldString, const std::string& newString, const bool replaceAll) {
            const std::size_t oldStringSize = oldString.length();
            const std::size_t newStringSize = newString.length();

            if (oldStringSize == 0 || newStringSize == 0) {
                return false;
            }

            std::size_t startPos = string.find(oldString);
            if (startPos == std::string::npos) {
                return false;
            }

            string.replace(startPos, oldStringSize, newString);
            if (!replaceAll) {
                return true;
            }

            startPos = string.find(oldString, newStringSize + startPos);
            while (startPos != std::string::npos) {
                string.replace(startPos, oldStringSize, newString);
                startPos = string.find(oldString, newStringSize + startPos);
            }
            return true;
        }

        template<class To>
        struct ConvertFn {
            template<class From>
            To operator()(const From& f) const {
                return static_cast<To>(f);
            }
        };
    }} // namespace detail::<anonymous>

    /**
     * This value is returned when indexOf(If) does not find the value specified.
     */
    constexpr LZ_INLINE_VAR std::size_t npos = std::numeric_limits<size_t>::max();

    /**
     * Gets the mean of a sequence.
     * @tparam Iterator Is automatically deduced.
     * @param begin The beginning of the sequence.
     * @param end The ending of the sequence.
     * @return The mean of the sequence.
     */
    template<LZ_CONCEPT_ITERATOR Iterator>
    double mean(const Iterator begin, const Iterator end) {
        const detail::DifferenceType<Iterator> distance = std::distance(begin, end);
        const detail::ValueTypeIterator<Iterator> sum = std::accumulate(begin, end, detail::ValueTypeIterator<Iterator>(0));
        return static_cast<double>(sum) / distance;
    }

    /**
     * Gets the mean of a sequence.
     * @tparam Iterable Is automatically deduced.
     * @param container The container to calculate the mean of.
     * @return The mean of the container.
     */
    template<LZ_CONCEPT_ITERABLE Iterable>
    double mean(const Iterable& container) {
        return mean(std::begin(container), std::end(container));
    }

    /**
      * Returns a StringSplitter iterator, that splits the string on `'\n'`.
      * @tparam SubString The string type that the `StringSplitter::value_type` must return. Must either be std::string or std::string_view.
      * @tparam String The string type. `std::string` is assumed but can be specified.
      * @param string The string to split on.
      * @return Returns a StringSplitter iterator, that splits the string on `'\n'`.
      */
#ifdef LZ_HAS_STRING_VIEW
    template<class SubString = std::string_view, class String = std::string>
#else // ^^^ Lz has string view vvv !lz has string view
    template<class SubString = std::string, class String = std::string>
#endif
    StringSplitter<SubString, String> lines(String&& string) {
        return split<SubString, String>(string, "\n");
    }

    /**
     * Sums all the values from [from, upToAndIncluding]
     * @tparam T An integral value.
     * @param from The start to sum from
     * @param upToAndIncluding The end of the sum
     * @return The result of the sum from [from, upToAndIncluding]
     */
    template<class T>
    T sumTo(const T from, const T upToAndIncluding) {
        static_assert(std::is_integral<T>::value, "To must be integral type");

        const T fromAbs = std::abs(from);
        const T toAbs = std::abs(upToAndIncluding);
        const T error = ((fromAbs - 1) * (fromAbs)) / 2;
        const T sum = (toAbs * (toAbs + 1)) / 2;

        if (from < 0) {
            if (upToAndIncluding < 0) {
                return error - sum;
            }
            return -(error + fromAbs - sum);
        }
        if (upToAndIncluding < 0) {
            return error - sum;
        }
        assert(from < upToAndIncluding && "'from' cannot be smaller than 'upToAndIncluding' if both are positive");
        return sum - error;
    }

    /**
     * Sums all the values from [0, upToAndIncluding]
     * @tparam T An integral value.
     * @param upToAndIncluding The end of the sum
     * @return The result of the sum from [0, upToAndIncluding]
     */
    template<class T>
    T sumTo(const T upToAndIncluding) {
        return sumTo(0, upToAndIncluding);
    }

    /**
     * The exact opposite of `lines`. It joins a container of `std::string` or `std::string_view` container with `'\n'` as delimiter.
     * @tparam Strings Is automatically deduced, but must be a container of `std::string` or `std::string_view`
     * @param strings The container of `std::string` or `std::string_view`.
     * @return A Join iterator that joins the strings in the container on `'\n'`.
     */
    template<class Strings, class Iterator = detail::IterType<Strings>>
    Join<Iterator> unlines(Strings&& strings) {
        static_assert(std::is_same<std::string, detail::ValueTypeIterator<Iterator>>::value
#ifdef LZ_HAS_STRING_VIEW
            || std::is_same<std::string_view, detail::ValueTypeIterator<Iterator>>::value
#endif // Lz has string view
            , "the type of the container should be std::string or std::string_view");
        return join(strings, "\n");
    }

#ifndef LZ_HAS_CXX17
    /**
     * This function is defined when C++ version is lower than 17.
     * For every element in the sequence, perform the function `binaryOp(init, *iterator)` where init is the initial value. For example:
     * to sum all string sizes in a container, use:
     * ```cpp
     * std::vector<std::string> s = {"hello", "world", "!"};
     * size_t totalSize = lz::transAccumulate(s.begin(), s.end(), 0, [](size_t i, const std::string& rhs) {
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
     * @return The result of the transAccumulate operation.
     */
    template<LZ_CONCEPT_ITERATOR Iterator, class Init, class SelectorFunc>
    Init transAccumulate(Iterator begin, const Iterator end, Init init, const SelectorFunc selectorFunc) {
        for (; begin != end; ++begin) {
            init = selectorFunc(std::move(init), *begin);
        }
        return init;
    }

    /**
     * This function is defined when C++ version is lower than 17.
     * For every element in the sequence, perform the function `binaryOp(init, *iterator)` where init is the initial value. For example:
     * to sum all string sizes in a container, use:
     * ```cpp
     * std::vector<std::string> s = {"hello", "world", "!"};
     * size_t totalSize = lz::transAccumulate(s, 0, [](size_t i, const std::string& rhs) {
     *      return i + rhs.size();
     * }); // totalSize = 11
     * ```
     * @tparam Iterable Is automatically deduced.
     * @tparam Init Is automatically deduced.
     * @tparam SelectorFunc Is automatically deduced.
     * @param it The container to iterate over.
     * @param init The starting value.
     * @param selectorFunc Function that specifies what to add to `init`.
     * @return The result of the transAccumulate operation.
     */
    template<LZ_CONCEPT_ITERABLE Iterable, class Init, class SelectorFunc>
    Init transAccumulate(const Iterable& it, Init init, const SelectorFunc selectorFunc) {
        return transAccumulate(std::begin(it), std::end(it), std::move(init), selectorFunc);
    }
#endif // End LZ_HAS_CXX17

    /**
     * Returns an iterator that accesses two adjacent elements of one container in a std::tuple<To, To> like fashion.
     * @tparam Iterator Is automatically deduced.
     * @param begin The beginning of the sequence.
     * @param end The ending of the sequence.
     * @return A zip iterator that accesses two adjacent elements of one container.
     */
    template<LZ_CONCEPT_ITERATOR Iterator>
    Zip<Iterator, Iterator> pairwise(const Iterator begin, const Iterator end) {
        Iterator next = begin;
        if (begin != end) {
            next = std::next(begin);
        }
        return zipRange(std::make_tuple(begin, next), std::make_tuple(end, end));
    }

    /**
     * Returns an iterator that accesses two adjacent elements of one container in a std::tuple<To, To> like fashion.
     * @tparam Iterable Is automatically deduced.
     * @param iterable A container/iterable.
     * @return A zip iterator that accesses two adjacent elements of one container.
     */
    template<LZ_CONCEPT_ITERABLE Iterable, class Iterator = detail::IterType<Iterable>>
    auto pairwise(Iterable&& iterable) -> Zip<Iterator, Iterator> {
        return pairwise(std::begin(iterable), std::end(iterable));
    }

    /**
     * Returns a view object of which its iterators are reversed.
     * @tparam Iterator Is automatically deduced.
     * @param begin The beginning of the sequence. Must have at least std::bidirectional_iterator_tag.
     * @param end The ending of the sequence. Must have at least std::bidirectional_iterator_tag.
     * @return A Take view object contains the reverse order of [begin end)
     */
    template<LZ_CONCEPT_BIDIRECTIONAL_ITERATOR Iterator>
    Take<std::reverse_iterator<Iterator>> reverse(const Iterator begin, const Iterator end) {
#ifndef LZ_HAS_CONCEPTS
        using IterCat = typename std::iterator_traits<Iterator>::iterator_category;
        static_assert(std::is_same<IterCat, std::bidirectional_iterator_tag>::value ||
            std::is_same<IterCat, std::random_access_iterator_tag>::value,
            "the type of the iterator must be bidirectional or stronger");
#endif // !Lz has concepts
        using ReverseIterator = std::reverse_iterator<Iterator>;
        return takeRange(ReverseIterator(end), ReverseIterator(begin));
    }
    /**
     * Returns a view object of which its iterators are reversed.
     * @tparam Iterable Is automatically deduced.
     * @param iterable The iterable. The iterable must have at least std::bidirectional_iterator_tag.
     * @return A Take view object contains the reverse order of [begin end)
     */
    template<LZ_CONCEPT_BIDIRECTIONAL_ITERABLE Iterable>
    Take<std::reverse_iterator<detail::IterType<Iterable>>> reverse(Iterable&& iterable) {
        return lz::reverse(std::begin(iterable), std::end(iterable)); // ADL std::reverse
    }

    /**
     * Returns an iterator that constructs type To from the given container. E.g. `lz::as<floats>(...begin(), ...end())` constructs
     * floating points from the given values in the container.
     * @tparam T The type to construct from the elements in the given container
     * @tparam Iterator Is automatically deduced.
     * @param begin The beginning of the sequence.
     * @param end The ending of the sequence.
     * @return A map iterator that constructs To from each of the elements in the given container.
     */
    template<class T, LZ_CONCEPT_ITERATOR Iterator>
    Map<Iterator, detail::ConvertFn<T>> as(const Iterator begin, const Iterator end) {
        using ValueTypeIterator = detail::ValueTypeIterator<Iterator>;
        static_assert(std::is_convertible<ValueTypeIterator, T>::value, "the value type of the container is not convertible to To");
        return mapRange(begin, end, detail::ConvertFn<T>());
    }

    /**
     * Returns an iterator that constructs type To from the given container. E.g. `lz::as<floats>(container)` constructs
     * floating points from the given values in the container.
     * @tparam T The type to construct from the elements in the given container
     * @tparam Iterable A container/iterable.
     * @return A map iterator that constructs To from each of the elements in the given container.
     */
    template<class T, LZ_CONCEPT_ITERABLE Iterable, class Iterator = detail::IterType<Iterable>>
    Map<Iterator, detail::ConvertFn<T>> as(Iterable&& iterable) {
        return as<T>(std::begin(iterable), std::end(iterable));
    }

    /**
     * Replaces one occurrence of `oldString` in `string` and replaces it with `newString`, and returns whether there was any newString
     * at all.
     * @param string The string to modify.
     * @param oldString The string to replace.
     * @param newString The new string.
     * @return `true` if replacing has taken place, `false` otherwise.
     */
     // ReSharper disable once CppNonInlineFunctionDefinitionInHeaderFile
    bool strReplace(std::string& string, const std::string& oldString, const std::string& newString) {
        return detail::stringReplaceImpl(string, oldString, newString, false);
    }

    /**
     * Replaces all occurrences of `oldString` in `string` to `newString`.
     * @param string The string to modify.
     * @param oldString The string to replace.
     * @param newString The new string.
     * @return `true` if replacing has taken place, `false` otherwise.
     */
     // ReSharper disable once CppNonInlineFunctionDefinitionInHeaderFile
    bool strReplaceAll(std::string& string, const std::string& oldString, const std::string& newString) {
        return detail::stringReplaceImpl(string, oldString, newString, true);
    }

#ifdef LZ_HAS_EXECUTION
    /**
     * Creates a map object with filter iterator that, if the filter function returns true, the map function is executed.
     * @tparam Iterator Is automatically deduced.
     * @tparam UnaryMapFunc Is automatically deduced.
     * @tparam UnaryFilterFunc Is automatically deduced.
     * @tparam Execution Is automatically deduced.
     * @param begin The beginning of the sequence.
     * @param end The ending of the sequence.
     * @param filterFunc The function that filters the elements. If this function returns `true`, its corresponding container value is
     * passed to the `mapFunc`.
     * @param mapFunc The function that returns the (new) type.
     * @param execPolicy TThe execution policy. Must be one of `std::execution`'s tags. Performs the find using this execution.
     * @return A map object that can be iterated over. The `value_type` of the this view object is equal to the return value of `mapFunc`.
     */
    template<class Execution = std::execution::sequenced_policy, class UnaryFilterFunc, class UnaryMapFunc, LZ_CONCEPT_ITERATOR Iterator>
    Map<detail::FilterIterator<Execution, Iterator, UnaryFilterFunc>, UnaryMapFunc>
        filterMap(const Iterator begin, const Iterator end, const UnaryFilterFunc& filterFunc, const UnaryMapFunc& mapFunc,
            const Execution execPolicy = std::execution::seq) {
        static_assert(std::is_execution_policy_v<Execution>, "Execution must be of type std::execution::...");

        Filter<Execution, Iterator, UnaryFilterFunc> filterView = filterRange(begin, end, filterFunc, execPolicy);
        return map(filterView, mapFunc);
    }

    /**
     * Creates a map object with filter iterator that, if the filter function returns true, the map function is executed.
     * @tparam Execution Is automatically deduced.
     * @tparam Iterable Is automatically deduced.
     * @tparam UnaryMapFunc Is automatically deduced.
     * @tparam UnaryFilterFunc Is automatically deduced.
     * @param iterable The iterable to filter/map.
     * @param filterFunc The function that filters the elements. If this function returns `true`, its corresponding container value is
     * passed to the `mapFunc`.
     * @param mapFunc The function that returns the (new) type.
     * @param execution TThe execution policy. Must be one of `std::execution`'s tags. Performs the find using this execution.
     * @return A map object that can be iterated over. The `value_type` of the this view object is equal to the return value of `mapFunc`.
     */
    template<class Execution = std::execution::sequenced_policy, class UnaryFilterFunc, class UnaryMapFunc, LZ_CONCEPT_ITERABLE Iterable>
    Map<detail::FilterIterator<Execution, detail::IterType<Iterable>, UnaryFilterFunc>, UnaryMapFunc>
        filterMap(Iterable&& iterable, const UnaryFilterFunc& filterFunc, const UnaryMapFunc& mapFunc,
            const Execution execution = std::execution::seq) {
        return filterMap(std::begin(iterable), std::end(iterable), filterFunc, mapFunc, execution);
    }

    /**
     * Gets the median of a sequence.
     * @tparam Iterator Is automatically deduced.
     * @tparam Compare Is automatically deduced.
     * @tparam Execution Is automatically deduced.
     * @param begin The beginning of the sequence
     * @param end The ending of the sequence
     * @param compare The sequence gets sorted with nth_element. A default operator of < is used, however a custom compare can be used.
     * @param execution Uses the execution to perform the nth_element algorithm + the std::max element if the length of the sequence is
     * even.
     * @return The median of the sequence.
     */
    template<class Execution = std::execution::sequenced_policy, LZ_CONCEPT_ITERATOR Iterator, class Compare>
    double median(const Iterator begin, const Iterator end, const Compare compare, const Execution execution = std::execution::seq) {
        static_assert(std::is_execution_policy_v<Execution>, "Execution must be of type std::execution::...");

        const detail::DifferenceType<Iterator> len = std::distance(begin, end);
        if (len == 0) {
            throw std::invalid_argument(LZ_FILE_LINE ": the length of the sequence cannot be 0");
        }

        const detail::DifferenceType<Iterator> mid = len >> 1;
        const Iterator midIter = std::next(begin, mid);
        std::nth_element(execution, begin, midIter, end, compare);

        if (detail::isEven(len)) {
            const Iterator leftHalf = std::max_element(execution, begin, midIter);
            return (static_cast<double>(*leftHalf) + *midIter) / 2.;
        }
        return *midIter;
    }

    /**
     * Gets the median of a sequence.
     * @tparam Execution Is automatically deduced.
     * @tparam Iterable Is automatically deduced.
     * @tparam Compare Is automatically deduced.
     * @param iterable The container/sequence by reference.
     * @param compare The sequence gets sorted with nth_element. A default operator of < is used, however a custom compare can be used.
     * @param execution Uses the execution to perform the nth_element algorithm + the std::max element if the length of the sequence is
     * even.
     * @return The median of the sequence.
     */
    template<class Execution = std::execution::sequenced_policy, LZ_CONCEPT_ITERABLE Iterable, class Compare>
    double median(Iterable& iterable, const Compare compare, const Execution execution = std::execution::seq) {
        return median(std::begin(iterable), std::end(iterable), compare, execution);
    }

    /**
     * Gets the median of a sequence.
     * @tparam Execution Is automatically deduced.
     * @tparam Iterator Is automatically deduced.
     * @param begin The beginning of the sequence
     * @param end The ending of the sequence
     * @param execution Uses the execution to perform the nth_element algorithm + the std::max element if the length of the sequence is
     * even.
     * @return The median of the sequence.
     */
    template<class Execution = std::execution::sequenced_policy, LZ_CONCEPT_ITERATOR Iterator>
    double median(const Iterator begin, const Iterator end, const Execution execution = std::execution::seq) {
        return median(begin, end, std::less<detail::ValueTypeIterator<Iterator>>(), execution);
    }

    /**
     * Gets the median of a sequence.
     * @tparam Execution Is automatically deduced.
     * @tparam Iterable Is automatically deduced.
     * @param iterable The container/sequence by reference.
     * @param execution Uses the execution to perform the nth_element algorithm + the std::max element if the length of the sequence is
     * even.
     * @return The median of the sequence.
     */
    template<class Execution = std::execution::sequenced_policy, LZ_CONCEPT_ITERABLE Iterable>
    double median(Iterable& iterable, const Execution execution = std::execution::seq) {
        return median(std::begin(iterable), std::end(iterable), std::less<detail::ValueTypeIterable<Iterable>>(), execution);
    }

    /**
     * Checks if `toFind` is in the sequence [begin, end). If so, it returns `toFind`, otherwise it returns `defaultValue`.
     * @tparam Execution Is automatically deduced.
     * @tparam Iterator Is automatically deduced.
     * @tparam To Is automatically deduced.
     * @param begin The beginning of the sequence.
     * @param end The ending of the sequence.
     * @param toFind The value to find. One can use `std::move(defaultValue)` to avoid copies.
     * @param defaultValue The value to return if `toFind` is not found.
     * @param execution Uses the execution to perform the find.
     * @return Either `toFind` or `defaultValue`.
     */
    template<class Execution = std::execution::sequenced_policy, LZ_CONCEPT_ITERATOR Iterator, class To,
        class ValueType = detail::ValueTypeIterator<Iterator>>
        ValueType findOrDefault(const Iterator begin, const Iterator end, ValueType&& toFind, To&& defaultValue,
            const Execution execution = std::execution::seq) {
        static_assert(std::is_execution_policy_v<Execution>, "Execution must be of type std::execution::...");

        return std::find(execution, begin, end, toFind) == end ? defaultValue : toFind;
    }

    /**
     * Checks if `toFind` is in the sequence [begin, end). If so, it returns `toFind`, otherwise it returns `defaultValue`.
     * @tparam Execution Is automatically deduced.
     * @tparam Iterable Is automatically deduced.
     * @tparam To Is automatically deduced.
     * @param iterable The iterable to search.
     * @param toFind The value to find. One can use `std::move(defaultValue)` to avoid copies.
     * @param defaultValue The value to return if `toFind` is not found.
     * @param execution Uses the execution to perform the find.
     * @return Either `toFind` or `defaultValue`.
     */
    template<class Execution = std::execution::sequenced_policy, LZ_CONCEPT_ITERABLE Iterable, class To,
        class ValueType = detail::ValueTypeIterable<Iterable>>
        ValueType findOrDefault(const Iterable& iterable, ValueType&& toFind, To&& defaultValue,
            const Execution execution = std::execution::seq) {
        return findOrDefault(std::begin(iterable), std::end(iterable), toFind, defaultValue, execution);
    }

    /**
     * Uses `std::find_if(begin, end, predicate)` to check if for one of the elements, `predicate` returns true. If so, its corresponding
     * value is returned. If not, `defaultValue` is returned.
     * @tparam Execution Is automatically deduced.
     * @tparam Iterator Is automatically deduced.
     * @tparam To Is automatically deduced.
     * @tparam UnaryPredicate Is automatically deduced.
     * @param begin The beginning of the sequence.
     * @param end The ending of the sequence.
     * @param predicate A function with one argument that returns a bool for its corresponding value type of the iterator.
     * (see `std::find_if`)
     * @param defaultValue The default value to return if predicate is `false` for all elements.
     * @param execution Uses the execution to perform the find.
     * @return Either the element that has been found by `predicate` or `defaultValue` if no such item exists.
     */
    template<class Execution = std::execution::sequenced_policy, LZ_CONCEPT_ITERATOR Iterator, class To, class UnaryPredicate,
        class ValueType = detail::ValueTypeIterator<Iterator>>
        ValueType findOrDefaultIf(const Iterator begin, const Iterator end, const UnaryPredicate predicate, To&& defaultValue,
            const Execution execution) {
        static_assert(std::is_execution_policy_v<Execution>, "Execution must be of type std::execution::...");

        const Iterator pos = std::find_if(execution, begin, end, predicate);
        return pos == end ? defaultValue : *pos;
    }


    /**
     * Uses `std::find_if(begin, end, predicate)` to check if for one of the elements, `predicate` returns true. If so, its corresponding
     * value is returned. If not, `defaultValue` is returned.
     * @tparam Execution Is automatically deduced.
     * @tparam Iterable Is automatically deduced.
     * @tparam To Is automatically deduced.
     * @tparam UnaryPredicate Is automatically deduced.
     * @param iterable The sequence to search.
     * @param predicate A function with one argument that returns a bool for its corresponding value type of the iterator.
     * (see `std::find_if`)
     * @param defaultValue The default value to return if predicate is `false` for all elements.
     * @param execution Uses the execution to perform the find.
     * @return Either the element that has been found by `predicate` or `defaultValue` if no such item exists.
     */
    template<class Execution = std::execution::sequenced_policy, LZ_CONCEPT_ITERABLE Iterable, class To, class UnaryPredicate,
        class ValueType = detail::ValueTypeIterable<Iterable>>
        ValueType findOrDefaultIf(const Iterable& iterable, const UnaryPredicate predicate, To&& defaultValue,
            const Execution execution = std::execution::seq) {
        return findOrDefaultIf(std::begin(iterable), std::end(iterable), predicate, defaultValue, execution);
    }

    /**
     * Searches [begin, end) for val, and returns its corresponding index, or lz::npos if no such value exists.
     * @tparam Execution Is automatically deduced.
     * @tparam Iterator Is automatically deduced.
     * @tparam To Is automatically deduced.
     * @param begin The beginning of the sequence.
     * @param end The ending of the sequence.
     * @param val The value to search.
     * @param execution Uses the execution to perform the find.
     * @return The index of `val` or lz::npos of no such value exists.
     */
    template<class Execution = std::execution::sequenced_policy, LZ_CONCEPT_ITERATOR Iterator, class To>
    std::size_t indexOf(const Iterator begin, const Iterator end, const To& val, const Execution execution = std::execution::seq) {
        static_assert(std::is_execution_policy_v<Execution>, "Execution must be of type std::execution::...");

        const Iterator pos = std::find(execution, begin, end, val);
        return pos == end ? npos : static_cast<size_t>(std::distance(begin, pos));
    }

    /**
     * Searches `iterable` for val, and returns its corresponding index, or lz::npos if no such value exists.
     * @tparam Execution Is automatically deduced.
     * @tparam Iterable Is automatically deduced.
     * @tparam To Is automatically deduced.
     * @param iterable The iterable to search.
     * @param val The value to search.
     * @param execution Uses the execution to perform the find.
     * @return The index of `val` or lz::npos of no such value exists.
     */
    template<class Execution = std::execution::sequenced_policy, LZ_CONCEPT_ITERABLE Iterable, class To>
    std::size_t indexOf(const Iterable& iterable, const To& val, const Execution execution = std::execution::seq) {
        return indexOf(std::begin(iterable), std::end(iterable), val, execution);
    }

    /**
    * Searches [begin, end) with unary predicate `predicate`, and returns its corresponding index, or lz::npos if no such value exists.
    * @tparam Execution Is automatically deduced.
    * @tparam Iterator Is automatically deduced.
    * @tparam UnaryFunc Is automatically deduced.
    * @param begin The beginning of the sequence.
    * @param end The ending of the sequence.
    * @param predicate The search predicate. Uses `std::find_if`.
    * @param execution Uses the execution to perform the find.
    * @return The index of the predicate where it returns `true` or lz::npos of no such predicate is present.
    */
    template<class Execution = std::execution::sequenced_policy, LZ_CONCEPT_ITERATOR Iterator, class UnaryFunc>
    std::size_t indexOfIf(const Iterator begin, const Iterator end, const UnaryFunc predicate, const Execution execution = std::execution::seq) {
        static_assert(std::is_execution_policy_v<Execution>, "Execution must be of type std::execution::...");

        const Iterator pos = std::find_if(execution, begin, end, predicate);
        return pos == end ? npos : static_cast<std::size_t>(std::distance(begin, pos));
    }

    /**
    * Searches iterable with unary predicate `predicate`, and returns its corresponding index, or lz::npos if no such value exists.
    * @tparam Iterable Is automatically deduced.
    * @tparam UnaryFunc Is automatically deduced.
    * @param iterable The sequence to search.
    * @param predicate The search predicate. Uses `std::find_if`.
    * @return The index of the predicate where it returns `true` or lz::npos of no such predicate is present.
    */
    template<class Execution = std::execution::sequenced_policy, LZ_CONCEPT_ITERABLE Iterable, class UnaryFunc>
    std::size_t indexOfIf(const Iterable& iterable, const UnaryFunc predicate, const Execution execution = std::execution::seq) {
        return indexOfIf(std::begin(iterable), std::end(iterable), predicate, execution);
    }
#else // ^^^ Lz has execution vvv !Lz has execution

    /**
     * Gets the median of a sequence.
     * @tparam Iterator Is automatically deduced.
     * @tparam Compare Is automatically deduced.
     * @param begin The beginning of the sequence
     * @param end The ending of the sequence
     * @param compare The sequence gets sorted with nth_element. A default operator of < is used, however a custom compare can be used.
     * @return The median of the sequence.
     */
    template<LZ_CONCEPT_ITERATOR Iterator, class Compare>
    double median(const Iterator begin, const Iterator end, const Compare compare) {
        const detail::DifferenceType<Iterator> len = std::distance(begin, end);
        if (len == 0) {
            throw std::invalid_argument(LZ_FILE_LINE ": the length of the sequence cannot be 0");
        }

        const detail::DifferenceType<Iterator> mid = len >> 1;
        const Iterator midIter = std::next(begin, mid);
        std::nth_element(begin, midIter, end, compare);

        if (detail::isEven(len)) {
            const Iterator leftHalf = std::max_element(begin, midIter);
            return (static_cast<double>(*leftHalf) + *midIter) / 2.;
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
    template<LZ_CONCEPT_ITERABLE Iterable, class Compare>
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
    template<LZ_CONCEPT_ITERATOR Iterator>
    double median(const Iterator begin, const Iterator end) {
        return median(begin, end, std::less<detail::ValueTypeIterator<Iterator>>());
    }

    /**
     * Gets the median of a sequence.
     * @tparam Iterable Is automatically deduced.
     * @param iterable The container/sequence by reference.
     * @return The median of the sequence.
     */
    template<LZ_CONCEPT_ITERABLE Iterable>
    double median(Iterable& iterable) {
        return median(std::begin(iterable), std::end(iterable), std::less<detail::ValueTypeIterable<Iterable>>());
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
    template<LZ_CONCEPT_ITERATOR Iterator, class T, class ValueType = detail::ValueTypeIterator<Iterator>>
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
    template<LZ_CONCEPT_ITERABLE Iterable, class T, class ValueType = detail::ValueTypeIterable<Iterable>>
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
    template<LZ_CONCEPT_ITERATOR Iterator, class T, class UnaryPredicate, class ValueType = detail::ValueTypeIterator<Iterator>>
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
    template<LZ_CONCEPT_ITERABLE Iterable, class T, class UnaryPredicate, class ValueType = detail::ValueTypeIterable<Iterable>>
    ValueType findOrDefaultIf(const Iterable& iterable, const UnaryPredicate predicate, T&& defaultValue) {
        return findOrDefaultIf(std::begin(iterable), std::end(iterable), predicate, defaultValue);
    }

    /**
     * Searches (begin, end] for val, and returns its corresponding index, or lz::npos if no such value exists.
     * @tparam Iterator Is automatically deduced.
     * @tparam T Is automatically deduced.
     * @param begin The beginning of the sequence.
     * @param end The ending of the sequence.
     * @param val The value to search.
     * @return The index of `val` or lz::npos of no such value exists.
     */
    template<LZ_CONCEPT_ITERATOR Iterator, class T>
    std::size_t indexOf(const Iterator begin, const Iterator end, const T& val) {
        const Iterator pos = std::find(begin, end, val);
        return pos == end ? npos : static_cast<std::size_t>(std::distance(begin, pos));
    }

    /**
    * Searches `iterable` for val, and returns its corresponding index, or lz::npos if no such value exists.
    * @tparam Iterable Is automatically deduced.
    * @tparam T Is automatically deduced.
    * @param iterable The iterable to search.
    * @param val The value to search.
    * @return The index of `val` or lz::npos of no such value exists.
    */
    template<LZ_CONCEPT_ITERABLE Iterable, class T>
    std::size_t indexOf(const Iterable& iterable, const T& val) {
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
    template<LZ_CONCEPT_ITERATOR Iterator, class UnaryFunc>
    std::size_t indexOfIf(const Iterator begin, const Iterator end, const UnaryFunc predicate) {
        const Iterator pos = std::find_if(begin, end, predicate);
        return pos == end ? npos : static_cast<std::size_t>(std::distance(begin, pos));
    }

    /**
    * Searches iterable with unary predicate `predicate`, and returns its corresponding index, or lz::npos if no such value exists.
    * @tparam Iterable Is automatically deduced.
    * @tparam UnaryFunc Is automatically deduced.
    * @param iterable The sequence to search.
    * @param predicate The search predicate. Uses `std::find_if`.
    * @return The index of the predicate where it returns `true` or lz::npos of no such predicate is present.
    */
    template<LZ_CONCEPT_ITERABLE Iterable, class UnaryFunc>
    std::size_t indexOfIf(const Iterable& iterable, const UnaryFunc predicate) {
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
    template<class UnaryFilterFunc, class UnaryMapFunc, LZ_CONCEPT_ITERATOR Iterator>
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
    template<class UnaryFilterFunc, class UnaryMapFunc, LZ_CONCEPT_ITERABLE Iterable>
    Map<detail::FilterIterator<detail::IterType<Iterable>, UnaryFilterFunc>, UnaryMapFunc>
        filterMap(Iterable&& iterable, const UnaryFilterFunc& filterFunc, const UnaryMapFunc& mapFunc) {
        return filterMap(std::begin(iterable), std::end(iterable), filterFunc, mapFunc);
    }

#endif // End LZ_HAS_EXECUTION
} // End namespace lz

#endif // End LZ_FUNCTION_TOOLS_HPP