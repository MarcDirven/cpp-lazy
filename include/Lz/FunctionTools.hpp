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
#include "Concatenate.hpp"

#ifdef LZ_HAS_CXX17
  #define LZ_INLINE_VAR inline
#else // ^^^ inline var vvv !inline var
  #define LZ_INLINE_VAR
#endif // lz has cxx17

#include <cassert>

namespace lz {
	// ReSharper disable once CppUnnamedNamespaceInHeaderFile
	namespace internal { namespace {
        template<class Iterator>
        using DifferenceType = typename std::iterator_traits<Iterator>::difference_type;

        template<class Iterator>
        using ReferenceIterator = typename std::iterator_traits<Iterator>::reference;

        template<class Iterable>
        using ReferenceIterable = typename std::iterator_traits<IterType<Iterable>>::reference;

        bool stringReplaceImpl(std::string& string, const std::string& oldString, const std::string& newString, const std::size_t startPos,
                               const std::false_type /* replaceAll */) {
            const std::size_t oldStringSize = oldString.length();
            const std::size_t newStringSize = newString.length();

            if (oldStringSize == 0 || newStringSize == 0) {
                return false;
            }

            if (startPos == std::string::npos) {
                return false;
            }

            string.replace(startPos, oldStringSize, newString);
            return true;
        }

        bool stringReplaceImpl(std::string& string, const std::string& oldString, const std::string& newString, std::size_t startPos,
                               const std::true_type /* replaceAll */) {
            if (!stringReplaceImpl(string, oldString, newString, startPos, std::false_type())) {
                return false;
            }

            const std::size_t oldStringSize = oldString.length();
            const std::size_t newStringSize = newString.length();
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

        template<class T>
        auto begin(T&& t) -> decltype(std::begin(t)) {
            return std::begin(t);
        }

        const char* begin(const char* s) {
            return s;
        }

        const char* end(const char* s) {
            return s + std::strlen(s);
        }

        template<std::size_t N>
        char* begin(char (&s)[N]) {
            return s;
        }

        template<std::size_t N>
        char* end(char (&s)[N]) {
            return s + N;
        }
    }} // namespace internal::<anonymous>

    /**
     * This value is returned when indexOf(If) does not find the value specified.
     */
    constexpr LZ_INLINE_VAR std::size_t npos = std::numeric_limits<size_t>::max();

    /**
     * Gets the mean of a sequence.
     * @param begin The beginning of the sequence.
     * @param end The ending of the sequence.
     * @return The mean of the sequence.
     */
    template<LZ_CONCEPT_ITERATOR Iterator>
    double mean(const Iterator begin, const Iterator end) {
        const internal::DifferenceType<Iterator> distance = std::distance(begin, end);
        const internal::ValueTypeIterator<Iterator> sum = std::accumulate(begin, end, internal::ValueTypeIterator<Iterator>(0));
        return static_cast<double>(sum) / distance;
    }

    /**
     * Gets the mean of a sequence.
     * @param container The container to calculate the mean of.
     * @return The mean of the container.
     */
    template<LZ_CONCEPT_ITERABLE Iterable>
    double mean(const Iterable& container) {
        return lz::mean(std::begin(container), std::end(container));
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
#endif // end has string view
    StringSplitter<SubString, String> lines(String&& string) {
        return lz::split<SubString, String>(string, "\n");
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
        return internal::stringReplaceImpl(string, oldString, newString, string.find(oldString), std::false_type());
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
        return internal::stringReplaceImpl(string, oldString, newString, string.find(oldString), std::true_type());
    }

#ifdef LZ_HAS_STRING_VIEW
    template<class... Types>
    auto concatAsStringView(Types&&... types) -> lz::Concatenate<decltype(std::begin(std::string_view(types)))...> {
        static_assert(
            internal::IsAllSame<char, typename std::iterator_traits<decltype(internal::begin(types))>::value_type...>::value,
            "the character type of the strings must use `char`"
        );
        return lz::concat(static_cast<std::string_view>(types)...);
    }
#endif // end lz has string view

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
        return lz::sumTo(0, upToAndIncluding);
    }

    /**
     * The exact opposite of `lines`. It joins a container of `std::string` or `std::string_view` container with `'\n'` as delimiter.
     * @param strings The container of `std::string` or `std::string_view`.
     * @return A Join iterator that joins the strings in the container on `'\n'`.
     */
    template<class Strings, LZ_CONCEPT_ITERATOR Iterator = internal::IterType<Strings>>
    Join<Iterator> unlines(Strings&& strings) {
        static_assert(std::is_same<std::string, internal::ValueTypeIterator<Iterator>>::value
#ifdef LZ_HAS_STRING_VIEW
            || std::is_same<std::string_view, internal::ValueTypeIterator<Iterator>>::value
#endif // Lz has string view
            , "the type of the container should be std::string or std::string_view");
        return lz::join(strings, "\n");
    }

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
     * @param begin The beginning of the sequence
     * @param end The ending of the sequence
     * @param init The starting value.
     * @param selectorFunc Function that specifies what to add to `init`.
     * @return The result of the transAccumulate operation.
     */
    template<LZ_CONCEPT_ITERATOR Iterator, class Init, class SelectorFunc>
#ifdef LZ_HAS_CXX17
    [[deprecated("a similar method is defined in <algorithm>; use std::transform_reduce instead")]]
#endif // end lz has cxx 17
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
     * @param it The container to iterate over.
     * @param init The starting value.
     * @param selectorFunc Function that specifies what to add to `init`.
     * @return The result of the transAccumulate operation.
     */
    template<LZ_CONCEPT_ITERABLE Iterable, class Init, class SelectorFunc>
    Init transAccumulate(const Iterable& it, Init init, const SelectorFunc selectorFunc) {
        return lz::transAccumulate(std::begin(it), std::end(it), std::move(init), selectorFunc);
    }

    /**
     * Returns an iterator that accesses two adjacent elements of one container in a std::tuple<To, To> like fashion.
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
        return lz::zipRange(std::make_tuple(begin, next), std::make_tuple(end, end));
    }

    /**
     * Returns an iterator that accesses two adjacent elements of one container in a std::tuple<To, To> like fashion.
     * @param iterable A container/iterable.
     * @return A zip iterator that accesses two adjacent elements of one container.
     */
    template<LZ_CONCEPT_ITERABLE Iterable, LZ_CONCEPT_ITERATOR Iterator = internal::IterType<Iterable>>
    auto pairwise(Iterable&& iterable) -> Zip<Iterator, Iterator> {
        return lz::pairwise(std::begin(iterable), std::end(iterable));
    }

    /**
     * Returns a view object of which its iterators are reversed.
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
        return lz::takeRange(ReverseIterator(end), ReverseIterator(begin));
    }
    /**
     * Returns a view object of which its iterators are reversed.
     * @param iterable The iterable. The iterable must have at least std::bidirectional_iterator_tag.
     * @return A Take view object contains the reverse order of [begin end)
     */
    template<LZ_CONCEPT_BIDIRECTIONAL_ITERABLE Iterable>
    Take<std::reverse_iterator<internal::IterType<Iterable>>> reverse(Iterable&& iterable) {
        return lz::reverse(std::begin(iterable), std::end(iterable)); // ADL std::reverse
    }

    /**
     * Returns an iterator that constructs type To from the given container. E.g. `lz::as<floats>(...begin(), ...end())` constructs
     * floating points from the given values in the container.
     * @tparam T The type to construct from the elements in the given container
     * @param begin The beginning of the sequence.
     * @param end The ending of the sequence.
     * @return A map iterator that constructs To from each of the elements in the given container.
     */
    template<class T, LZ_CONCEPT_ITERATOR Iterator>
    Map<Iterator, internal::ConvertFn<T>> as(const Iterator begin, const Iterator end) {
        using ValueTypeIterator = internal::ValueTypeIterator<Iterator>;
        static_assert(std::is_convertible<ValueTypeIterator, T>::value, "the value type of the container is not convertible to To");
        return lz::mapRange(begin, end, internal::ConvertFn<T>());
    }

    /**
     * Returns an iterator that constructs type To from the given container. E.g. `lz::as<floats>(container)` constructs
     * floating points from the given values in the container.
     * @tparam T The type to construct from the elements in the given container
     * @tparam Iterable A container/iterable.
     * @return A map iterator that constructs To from each of the elements in the given container.
     */
    template<class T, LZ_CONCEPT_ITERABLE Iterable>
    auto as(Iterable&& iterable) -> Map<decltype(std::begin(iterable)), internal::ConvertFn<T>> {
        return lz::as<T>(std::begin(iterable), std::end(iterable));
    }

    template<LZ_CONCEPT_ITERATOR Iterator>
    internal::DifferenceType<Iterator> length(const Iterator begin, const Iterator end) {
        return std::distance(begin, end);
    }

    /**
     * Gets the length of the sequence.
     * @param iterable The sequence to get the length of.
     * @return The length of the sequence.
     */
    template<LZ_CONCEPT_ITERABLE Iterable>
    auto length(const Iterable& iterable) -> internal::DifferenceType<decltype(std::begin(iterable))> {
        return lz::length(std::begin(iterable), std::end(iterable));
    }

    /**
     * Checks whether [begin, end) is empty.
     * @param begin The beginning of the sequence.
     * @param end The end of the sequence.
     * @return True if it is empty, false otherwise.
     */
    template<LZ_CONCEPT_ITERATOR Iterator>
    bool isEmpty(const Iterator begin, const Iterator end) {
        return begin == end;
    }

    /**
     * Checks whether `iterable` is empty.
     * @param iterable The sequence to check whether it is empty yes/no.
     * @return True if it is empty, false otherwise.
     */
    template<LZ_CONCEPT_ITERABLE Iterable>
    bool isEmpty(const Iterable& iterable) {
        return lz::isEmpty(std::begin(iterable), std::end(iterable));
    }

    /**
     * Returns whether the sequence holds exactly one element.
     * @param begin The beginning of the sequence.
     * @param end The end of the sequence.
     * @return True if it has one element exactly, false otherwise.
     */
    template<LZ_CONCEPT_ITERATOR Iterator>
    bool hasOne(Iterator begin, const Iterator end) {
        return !lz::isEmpty(begin, end) && begin++ == end;
    }

    /**
     * Returns whether the sequence holds exactly one element.
     * @param iterable The sequence to check whether is has exactly one element.
     * @return True if it has one element exactly, false otherwise.
     */
    template<LZ_CONCEPT_ITERABLE Iterable>
    bool hasOne(const Iterable& iterable) {
        return lz::hasOne(std::begin(iterable), std::end(iterable));
    }

    /**
     * Returns whether the sequence holds >= 2 elements.
     * @param begin The beginning of the sequence.
     * @param end The end of the sequence.
     * @return True if the amount of values are >= 2, false otherwise.
     */
    template<LZ_CONCEPT_ITERATOR Iterator>
    bool hasMany(const Iterator begin, const Iterator end) {
        return !lz::isEmpty(begin, end) && !lz::hasOne(begin, end);
    }

    /**
     * Returns whether the sequence holds >= 2 elements.
     * @param iterable The sequence to check whether it has many (>= 2) elements.
     * @return True if the amount of values are >= 2, false otherwise.
     */
    template<LZ_CONCEPT_ITERABLE Iterable>
    bool hasMany(const Iterable& iterable) {
        return !lz::isEmpty(iterable) && !lz::hasOne(iterable);
    }

    /**
     * Returns the first element. Asserts if the sequence is empty.
     * @param begin The beginning of the sequence.
     * @param end The end of the sequence.
     * @return The first element of the sequence (by reference).
     */
    template<LZ_CONCEPT_ITERATOR Iterator>
    internal::ReferenceIterator<Iterator> first(const Iterator begin, const Iterator end) {
        assert(!lz::isEmpty(begin, end) && "sequence cannot be empty in order to get the first element");
        return *begin;
    }

    /**
     * Returns the first element. Asserts if the sequence is empty.
     * @param iterable The sequence to get the first element of.
     * @return The first element of the sequence (by reference).
     */
    template<LZ_CONCEPT_ITERABLE Iterable>
    internal::ReferenceIterable<Iterable> first(Iterable&& iterable) {
        return lz::first(std::begin(iterable), std::end(iterable));
    }

    /**
     * Returns the last element. Asserts if the sequence is empty.
     * @param begin The beginning of the sequence.
     * @param end The end of the sequence.
     * @return The last element of the sequence (by reference i.e. type of *iterator).
     */
    template<LZ_CONCEPT_ITERATOR Iterator>
    internal::ReferenceIterator<Iterator> last(Iterator begin, const Iterator end) {
        assert(!lz::isEmpty(begin, end) && "sequence cannot be empty in order to get the last element");
        const internal::DifferenceType<Iterator> len = lz::length(begin, end);
        std::advance(begin, len - 1);
        return *begin;
    }

    /**
     * Returns the last element. Asserts if the sequence is empty.
     * @param iterable The sequence to get the last element of.
     * @return The last element of the sequence (by reference).
     */
    template<LZ_CONCEPT_ITERABLE Iterable>
    typename internal::IterType<Iterable>::reference last(Iterable&& iterable) {
        return lz::last(std::begin(iterable), std::end(iterable));
    }

    template<LZ_CONCEPT_ITERATOR Iterator, class T>
    internal::ValueTypeIterator<Iterator> firstOr(const Iterator begin, const Iterator end, const T& defaultValue) {
        return lz::isEmpty(begin, end) ? defaultValue : lz::first(begin, end);
    }

    /**
     * Gets the first element of the sequence, or `value` if the sequence is empty.
     * @param iterable The iterable to get the first value of, or `value` in case it is empty.
     * @param value The value to return if `iterable` is empty.
     * @return Either the first element of `iterable` or `value` if the sequence is emtpy.
     */
    template<LZ_CONCEPT_ITERABLE Iterable, class T>
    internal::ValueTypeIterable<Iterable> firstOr(const Iterable& iterable, const T& value) {
        return lz::firstOr(std::begin(iterable), std::end(iterable), value);
    }

    template<LZ_CONCEPT_ITERATOR Iterator, class T>
    internal::ValueTypeIterator<Iterator> lastOr(const Iterator begin, const Iterator end, const T& value) {
        return lz::isEmpty(begin, end) ? value : lz::last(begin, end);
    }

    /**
     * This function returns the last element. If the sequence is empty, it returns `value`.
     * @param iterable The iterable to get the last value of, or `value` in case it is empty.
     * @param value The value to return if `iterable` is empty.
     * @return Either the last element of `iterable` or `value` if the sequence is emtpy.
     */
    template<LZ_CONCEPT_ITERABLE Iterable, class T>
    internal::ValueTypeIterable<Iterable> lastOr(const Iterable& iterable, const T& value) {
        return lz::lastOr(std::begin(iterable), std::end(iterable), value);
    }

#ifdef LZ_HAS_EXECUTION
    /**
     * Creates a map object with filter iterator that, if the filter function returns true, the map function is executed.
     * @param begin The beginning of the sequence.
     * @param end The ending of the sequence.
     * @param filterFunc The function that filters the elements. If this function returns `true`, its corresponding container value is
     * passed to the `mapFunc`.
     * @param mapFunc The function that returns the (new) type.
     * @param execPolicy TThe execution policy. Must be one of `std::execution`'s tags. Performs the find using this execution.
     * @return A map object that can be iterated over. The `value_type` of the this view object is equal to the return value of `mapFunc`.
     */
    template<class Execution = std::execution::sequenced_policy, class UnaryFilterFunc, class UnaryMapFunc, LZ_CONCEPT_ITERATOR Iterator>
    Map<internal::FilterIterator<Execution, Iterator, UnaryFilterFunc>, UnaryMapFunc>
	filterMap(const Iterator begin, const Iterator end, const UnaryFilterFunc& filterFunc, const UnaryMapFunc& mapFunc,
            const Execution execPolicy = std::execution::seq) {
        static_assert(std::is_execution_policy_v<Execution>, "Execution must be of type std::execution::...");

        Filter<Execution, Iterator, UnaryFilterFunc> filterView = filterRange(begin, end, filterFunc, execPolicy);
        return lz::map(filterView, mapFunc);
    }

    /**
     * Creates a map object with filter iterator that, if the filter function returns true, the map function is executed.
     * @param iterable The iterable to filter/map.
     * @param filterFunc The function that filters the elements. If this function returns `true`, its corresponding container value is
     * passed to the `mapFunc`.
     * @param mapFunc The function that returns the (new) type.
     * @param execution TThe execution policy. Must be one of `std::execution`'s tags. Performs the find using this execution.
     * @return A map object that can be iterated over. The `value_type` of the this view object is equal to the return value of `mapFunc`.
     */
    template<class Execution = std::execution::sequenced_policy, class UnaryFilterFunc, class UnaryMapFunc, LZ_CONCEPT_ITERABLE Iterable>
    Map<internal::FilterIterator<Execution, internal::IterType<Iterable>, UnaryFilterFunc>, UnaryMapFunc>
	filterMap(Iterable&& iterable, const UnaryFilterFunc& filterFunc, const UnaryMapFunc& mapFunc,
            const Execution execution = std::execution::seq) {
        return lz::filterMap(std::begin(iterable), std::end(iterable), filterFunc, mapFunc, execution);
    }

    /**
     * Gets the median of a sequence.
     * @param begin The beginning of the sequence
     * @param end The ending of the sequence
     * @param compare The sequence gets sorted with nth_element. A default operator of < is used, however a custom compare can be used.
     * @param execution Uses the execution to perform the nth_element algorithm + the std::max element if the length of the sequence is
     * even.
     * @return The median of the sequence.
     */
    template<class Execution = std::execution::sequenced_policy, LZ_CONCEPT_ITERATOR Iterator, class Compare>
    double median(const Iterator begin, const Iterator end, const Compare compare, const Execution execution = std::execution::seq) {
        const internal::DifferenceType<Iterator> len = std::distance(begin, end);
        if (len == 0) {
            throw std::invalid_argument(LZ_FILE_LINE ": the length of the sequence cannot be 0");
        }

        const internal::DifferenceType<Iterator> mid = len >> 1;
        const Iterator midIter = std::next(begin, mid);

        std::nth_element(execution, begin, midIter, end, compare);

        if (internal::isEven(len)) {
            const Iterator leftHalf = std::max_element(execution, begin, midIter);
            return (static_cast<double>(*leftHalf) + *midIter) / 2.;
        }
        return *midIter;
    }

    /**
     * Gets the median of a sequence.
     * @param iterable The container/sequence by reference.
     * @param compare The sequence gets sorted with nth_element. A default operator of < is used, however a custom compare can be used.
     * @param execution Uses the execution to perform the nth_element algorithm + the std::max element if the length of the sequence is
     * even.
     * @return The median of the sequence.
     */
    template<class Execution = std::execution::sequenced_policy, LZ_CONCEPT_ITERABLE Iterable, class Compare>
    double median(Iterable& iterable, const Compare compare, const Execution execution = std::execution::seq) {
        return lz::median(std::begin(iterable), std::end(iterable), compare, execution);
    }

    /**
     * Gets the median of a sequence.
     * @param begin The beginning of the sequence
     * @param end The ending of the sequence
     * @param execution Uses the execution to perform the nth_element algorithm + the std::max element if the length of the sequence is
     * even.
     * @return The median of the sequence.
     */
    template<class Execution = std::execution::sequenced_policy, LZ_CONCEPT_ITERATOR Iterator>
    double median(const Iterator begin, const Iterator end, const Execution execution = std::execution::seq) {
        return lz::median(begin, end, std::less<internal::ValueTypeIterator<Iterator>>(), execution);
    }

    /**
     * Gets the median of a sequence.
     * @param iterable The container/sequence by reference.
     * @param execution Uses the execution to perform the nth_element algorithm + the std::max element if the length of the sequence is
     * even.
     * @return The median of the sequence.
     */
    template<class Execution = std::execution::sequenced_policy, LZ_CONCEPT_ITERABLE Iterable>
    double median(Iterable& iterable, const Execution execution = std::execution::seq) {
        return lz::median(std::begin(iterable), std::end(iterable), std::less<internal::ValueTypeIterable<Iterable>>(), execution);
    }

    /**
     * Checks if `toFind` is in the sequence [begin, end). If so, it returns `toFind`, otherwise it returns `defaultValue`.
     * @param begin The beginning of the sequence.
     * @param end The ending of the sequence.
     * @param toFind The value to find. One can use `std::move(defaultValue)` to avoid copies.
     * @param defaultValue The value to return if `toFind` is not found.
     * @param execution Uses the execution to perform the find.
     * @return Either `toFind` or `defaultValue`.
     */
    template<class Execution = std::execution::sequenced_policy, LZ_CONCEPT_ITERATOR Iterator, class T, class U>
    internal::ValueTypeIterator<Iterator>
    findOrDefault(const Iterator begin, const Iterator end, T&& toFind, U&& defaultValue, const Execution execution = std::execution::seq) {
        using ValueType = internal::ValueTypeIterator<Iterator>;
        return static_cast<ValueType>(std::find(execution, begin, end, toFind) == end ? defaultValue : toFind);
    }

    /**
     * Checks if `toFind` is in the sequence [begin, end). If so, it returns `toFind`, otherwise it returns `defaultValue`.
     * @param iterable The iterable to search.
     * @param toFind The value to find. One can use `std::move(defaultValue)` to avoid copies.
     * @param defaultValue The value to return if `toFind` is not found.
     * @param execution Uses the execution to perform the find.
     * @return Either `toFind` or `defaultValue`.
     */
    template<class Execution = std::execution::sequenced_policy, LZ_CONCEPT_ITERABLE Iterable, class T, class U>
    internal::ValueTypeIterable<Iterable> findOrDefault(const Iterable& iterable, T&& toFind, U&& defaultValue,
                                                        const Execution execution = std::execution::seq) {
        return lz::findOrDefault(std::begin(iterable), std::end(iterable), toFind, defaultValue, execution);
    }

    /**
     * Uses `std::find_if(begin, end, predicate)` to check if for one of the elements, `predicate` returns true. If so, its corresponding
     * value is returned. If not, `defaultValue` is returned.
     * @param begin The beginning of the sequence.
     * @param end The ending of the sequence.
     * @param predicate A function with one argument that returns a bool for its corresponding value type of the iterator.
     * (see `std::find_if`)
     * @param defaultValue The default value to return if predicate is `false` for all elements.
     * @param execution Uses the execution to perform the find.
     * @return Either the element that has been found by `predicate` or `defaultValue` if no such item exists.
     */
    template<class Execution = std::execution::sequenced_policy, LZ_CONCEPT_ITERATOR Iterator, class UnaryPredicate, class T>
    internal::ValueTypeIterator<Iterator> findOrDefaultIf(const Iterator begin, const Iterator end, const UnaryPredicate predicate,
                                                          T&& defaultValue, const Execution execution) {
        using ValueType = internal::ValueTypeIterator<Iterator>;
        const Iterator pos = std::find_if(execution, begin, end, predicate);
        return static_cast<ValueType>(pos == end ? defaultValue : *pos);
    }

    /**
     * Uses `std::find_if(begin, end, predicate)` to check if for one of the elements, `predicate` returns true. If so, its corresponding
     * value is returned. If not, `defaultValue` is returned.
     * @param iterable The sequence to search.
     * @param predicate A function with one argument that returns a bool for its corresponding value type of the iterator.
     * (see `std::find_if`)
     * @param defaultValue The default value to return if predicate is `false` for all elements.
     * @param execution Uses the execution to perform the find.
     * @return Either the element that has been found by `predicate` or `defaultValue` if no such item exists.
     */
    template<class Execution = std::execution::sequenced_policy, LZ_CONCEPT_ITERABLE Iterable, class T, class UnaryPredicate,
        class ValueType = internal::ValueTypeIterable<Iterable>>
    internal::ValueTypeIterable<Iterable> findOrDefaultIf(const Iterable& iterable, const UnaryPredicate predicate, T&& defaultValue,
                                                          const Execution execution = std::execution::seq) {
        return lz::findOrDefaultIf(std::begin(iterable), std::end(iterable), predicate, defaultValue, execution);
    }

    /**
     * Searches [begin, end) for val, and returns its corresponding index, or lz::npos if no such value exists.
     * @param begin The beginning of the sequence.
     * @param end The ending of the sequence.
     * @param val The value to search.
     * @param execution Uses the execution to perform the find.
     * @return The index of `val` or lz::npos of no such value exists.
     */
    template<class Execution = std::execution::sequenced_policy, LZ_CONCEPT_ITERATOR Iterator, class T>
    std::size_t indexOf(const Iterator begin, const Iterator end, const T& val, const Execution execution = std::execution::seq) {
        const Iterator pos = std::find(execution, begin, end, val);
        return pos == end ? npos : static_cast<size_t>(std::distance(begin, pos));
    }

    /**
     * Searches `iterable` for val, and returns its corresponding index, or lz::npos if no such value exists.
     * @param iterable The iterable to search.
     * @param val The value to search.
     * @param execution Uses the execution to perform the find.
     * @return The index of `val` or lz::npos of no such value exists.
     */
    template<class Execution = std::execution::sequenced_policy, LZ_CONCEPT_ITERABLE Iterable, class To>
    std::size_t indexOf(const Iterable& iterable, const To& val, const Execution execution = std::execution::seq) {
        return lz::indexOf(std::begin(iterable), std::end(iterable), val, execution);
    }

    /**
    * Searches [begin, end) with unary predicate `predicate`, and returns its corresponding index, or lz::npos if no such value exists.
    * @param begin The beginning of the sequence.
    * @param end The ending of the sequence.
    * @param predicate The search predicate. Uses `std::find_if`.
    * @param execution Uses the execution to perform the find.
    * @return The index of the predicate where it returns `true` or lz::npos of no such predicate is present.
    */
    template<class Execution = std::execution::sequenced_policy, LZ_CONCEPT_ITERATOR Iterator, class UnaryFunc>
    std::size_t indexOfIf(const Iterator begin, const Iterator end, const UnaryFunc predicate,
                          const Execution execution = std::execution::seq) {
        const Iterator pos = std::find_if(execution, begin, end, predicate);
        return pos == end ? npos : static_cast<std::size_t>(std::distance(begin, pos));
    }

    /**
    * Searches iterable with unary predicate `predicate`, and returns its corresponding index, or lz::npos if no such value exists.
    * @param iterable The sequence to search.
    * @param predicate The search predicate. Uses `std::find_if`.
    * @return The index of the predicate where it returns `true` or lz::npos of no such predicate is present.
    */
    template<class Execution = std::execution::sequenced_policy, LZ_CONCEPT_ITERABLE Iterable, class UnaryFunc>
    std::size_t indexOfIf(const Iterable& iterable, const UnaryFunc predicate, const Execution execution = std::execution::seq) {
        return lz::indexOfIf(std::begin(iterable), std::end(iterable), predicate, execution);
    }

    /**
     * Check whether `iterable` contains `value`.
     * @param begin The beginning of the sequence to search.
     * @param end The ending of the sequence to search.
     * @param value The value to find.
     * @return True if `iterable` contains `value`, false otherwise.
     */
    template<LZ_CONCEPT_ITERATOR Iterator, class T, class Execution = std::execution::sequenced_policy>
    bool contains(const Iterator begin, const Iterator end, const T& value, const Execution exec = std::execution::seq) {
        return std::find(exec, begin, end, value) != end;
    }

    /**
     * Check whether `iterable` contains `value`.
     * @param iterable The iterable to search.
     * @param value The value to find.
     * @return True if `iterable` contains `value`, false otherwise.
     */
    template<LZ_CONCEPT_ITERABLE Iterable, class T, class Execution = std::execution::sequenced_policy>
    bool contains(const Iterable& iterable, const T& value, const Execution exec = std::execution::seq) {
        return contains(std::begin(iterable), std::end(iterable), value, exec);
    }

    /**
     * Searches [begin, end) with a given `predicate`. If `predicate` returns true, `iterable` contains that predicate, false otherwise.
     * @param begin The beginning of the sequence to search.
     * @param end The ending of the sequence to search.
     * @param predicate The search predicate.
     * @return Returns true if `predicate` returns true, false otherwise.
     */
    template<LZ_CONCEPT_ITERATOR Iterator, class T, class BinaryPredicate, class Execution = std::execution::sequenced_policy>
    bool containsIf(const Iterator begin, const Iterator end, const BinaryPredicate predicate, const Execution exec = std::execution::seq) {
        return std::find_if(exec, begin, end, predicate) != end;
    }

    /**
     * Searches iterable with a given `predicate`. If `predicate` returns true, `iterable` contains that predicate, false otherwise.
     * @tparam BinaryPredicate A binary predicate functor.
     * @param iterable The iterable to search.
     * @param predicate The search predicate.
     * @return Returns true if `predicate` returns true, false otherwise.
     */
    template<LZ_CONCEPT_ITERABLE Iterable, class T, class BinaryPredicate, class Execution = std::execution::sequenced_policy>
    bool containsIf(const Iterable& iterable, const BinaryPredicate predicate, const Execution exec = std::execution::seq) {
        return containsIf(std::begin(iterable), std::end(iterable), predicate, exec);
    }
#else // ^^^ Lz has execution vvv !Lz has execution

    /**
     * Gets the median of a sequence.
     * @param begin The beginning of the sequence
     * @param end The ending of the sequence
     * @param compare The sequence gets sorted with nth_element. A default operator of < is used, however a custom compare can be used.
     * @return The median of the sequence.
     */
    template<LZ_CONCEPT_ITERATOR Iterator, class Compare>
    double median(const Iterator begin, const Iterator end, const Compare compare) {
        const internal::DifferenceType<Iterator> len = std::distance(begin, end);
        if (len == 0) {
            throw std::invalid_argument(LZ_FILE_LINE ": the length of the sequence cannot be 0");
        }

        const internal::DifferenceType<Iterator> mid = len >> 1;
        const Iterator midIter = std::next(begin, mid);
        std::nth_element(begin, midIter, end, compare);

        if (internal::isEven(len)) {
            const Iterator leftHalf = std::max_element(begin, midIter);
            return (static_cast<double>(*leftHalf) + *midIter) / 2.;
        }
        return *midIter;
    }

    /**
     * Gets the median of a sequence.
     * @param iterable The container/sequence by reference.
     * @param compare The sequence gets sorted with nth_element. A default operator of < is used, however a custom compare can be used.
     * @return The median of the sequence.
     */
    template<LZ_CONCEPT_ITERABLE Iterable, class Compare>
    double median(Iterable& iterable, const Compare compare) {
        return lz::median(std::begin(iterable), std::end(iterable), compare);
    }

    /**
     * Gets the median of a sequence.
     * @param begin The beginning of the sequence
     * @param end The ending of the sequence
     * @return The median of the sequence.
     */
    template<LZ_CONCEPT_ITERATOR Iterator>
    double median(const Iterator begin, const Iterator end) {
        return lz::median(begin, end, std::less<internal::ValueTypeIterator<Iterator>>());
    }

    /**
     * Gets the median of a sequence.
     * @param iterable The container/sequence by reference.
     * @return The median of the sequence.
     */
    template<LZ_CONCEPT_ITERABLE Iterable>
    double median(Iterable& iterable) {
        return lz::median(std::begin(iterable), std::end(iterable), std::less<internal::ValueTypeIterable<Iterable>>());
    }

    /**
     * Checks if `toFind` is in the sequence [begin, end). If so, it returns `toFind`, otherwise it returns `defaultValue`.
     * @param begin The beginning of the sequence.
     * @param end The ending of the sequence.
     * @param toFind The value to find. One can use `std::move(defaultValue)` to avoid copies.
     * @param defaultValue The value to return if `toFind` is not found.  One can use `std::move(toFind)` to avoid copies.
     * @return Either `toFind` or `defaultValue`.
     */
    template<LZ_CONCEPT_ITERATOR Iterator, class T, class U>
    internal::ValueTypeIterator<Iterator> findOrDefault(const Iterator begin, const Iterator end, T&& toFind, U&& defaultValue) {
        using CastType = internal::ValueTypeIterator<Iterator>;
        return static_cast<CastType>(std::find(begin, end, toFind) == end ? defaultValue : toFind);
    }

    /**
     * Checks if `toFind` is in the sequence [begin, end). If so, it returns `toFind`, otherwise it returns `defaultValue`.
     * @param iterable The iterable to search.
     * @param toFind The value to find. One can use `std::move(defaultValue)` to avoid copies.
     * @param defaultValue The value to return if `toFind` is not found.  One can use `std::move(toFind)` to avoid copies.
     * @return Either `toFind` or `defaultValue`.
     */
    template<LZ_CONCEPT_ITERABLE Iterable, class T, class U>
    internal::ValueTypeIterable<Iterable> findOrDefault(const Iterable& iterable, T&& toFind, U&& defaultValue) {
        return lz::findOrDefault(std::begin(iterable), std::end(iterable), toFind, defaultValue);
    }

    /**
     * Uses `std::find_if(begin, end, predicate)` to check if for one of the elements, `predicate` returns true. If so, its corresponding
     * value is returned. If not, `defaultValue` is returned.
     * @param begin The beginning of the sequence.
     * @param end The ending of the sequence.
     * @param predicate A function with one argument that returns a bool for its corresponding value type of the iterator.
     * (see `std::find_if`)
     * @param defaultValue The default value to return if predicate is `false` for all elements. Use `std::move` to avoid copies.
     * @return Either the element that has been found by `predicate` or `defaultValue` if no such item exists.
     */
    template<LZ_CONCEPT_ITERATOR Iterator, class T, class UnaryPredicate>
    internal::ValueTypeIterator<Iterator> findOrDefaultIf(const Iterator begin, const Iterator end, const UnaryPredicate predicate,
                                                          T&& defaultValue) {
        using CastType = internal::ValueTypeIterator<Iterator>;
        const Iterator pos = std::find_if(begin, end, predicate);
        return static_cast<CastType>(pos == end ? defaultValue : *pos);
    }

    /**
     * Uses `std::find_if(begin, end, predicate)` to check if for one of the elements, `predicate` returns true. If so, its corresponding
     * value is returned. If not, `defaultValue` is returned.
     * @param iterable The sequence to search.
     * @param predicate A function with one argument that returns a bool for its corresponding value type of the iterator.
     * (see `std::find_if`)
     * @param defaultValue The default value to return if predicate is `false` for all elements. Use `std::move` to avoid copies.
     * @return Either the element that has been found by `predicate` or `defaultValue` if no such item exists.
     */
    template<LZ_CONCEPT_ITERABLE Iterable, class T, class UnaryPredicate>
    internal::ValueTypeIterable<Iterable> findOrDefaultIf(const Iterable& iterable, const UnaryPredicate predicate, T&& defaultValue) {
        return lz::findOrDefaultIf(std::begin(iterable), std::end(iterable), predicate, defaultValue);
    }

    /**
     * Searches [begin, end) for val, and returns its corresponding index, or lz::npos if no such value exists.
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
    * @param iterable The iterable to search.
    * @param val The value to search.
    * @return The index of `val` or lz::npos of no such value exists.
    */
    template<LZ_CONCEPT_ITERABLE Iterable, class T>
    std::size_t indexOf(const Iterable& iterable, const T& val) {
        return lz::indexOf(std::begin(iterable), std::end(iterable), val);
    }

    /**
    * Searches [begin, end) with unary predicate `predicate`, and returns its corresponding index, or lz::npos if no such value exists.
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
    * @param iterable The sequence to search.
    * @param predicate The search predicate. Uses `std::find_if`.
    * @return The index of the predicate where it returns `true` or lz::npos of no such predicate is present.
    */
    template<LZ_CONCEPT_ITERABLE Iterable, class UnaryFunc>
    std::size_t indexOfIf(const Iterable& iterable, const UnaryFunc predicate) {
        return lz::indexOfIf(std::begin(iterable), std::end(iterable), predicate);
    }


    /**
     * Check whether `iterable` contains `value`.
     * @param begin The beginning of the sequence to search.
     * @param end The ending of the sequence to search.
     * @param value The value to find.
     * @return True if `iterable` contains `value`, false otherwise.
     */
    template<LZ_CONCEPT_ITERATOR Iterator, class T>
    bool contains(const Iterator begin, const Iterator end, const T& value) {
        return std::find(begin, end, value) != end;
    }

    /**
     * Check whether `iterable` contains `value`.
     * @param iterable The iterable to search.
     * @param value The value to find.
     * @return True if `iterable` contains `value`, false otherwise.
     */
    template<LZ_CONCEPT_ITERABLE Iterable, class T>
    bool contains(const Iterable& iterable, const T& value) {
        return contains(std::begin(iterable), std::end(iterable), value);
    }

    /**
     * Searches [begin, end) with a given `predicate`. If `predicate` returns true, `iterable` contains that predicate, false otherwise.
     * @tparam BinaryPredicate A binary predicate functor.
     * @param begin The beginning of the sequence to search.
     * @param end The ending of the sequence to search.
     * @param predicate The search predicate.
     * @return Returns true if `predicate` returns true, false otherwise.
     */
    template<LZ_CONCEPT_ITERATOR Iterator, class T, class BinaryPredicate>
    bool containsIf(const Iterator begin, const Iterator end, const BinaryPredicate predicate) {
        return std::find_if(begin, end, predicate) != end;
    }

    /**
     * Searches iterable with a given `predicate`. If `predicate` returns true, `iterable` contains that predicate, false otherwise.
     * @tparam BinaryPredicate A binary predicate functor.
     * @param iterable The iterable to search.
     * @param predicate The search predicate.
     * @return Returns true if `predicate` returns true, false otherwise.
     */
    template<LZ_CONCEPT_ITERABLE Iterable, class T, class BinaryPredicate>
    bool containsIf(const Iterable& iterable, const BinaryPredicate predicate) {
        return containsIf(std::begin(iterable), std::end(iterable), predicate);
    }

    /**
     * Creates a map object with filter iterator that, if the filter function returns true, the map function is executed.
     * @param begin The beginning of the sequence.
     * @param end The ending of the sequence.
     * @param filterFunc The function that filters the elements. If this function returns `true`, its corresponding container value is
     * passed to the `mapFunc`.
     * @param mapFunc The function that returns the (new) type.
     * @return A map object that can be iterated over. The `value_type` of the this view object is equal to the return value of `mapFunc`.
     */
    template<class UnaryFilterFunc, class UnaryMapFunc, LZ_CONCEPT_ITERATOR Iterator>
    Map<internal::FilterIterator<Iterator, UnaryFilterFunc>, UnaryMapFunc>
	filterMap(const Iterator begin, const Iterator end, const UnaryFilterFunc& filterFunc, const UnaryMapFunc& mapFunc) {
        Filter<Iterator, UnaryFilterFunc> filterView = filterRange(begin, end, filterFunc);
        return lz::map(filterView, mapFunc);
    }

    /**
     * Creates a map object with filter iterator that, if the filter function returns true, the map function is executed.
     * @param iterable The iterable to filter/map.
     * @param filterFunc The function that filters the elements. If this function returns `true`, its corresponding container value is
     * passed to the `mapFunc`.
     * @param mapFunc The function that returns the (new) type.
     * @return A map object that can be iterated over. The `value_type` of the this view object is equal to the return value of `mapFunc`.
     */
    template<class UnaryFilterFunc, class UnaryMapFunc, LZ_CONCEPT_ITERABLE Iterable>
    Map<internal::FilterIterator<internal::IterType<Iterable>, UnaryFilterFunc>, UnaryMapFunc>
	filterMap(Iterable&& iterable, const UnaryFilterFunc& filterFunc, const UnaryMapFunc& mapFunc) {
        return lz::filterMap(std::begin(iterable), std::end(iterable), filterFunc, mapFunc);
    }

#endif // End LZ_HAS_EXECUTION
} // End namespace lz

#endif // End LZ_FUNCTION_TOOLS_HPP